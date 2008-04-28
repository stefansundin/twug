#include "NetworkManager.h"

NetworkManager::NetworkManager(UIEventQueue* p_to_ui, UIEventQueue* p_to_network)
{
	m_to_network = p_to_network;

	m_readfd = p_to_network->getReadFd();
	m_sending = false;

	m_events = new EventsToUI(p_to_ui);

//	m_socket = m_client_network.getSocket(); // socket file descriptor 

    m_data.record_buffer.frameIndex=0;
    m_data.record_buffer.maxFrameIndex=60*SAMPLE_RATE;
	m_data.record_buffer.tx_pos=0;
	m_data.record_buffer.size=sizeof(SAMPLE)*NUM_CHANNELS*m_data.record_buffer.maxFrameIndex;
	m_data.record_buffer.go=0;
	m_data.record_buffer.samples=new SAMPLE[m_data.record_buffer.size];
	
    m_data.num_play_buffers=1;
	m_data.play_buffers=new paBuffer[m_data.num_play_buffers];
	for (int i=0; i < m_data.num_play_buffers; i++) {
		m_data.play_buffers[i].frameIndex=0;
		m_data.play_buffers[i].maxFrameIndex=0;
		m_data.play_buffers[i].tx_pos=0;
		m_data.play_buffers[i].size=60*sizeof(SAMPLE)*NUM_CHANNELS*SAMPLE_RATE;
		m_data.play_buffers[i].go=0;
		m_data.play_buffers[i].samples=new SAMPLE[m_data.play_buffers[i].size];
	}
	
	//Start portaudio
	PaError err;
	err = Pa_Initialize();
	if (err != paNoError)
	{
		fprintf(stderr,"Pa_Initialize() error: %d\n",err);
		m_events->pushEvent(UIEvent("NOTIFICATION", "Audio", "Initialization failed\nPa_Initialize() failed"));
		return;
	}
	
	PaDeviceIndex device = Pa_GetDefaultInputDevice();
	if (device == paNoDevice) {
		fprintf(stderr,"Pa_GetDefaultInputDevice() error: paNoDevice\n");
		m_events->pushEvent(UIEvent("NOTIFICATION", "Audio", "Initialization failed\nPa_GetDefaultInputDevice() returned paNoDevice"));
		return;
	}
	m_streamParameters.device = device;
	m_streamParameters.channelCount = NUM_CHANNELS;
	m_streamParameters.sampleFormat = PA_SAMPLE_TYPE;
	m_streamParameters.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency;
	m_streamParameters.hostApiSpecificStreamInfo = NULL;
	err = Pa_OpenStream(
		&m_stream,
		&m_streamParameters,
		&m_streamParameters,
		SAMPLE_RATE,
		FRAMES_PER_BUFFER,
		paClipOff,
		this->audioCallback,
		this);
	if (err != paNoError)
	{
		fprintf(stderr,"Pa_OpenStream() error: %d\n",err);
		m_events->pushEvent(UIEvent("NOTIFICATION", "Audio", "Initialization failed\nPa_OpenStream() failed"));
		return;
	}
	
	err = Pa_StartStream(m_stream);
	if (err != paNoError)
	{
		fprintf(stderr,"Pa_StartStream() error: %d\n",err);
		m_events->pushEvent(UIEvent("NOTIFICATION", "Audio", "Initialization failed\nPa_StartStream() failed"));
		return;
	}
}

NetworkManager::~NetworkManager ()
{
	::close(m_readfd);
	
	PaError err;
	err = Pa_CloseStream(m_stream);
	if (err != paNoError) fprintf(stderr,"Pa_CloseStream() error: %d\n",err);
	
	err = Pa_Terminate();
	if (err != paNoError) fprintf(stderr,"Pa_Terminate() error: %d\n",err);
	
	delete[] m_data.record_buffer.samples;
	for (int i=0; i < m_data.num_play_buffers; i++) {
		delete[] m_data.play_buffers[i].samples;
	}
	delete[] m_data.play_buffers;
}


