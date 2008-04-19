#include "NetworkManager.h"

NetworkManager::NetworkManager(UIEventQueue* p_to_ui, UIEventQueue* p_to_network, DataKeeper* p_data)
{
	m_to_network = p_to_network;
	m_data = p_data;

	m_readfd = p_to_network->getReadFd();

	m_events = new EventsToUI(p_to_ui);

	m_socket = m_client_network.getSocket(); // socket file descriptor 
}

NetworkManager::~NetworkManager ()
{
	::close(m_readfd);
}


void NetworkManager::run()
{
	bool last_connected_status = false;
	while(true) // thread main loop
	{
//		print_me("start of network thread loop");

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
			tv.tv_sec = 60;
			tv.tv_usec = 0;

			print_me("selecting on readfd");
			int select_returned = select(m_readfd+1, &read, NULL, NULL, &tv);
			print_me("selected on readfd");
			if(select_returned == -1)
			{
				report_error(strerror(errno));
			}

		} else { //selecting on both fds

			FD_SET(m_socket, &read);

			if(m_data->getSending()) // means we need to wake up more often and send audio data
			{
				tv.tv_sec = 0;
				tv.tv_usec = 10000;

				// need to send audio data here, too
			} else {
				tv.tv_sec = 1;
				tv.tv_usec = 0;
			}		

			int highest;
			if (m_socket > m_readfd) {
				highest = m_socket+1;
			} else {
				highest = m_readfd+1;
			}

			print_me("selecting on both fds");
			int select_returned = select(highest, &read, NULL, NULL, &tv);
			if(select_returned == -1)
			{
				report_error(strerror(errno));
			}
			print_me("selected on both fds");

			if(FD_ISSET(m_socket, &read))
			{
				print_me("got network data");
				processNetworkEvents();
				print_me("processed network data");
			}
			else
			{
				print_me("not set, reading anyway");
				processNetworkEvents();
			}
		}

		if(FD_ISSET(m_readfd, &read))
		{
			print_me("got pipe data");
			char buf[100];
			::read(m_readfd, buf, 100);
			processUIEvents();
			print_me("processed UI events");
		}

		last_connected_status = new_connection_status;	
	}
}