void NetworkManager::run()
{
	bool last_connected_status = false;
	while(true) // thread main loop
	{
//		//print_me("start of network thread loop");

		bool new_connection_status = m_client_network.getConnectionStatus();

		if (last_connected_status && !new_connection_status) //means if we where connected last time and not this time
		{
			m_events->pushEvent(UIEvent("NEW_CONNECTION_STATUS", "CONNECTION_LOST", m_connected_to));
		}

		struct timeval tv;
		fd_set read;
		FD_ZERO(&read);

		FD_SET(m_readfd, &read);

		if(!new_connection_status) // if we arent connected only select on readfd
		{
			tv.tv_sec = 1024;
			tv.tv_usec = 0;

			//print_me("selecting on readfd");
			int select_returned = select(m_readfd+1, &read, NULL, NULL, &tv);
			//print_me("selected on readfd");
			if(select_returned == -1)
			{
				report_error(strerror(errno));
			}

		} else { //selecting on both fds

			FD_SET(m_socket, &read);

			if(m_sending) // means we need to wake up more often and send audio data
			{
				tv.tv_sec = 0;
				tv.tv_usec = (int)1e6/(SAMPLE_RATE/FRAMES_PER_BUFFER);
				
				//send audio
				paBuffer *buffer=&m_data.record_buffer;
				int bytestosend=sizeof(SAMPLE)*NUM_CHANNELS*buffer->frameIndex-buffer->tx_pos;
				if (bytestosend > 0)
				{
					m_client_network.sendAudioData((char*)buffer->samples+buffer->tx_pos, bytestosend);
					buffer->tx_pos+=bytestosend;
				}
			} else {
				tv.tv_sec = 1024;
				tv.tv_usec = 0;
			}		

			int highest;
			if (m_socket > m_readfd) {
				highest = m_socket+1;
			} else {
				highest = m_readfd+1;
			}

			//print_me("selecting on both fds");
			int select_returned = select(highest, &read, NULL, NULL, &tv);
			if(select_returned == -1)
			{
				report_error(strerror(errno));
			}
			//print_me("selected on both fds");

			if(FD_ISSET(m_socket, &read))
			{
				//print_me("got network data");
				processNetworkEvents();
				//print_me("processed network data");
			}
			else
			{
				//print_me("not set, reading anyway");
				processNetworkEvents();
			}
		}

		if(FD_ISSET(m_readfd, &read))
		{
			//print_me("got pipe data");
			char buf[100];
			::read(m_readfd, buf, 100);
			processUIEvents();
			//print_me("processed UI events");
		}

		last_connected_status = new_connection_status;	
	}
}

void NetworkManager::processNetworkEvents()
{
	if(!m_client_network.processNetworking())
	{
		//print_me("Got disconnected");
	}
	else
	{
		//print_me("Processed networking");
	}

	Message incoming_message;
	while(m_client_network.getMessage(incoming_message))
	{
		handleNetworkMessage(incoming_message);
		//print_me("Got Message");
	}
	//print_me("Got all available messages");
}
void NetworkManager::processUIEvents()
{
	//print_me("Processing events from UI");

	bool process = true;
	while(process)
	{
		UIEvent event = m_to_network->popEvent(); // pop an event

		if (event.getType() == "EMPTY") {
			process = false;
		} else if (event.getType() == "JOINCHANNEL") {
			joinChannel(event.pop());
		} else if (event.getType() == "NEWCHANNEL") {
			m_client_network.sendCreateChannelRequest(event.pop());
		} else if (event.getType() == "REMOVECHANNEL") {
			m_client_network.sendRemoveChannelRequest(event.pop());
		} else if (event.getType() == "CONNECTTOSERVER") {
			connectToServer( event.pop(), event.pop(), event.pop() );
		} else if (event.getType() == "DISCONNECT") {
			disconnect();
		} else if (event.getType() == "I_START_TALKING") {
			m_sending=true;
			m_data.record_buffer.go=1;
			//while (Pa_IsStreamActive( m_stream )) { Pa_Sleep(10); }
			//printf("Recording ended!\n"); fflush(stdout);
			
			/*PaError err;
			while( ( err = Pa_IsStreamActive( m_stream ) ) == 1 )
			{
				Pa_Sleep(10);
				if (sizeof(SAMPLE)*NUM_CHANNELS*buffer->frameIndex+sizeof(SAMPLE)*NUM_CHANNELS*SAMPLE_RATE > buffer->size) {
					//We have to realloc, add size for 1 second
					buffer->maxFrameIndex+=SAMPLE_RATE;
					//buffer->maxFrameIndex=(buffer->maxFrameIndex > 44100*10?44100*10:buffer->maxFrameIndex);
					buffer->size=sizeof(SAMPLE)*NUM_CHANNELS*buffer->maxFrameIndex;
					printf("Realloc buffer, new size: %d bytes.\n",buffer->size);
					buffer->samples=(SAMPLE*)realloc(buffer->samples,buffer->size);
				}
			}*/
		} else if (event.getType() == "I_STOP_TALKING") {
			m_sending=false;
			paBuffer *buffer=&m_data.record_buffer;
			buffer->go=0;
			//Write to file
			/*printf("Writing recorded data to recording-client.raw (frameIndex: %d)\n",buffer->frameIndex); fflush(stdout);
			FILE *f;
			f=fopen("recording-client.raw","wb");
			fwrite(buffer->samples,1,sizeof(SAMPLE)*NUM_CHANNELS*buffer->frameIndex,f);
			fclose(f);*/
			//Reset buffer
			buffer->frameIndex=0;
			buffer->tx_pos=0;
		} else if (event.getType() == "SEND_TEXT") {
			std::string destination = event.pop_first();
			std::string msg = event.pop_first();

			//print_me("sending ("+msg+") to ("+destination+")");
			printf("(%d long)\n", msg.size());

			m_client_network.sendText(destination, msg);
		} else if (event.getType() == "BROADCAST_TEXT") {
			m_client_network.sendTextBroadcast(event.pop());
		} else {
			//print_me("NetworkManager: Got invalid event");
		}
		
	}
}