void NetworkManager::processNetworkEvents()
{
	if(!m_client_network.processNetworking())
	{
		print_me("Got disconnected");
	}
	else
	{
		print_me("Processed networking");
	}

	Message incoming_message;
	while(m_client_network.getMessage(incoming_message))
	{
		handleNetworkMessage(incoming_message);
		print_me("Got Message");
	}
	print_me("Got all available messages");
}
void NetworkManager::processUIEvents()
{
	print_me("NetworkManager: Processing events from UI");

	bool process = true;
	while(process)
	{
		UIEvent event = m_to_network->popEvent(); // pop an event

		if (event.getType() == "EMPTY") {
			process = false;
		} else if (event.getType() == "JOINCHANNEL") {
				joinChannel(event.pop());
		} else if (event.getType() == "NEWCHANNEL") {
				std::cout << "DEBUG> CREATE NEW CHANNEL " << event.pop() << std::endl;
		} else if (event.getType() == "REMOVECHANNEL") {
				std::cout << "DEBUG> REMOVE CHANNEL " << event.pop() << std::endl;
		} else if (event.getType() == "CONNECTTOSERVER") {
				connectToServer( event.pop(), event.pop(), event.pop() );
		} else if (event.getType() == "DISCONNECT") {
				disconnect();
		} else if (event.getType() == "I_START_TALKING") {
				m_data->setSending(true);
		} else if (event.getType() == "I_STOP_TALKING") {
				m_data->setSending(false);
		} else if (event.getType() == "SEND_TEXT") {
				std::string destination = event.pop_first();
				std::string msg = event.pop_first();
	
				print_me("sending ("+msg+") to ("+destination+")");
				printf("(%d long)\n", msg.size());

				m_client_network.sendText(destination, msg);
		} else {
				print_me("NetworkManager: Got invalid event");
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
			print_me("got \"SERVER_LOGIN_OK\"");
			m_connected_to = m_last_requested_server;
			m_events->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "CONNECTED", m_last_requested_server, m_last_requested_nick ) );
			channelListChanged();
			
	}
	else if(p_message.getData().getType() == SERVER_LOGIN_BAD)
	{
		printf("got \"SERVER_LOGIN_BAD\"\n");

		int why = *((int*)data);
		if(why == 1)
		{
			print_me("bad username");
			m_events->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "ERROR_CONNECTING", "bad nickname" ) );
		} else if(why == 2) {
			print_me("bad password");
			m_events->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "ERROR_CONNECTING", "ERR_IS_PASSWORD" ) );
		} else if(why == 3) {
			print_me("there is already a user with that name");
			m_events->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "ERROR_CONNECTING", "Nickname busy" ) );
		}
	}
	else if(p_message.getData().getType() == SERVER_DISCONNECT)
	{
		print_me("got \"SERVER_DISCONNECT\"");
		m_events->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "CONNECTION_LOST", m_connected_to ) );

		if(m_client_network.disconnect() != 0)
		{
			print_me("Disconnection failed!");
		}
	}
	else if(p_message.getData().getType() == SERVER_AUDIO_DATA)
	{
		print_me("got \"SERVER_AUDIO_DATA\"");
	}
	else if(p_message.getData().getType() == SERVER_TEXT_DATA)
	{
		print_me("got \"SERVER_TEXT_DATA\"");
		print_me("got message: ("+data_str+")");
		printf("(%d long)\n", data_str.size());
		std::string sender = data_str.substr(0, 20);
		strip(sender);
		std::string message = data_str.substr(20);

		//remove this later, this is only for the dirty hack "pumping"
		if (sender != "server") {
			m_events->pushEvent(UIEvent("TEXT_MESSAGE", sender, message));
			print_me(sender+": "+message);
		} else {
//			print_me("server pump");
		}
	}
	else if(p_message.getData().getType() == SERVER_CHANNEL_CHANGE_RESPONSE)
	{
		print_me("got \"SERVER_CHANNEL_CHANGE_RESPONSE\"");

		int response = *((int*)data);

		printf("got %d in response", response);
	}
	else if(p_message.getData().getType() == SERVER_ADD_CLIENT)
	{
		print_me("got \"SERVER_ADD_CLIENT\"");

		if(data_str.size() != 40)
		{
			print_me("Message not of proper length");
			printf("data_str.size(): (%d)\n", data_str.size());
			std::cout << "data_str: (" << data_str << ")" << std::endl;

			printf("lengt: (%d)\n", length);
			printf("printf: data: (%s)\n", data);
			return;
		}

		std::string client_name = data_str.substr(0,20);
		std::string channel_name = data_str.substr(20,20);
		strip(client_name);
		strip(channel_name);

		if(!m_client_pool.addClient(client_name, channel_name, 0))
		{
			print_me("Could not add client ("+client_name+") to channel ("+channel_name+")");
		}
		else
		{
			print_me("Added ("+client_name+") to ("+channel_name+")");
			channelListChanged();
		}
	}
	else if(p_message.getData().getType() == SERVER_MOVE_CLIENT)
	{
		print_me("got \"SERVER_MOVE_CLIENT\"");

		std::string client_name = data_str.substr(0, 20);
		std::string channel_name = data_str.substr(20);
		strip(client_name);
		strip(channel_name);

		print_me("should move \""+client_name+"\" to channel \""+channel_name+"\"");
		channelListChanged();
	}
	else if(p_message.getData().getType() == SERVER_REMOVE_CLIENT)
	{
		print_me("got \"SERVER_REMOVE_CLIENT\"");

		std::string client_name = data_str.substr(0, 20);
		strip(client_name);

		m_client_pool.removeClient(client_name);
		print_me("removed \""+client_name+"\"");
		channelListChanged();
	}
}

void NetworkManager::channelListChanged()
{
	print_me("NetworkManager: Giving new channel list to UI");

	std::vector<std::string> new_list;
	std::vector<std::string> channels = m_client_pool.getChannelNames();
	for (int i=0;i<channels.size();i++)
	{
		new_list.push_back(channels.at(i));

		std::vector<std::string> members;
		print_me("channel: ("+channels.at(i)+")");
		if(!m_client_pool.getChannelClientNames(channels.at(i), &members))
		{
			log_this("could not get channel's client names");
		}
		for(int j=0;j<members.size();j++)
		{
			print_me("member: ("+members.at(j)+")");
			new_list.push_back(members.at(j));
		}
	
		new_list.push_back("--END--");
	}	

	m_events->pushEvent(UIEvent("NEW_CHANNEL_LIST", new_list));
}

void NetworkManager::joinChannel(std::string p_channel_name)
{
	print_me("NetworkManager::joinChannel() trying to join "+p_channel_name);

	m_client_network.changeChannels(p_channel_name, "");		//FIXME: this is hack
}

void NetworkManager::connectToServer(std::string p_address, std::string p_username, std::string p_password)
{
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

	if (returned == 0) {
		m_events->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "LOGGING_IN", m_last_requested_server, m_last_requested_nick ) );
		print_me("Sending login request");
		m_client_network.loginRequest(m_last_requested_nick, p_password);
		print_me("Sent login request");
	} else {
		m_events->pushEvent(UIEvent("NEW_CONNECTION_STATUS", "ERROR_CONNECTING", m_last_requested_server));
	}
}


void NetworkManager::disconnect()
{
	m_client_network.logout();

	if(m_client_network.disconnect() != 0)
	{
		print_me("Disconnection failed!");
	}
	print_me("Disconnected");

	m_events->pushEvent(UIEvent("NEW_CONNECTION_STATUS", "DISCONNECTED"));
}