void NetworkManager::handleNetworkMessage(Message p_message)
{
	char *data = (char*)p_message.getData().getData();
	int length = p_message.getData().getLength();
	Data response;

	std::string data_str;
	int i;
	for(i = 0; i < length; i++)
	{
		data_str.push_back((char)data[i]);
	}

	if(p_message.getData().getType() == SERVER_LOGIN_OK)
	{
			//print_me("got \"SERVER_LOGIN_OK\"");
			m_connected_to = m_last_requested_server;
			m_events->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "CONNECTED", m_last_requested_server, m_last_requested_nick ) );
			channelListChanged();
			
	}
	else if(p_message.getData().getType() == SERVER_LOGIN_BAD)
	{
		printf("got \"SERVER_LOGIN_BAD\"\n");

		//check that the message is of proper length
		if(data_str.size() != 1)
		{
			//print_me("Message not of proper length.");
			return;
		}

		if(data_str == "1")
		{
			m_events->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "ERROR_CONNECTING", "bad nickname" ) );
		}
		else if(data_str == "2")
		{
			m_events->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "ERROR_CONNECTING", "ERR_IS_PASSWORD" ) );
		}
		else if(data_str == "3")
		{
			m_events->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "ERROR_CONNECTING", "Nickname busy" ) );
		}
	}
	else if(p_message.getData().getType() == SERVER_DISCONNECT)
	{
		//print_me("got \"SERVER_DISCONNECT\"");
		m_events->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "CONNECTION_LOST", m_connected_to ) );

		if(m_client_network.disconnect() != 0)
		{
			//print_me("Disconnection failed!");
		}
	}
	else if(p_message.getData().getType() == SERVER_AUDIO_DATA)
	{
		//print_me("got \"SERVER_AUDIO_DATA\"");
		
		//Add check here so we don't write out-of-bounds
		paBuffer *buffer=&m_data.play_buffers[0];
		memcpy(buffer->samples+buffer->tx_pos, data, length);
		//Write to file
		/*printf("Writing recorded data to recording-server.raw (length: %d)\n",length); fflush(stdout);
		FILE *f;
		f=fopen("recording-server.raw","ab");
		fwrite(data,1,length,f);
		fclose(f);*/
		//Write log
		/*f=fopen("recording-server.log","ab");
		fprintf(f,"play_buffer[0] tx_pos: %d frameIndex: %d maxFrameIndex: %d go: %d\n",buffer->tx_pos,buffer->frameIndex,buffer->maxFrameIndex,buffer->go);
		fclose(f);*/
		
		buffer->tx_pos+=length;
		buffer->maxFrameIndex=buffer->tx_pos/(sizeof(SAMPLE)*NUM_CHANNELS);
		if (!buffer->go && buffer->maxFrameIndex > SAMPLE_RATE/2) {
			buffer->go=1;
		}
	}
	else if(p_message.getData().getType() == SERVER_TEXT_DATA)
	{
		//print_me("got \"SERVER_TEXT_DATA\"");

		//check that the message is of proper length
		if(data_str.size() < MESSAGE_FILL)
		{
			//print_me("Message not of proper length.");
			return;
		}

		std::string sender = data_str.substr(0, MESSAGE_FILL);
		strip(sender);
		std::string message = data_str.substr(MESSAGE_FILL);

		m_events->pushEvent(UIEvent("TEXT_MESSAGE", sender, message));
	}
	else if(p_message.getData().getType() == SERVER_CHANNEL_CHANGE_RESPONSE)
	{
		//print_me("got \"SERVER_CHANNEL_CHANGE_RESPONSE\"");

		if(data_str.size() != 1)
		{
			//print_me("Message not of proper length");
		}

		if(data_str == "0")
		{
			//print_me("Channel change successful");
		}
		else if(data_str == "1")
		{
			log_this("Channel name not found");
		}
		else if(data_str == "2")
		{
			//print_me("<OPEN A DIALOG HERE NOTIFYING THE USER>");
			//print_me("bad password");
		}
		else if(data_str == "3")
		{
			log_this("Channel change failed. Reason unknown.");
		}
		else
		{
			log_this("Got an unknown message");
		}
	}
	else if(p_message.getData().getType() == SERVER_ADD_CLIENT)
	{
		//print_me("got \"SERVER_ADD_CLIENT\"");

		if(data_str.size() != MESSAGE_FILL*2)
		{
			//print_me("Message not of proper length");
			return;
		}

		std::string client_name = data_str.substr(0,MESSAGE_FILL);
		std::string channel_name = data_str.substr(MESSAGE_FILL,MESSAGE_FILL);
		strip(client_name);
		strip(channel_name);

		int returned = m_client_pool.addClient(client_name, channel_name, 0);
		if(returned == 0)
		{
			//print_me("Added ("+client_name+") to ("+channel_name+")");
			channelListChanged();
		}
		else if(returned == -1)
		{
			//print_me("Could not find channel ("+channel_name+")");
		}
		else if(returned == -2)
		{
			//print_me("Client with name ("+client_name+") already exists");
		}
		else if(returned == -3)
		{
			//print_me("Client with the same socket already exists");
		}
	}
	else if(p_message.getData().getType() == SERVER_MOVE_CLIENT)
	{
		//print_me("got \"SERVER_MOVE_CLIENT\"");

		//check that the message is of proper length
		if(data_str.size() != MESSAGE_FILL*2)
		{
			//print_me("Message not of proper length.");
			return;
		}

		std::string client_name = data_str.substr(0, MESSAGE_FILL);
		std::string channel_name = data_str.substr(MESSAGE_FILL, MESSAGE_FILL);
		strip(client_name);
		strip(channel_name);

		//print_me("should move \""+client_name+"\" to channel \""+channel_name+"\"");
		channelListChanged();
	}
	else if(p_message.getData().getType() == SERVER_REMOVE_CLIENT)
	{
		//print_me("got \"SERVER_REMOVE_CLIENT\"");

		//check that the message is of proper length
		if(data_str.size() != MESSAGE_FILL)
		{
			//print_me("Message not of proper length.");
			return;
		}

		std::string client_name = data_str.substr(0, MESSAGE_FILL);
		strip(client_name);

		m_client_pool.removeClient(client_name);
		//print_me("removed \""+client_name+"\"");
		channelListChanged();
	}
	else if(p_message.getData().getType() == SERVER_ADD_CHANNEL)
	{
		//print_me("got \"SERVER_ADD_CHANNEL\"");

		//check that the message is of proper length
		if(data_str.size() != MESSAGE_FILL)
		{
			//print_me("Message not of proper length.");
			return;
		}

		std::string channel_name = data_str.substr(0, MESSAGE_FILL);
		strip(channel_name);
		if(!m_client_pool.addChannel(channel_name, ""))
		{
			//print_me("Could not add channel ("+channel_name+")");
		}
		else
		{
			//print_me("Added channel ("+channel_name+")");
		}
		channelListChanged();
	}
	else if(p_message.getData().getType() == SERVER_REMOVE_CHANNEL)
	{
		//print_me("got \"SERVER_REMOVE_CHANNEL\"");

		//check that the message is of proper length
		if(data_str.size() != MESSAGE_FILL)
		{
			//print_me("Message not of proper length.");
			return;
		}

		std::string channel_name = data_str.substr(0, MESSAGE_FILL);
		strip(channel_name);
		if(!m_client_pool.removeChannel(channel_name))
		{
			//print_me("Could not remove channel ("+channel_name+")");
		}
		else
		{
			//print_me("Removed channel ("+channel_name+")");
		}
		channelListChanged();
	}
	else if(p_message.getData().getType() == SERVER_NOTIFY
			|| p_message.getData().getType() == SERVER_ERROR_NOTIFY)
	{
		//print_me("got \"SERVER_NOTIFY\" or \"SERVER_ERROR_NOTIFY\"");

		//check that the message is of proper length
		if(data_str.size() < MESSAGE_FILL)
		{
			//print_me("Message not of proper length.");
			return;
		}

		std::string title = data_str.substr(0, MESSAGE_FILL);
		strip(title);
		std::string message = data_str.substr(MESSAGE_FILL);

		if(p_message.getData().getType() == SERVER_NOTIFY)
		{
			m_events->pushEvent(UIEvent("NOTIFICATION", title, message));
		}
		else
		{
			m_events->pushEvent(UIEvent("ERROR_MESSAGE", title, message));
		}
	}
}

void NetworkManager::channelListChanged()
{
	//print_me("NetworkManager: Giving new channel list to UI");

	std::vector<std::string> new_list;
	std::vector<std::string> channels = m_client_pool.getChannelNames();
	for (int i=0;i<channels.size();i++)
	{
		new_list.push_back(channels.at(i));

		std::vector<std::string> members;
		//print_me("channel: ("+channels.at(i)+")");
		if(!m_client_pool.getChannelClientNames(channels.at(i), members))
		{
			log_this("could not get channel's client names");
		}
		for(int j=0;j<members.size();j++)
		{
			//print_me("member: ("+members.at(j)+")");
			new_list.push_back(members.at(j));
		}
	
		new_list.push_back("--END--");
	}	

	m_events->pushEvent(UIEvent("NEW_CHANNEL_LIST", new_list));
}

void NetworkManager::joinChannel(std::string p_channel_name)
{
	//print_me("NetworkManager::joinChannel() trying to join "+p_channel_name);

	m_client_network.changeChannels(p_channel_name, "");		//FIXME: this is hack
}

void NetworkManager::connectToServer(std::string p_address, std::string p_username, std::string p_password)
{
	m_client_pool.clear();

	m_last_requested_nick = p_username;
	m_last_requested_server = p_address;

	std::string parsed_ip;
	unsigned int parsed_port;	
	int pos = p_address.find_last_of(':');	
	if (pos == -1)
	{
		parsed_ip = p_address;
		parsed_port = STANDARD_PORT; // twug standard port
	}
	else
	{		
		parsed_ip = p_address.substr(0,pos);
		parsed_port = atoi( p_address.substr(pos+1).c_str());
	}

	std::cout << "NetworkManager::connectToServer() connecting to " << parsed_ip << ":" << parsed_port << " as " << p_username << " with password " << p_password << std::endl;

	m_events->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "CONNECTING", m_last_requested_server, m_last_requested_nick ) );

	int returned = m_client_network.connect(parsed_ip, parsed_port);
	m_socket = m_client_network.getSocket(); // socket file descriptor 

	if (returned == 0) {
		m_events->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "LOGGING_IN", m_last_requested_server, m_last_requested_nick ) );
		//print_me("Sending login request");
		m_client_network.loginRequest(m_last_requested_nick, p_password);
		//print_me("Sent login request");
	} else {
		m_events->pushEvent(UIEvent("NEW_CONNECTION_STATUS", "ERROR_CONNECTING", m_last_requested_server));
	}
}


void NetworkManager::disconnect()
{
	m_client_network.logout();

	if(m_client_network.disconnect() != 0)
	{
		//print_me("Disconnection failed!");
	}
	//print_me("Disconnected");

	m_events->pushEvent(UIEvent("NEW_CONNECTION_STATUS", "DISCONNECTED"));
}

int NetworkManager::audioCallback(const void *inputBuffer,
	void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	unsigned int i=0;
	
	//Got anything to play?
	paBuffer *buffer = &static_cast<NetworkManager*>(userData)->m_data.play_buffers[0];
	SAMPLE *wptr = (SAMPLE*)outputBuffer;
	if (buffer->go)
	{
		SAMPLE *rptr = &buffer->samples[buffer->frameIndex * NUM_CHANNELS];
		unsigned int framesLeft = buffer->maxFrameIndex-buffer->frameIndex;
		unsigned int framesToPlay = (framesLeft < framesPerBuffer?framesLeft:framesPerBuffer);
		//We got some audio to play
		for(; i < framesToPlay; i++ )
		{
			*wptr++ = *rptr++;  // left
			if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  // right
		}
		buffer->frameIndex += framesToPlay;
	}
	//Fill the rest of the buffer with silence
	for(; i<framesPerBuffer; i++ )
	{
		*wptr++ = SAMPLE_SILENCE;  // left
		if( NUM_CHANNELS == 2 ) *wptr++ = SAMPLE_SILENCE;  // right
	}
	//Reset buffer if we've reached the end
	if (buffer->go && buffer->frameIndex == buffer->maxFrameIndex) {
		buffer->go=0;
		buffer->frameIndex=0;
		buffer->maxFrameIndex=0;
		buffer->tx_pos=0;
	}
	
	//Record anything?
	buffer = &static_cast<NetworkManager*>(userData)->m_data.record_buffer;
	if (buffer->go && inputBuffer != NULL)
	{
		const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
		SAMPLE *wptr = &buffer->samples[buffer->frameIndex * NUM_CHANNELS];
		unsigned int framesLeft = buffer->maxFrameIndex-buffer->frameIndex;
		unsigned int framesToRecord = (framesLeft < framesPerBuffer?framesLeft:framesPerBuffer);
		//We got some audio to record
		for(i=0; i < framesToRecord; i++ )
		{
			*wptr++ = *rptr++;  // left
			if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  // right
		}
		buffer->frameIndex += framesToRecord;
	}
	
	return paContinue;
}

