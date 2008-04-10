#include "NetworkManager.h"

void NetworkManager::processNetworkEvents()
{
	//got_here();
	Message incoming_message;
	while(m_client_network.processNetworking())
	{
		got_here();
		while(m_client_network.getMessage(incoming_message))
		{
			got_here();
			handleNetworkMessage(incoming_message);
		}
	}
}
void NetworkManager::processUIEvents()
{
	std::cout << "process events to_network" << std::endl;

	bool kaka=true;
	while(kaka)
	{
		UIEvent event = m_events->to_network->popEvent();

		if (event.getType() == "EMPTY") {
			std::cout << "Got empty event\n";
			kaka=false;
		} else if (event.getType() == "JOINCHANNEL") {
			std::cout << "Joining channel\n";
				joinChannel(event.pop());
		} else if (event.getType() == "HACK") {
			std::cout << "Hack\n";
			m_events->to_ui->pushEvent(UIEvent ("EMPTY"));
		} else if (event.getType() == "CONNECTTOSERVER") {
				got_here();
				connectToServer( event.pop(), event.pop(), event.pop() );
		} else if (event.getType() == "DISCONNECT") {
				got_here();
				disconnect();
		} else if (event.getType() == "I_START_TALKING") {
				got_here();
				m_talkbutton=true;
		} else if (event.getType() == "I_STOP_TALKING") {
				got_here();
				m_talkbutton=false;
		} else if (event.getType() == "SEND_TEXT") {
				std::string destination = event.pop_first();
				std::string msg = event.pop_first();
	
				printf( "NetworkManager::sendText() sending \"%s\" to \"%s\"\n",
					 destination.c_str(), msg.c_str());

				m_client_network.sendText(destination, msg);
		} else {
				got_here();
				std::cout << "NetworkManager: Got invalid event" << std::endl;
		}
		
	}
}

void NetworkManager::handleNetworkMessage(Message p_message)
{
	char *data = (char*)p_message.getData().getData();
	int length = p_message.getData().getLength();
	std::string data_str = data;
	Data response;

	if(p_message.getData().getType() == SERVER_LOGIN_OK)
	{
			printf("got \"SERVER_LOGIN_OK\"\n");

			m_events->to_ui->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "CONNECTED", m_lastrequestedserver, m_lastrequestednick ) );
			channelListChanged();
			
	}
	else if(p_message.getData().getType() == SERVER_LOGIN_BAD)
	{
		int why = *((int*)data);

		printf("got \"SERVER_LOGIN_BAD\"\n");
		if(why == 1)
		{
			printf("bad username\n");
			m_events->to_ui->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "ERROR_CONNECTING", "bad nickname" ) );
		} else if(why == 2) {
			printf("bad password\n");
			m_events->to_ui->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "ERROR_CONNECTING", "ERR_IS_PASSWORD" ) );
		} else if(why == 3) {
			printf("there is already a user with that name\n");
			m_events->to_ui->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "ERROR_CONNECTING", "Nickname busy" ) );
		}
	}
	else if(p_message.getData().getType() == SERVER_DISCONNECT)
	{
		printf("got \"SERVER_DISCONNECT\"\n");
		m_events->to_ui->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "CONNECTION_LOST", m_connectedTo ) );
		m_connectedandorloggedin=0;
	}
	else if(p_message.getData().getType() == SERVER_AUDIO_DATA)
	{
		printf("got \"SERVER_AUDIO_DATA\"\n");
	}
	else if(p_message.getData().getType() == SERVER_TEXT_DATA)
	{
		printf("got \"SERVER_TEXT_DATA\"\n");
		std::string str = data;
		std::string sender = str.substr(0, 20);
		strip(sender);
		std::string message = str.substr(20);

		m_events->to_ui->pushEvent( UIEvent ("TEXT_MESSAGE", sender, message) );

		printf("\"%s\": \"%s\"\n", sender.c_str(), message.c_str());
	}
	else if(p_message.getData().getType() == SERVER_CHANNEL_CHANGE_RESPONSE)
	{
		printf("got \"SERVER_CHANNEL_CHANGE_RESPONSE\"\n");

		int response = *((int*)data);

		printf("got %d in response\n", response);
	}
	else if(p_message.getData().getType() == SERVER_ADD_CLIENT)
	{
		printf("got \"SERVER_ADD_CLIENT\"\n");
		std::string client_name = data_str.substr(0, 20);
		std::string channel_name = data_str.substr(20);
		strip(client_name);
		strip(channel_name);

		m_client_pool.addClient(client_name, channel_name, 0);
		printf("added \"%s\" to \"%s\"\n", client_name.c_str(), channel_name.c_str());
		channelListChanged();
	}
	else if(p_message.getData().getType() == SERVER_MOVE_CLIENT)
	{
		printf("got \"SERVER_MOVE_CLIENT\"\n");

		std::string client_name = data_str.substr(0, 20);
		std::string channel_name = data_str.substr(20);
		strip(client_name);
		strip(channel_name);

		printf("should move \"%s\" to channel \"%s\"\n", client_name.c_str(), channel_name.c_str());
		channelListChanged();
	}
	else if(p_message.getData().getType() == SERVER_REMOVE_CLIENT)
	{
		printf("got \"SERVER_REMOVE_CLIENT\"\n");

		std::string client_name = data_str.substr(0, 20);
		strip(client_name);

		m_client_pool.removeClient(client_name);
		printf("removed \"%s\"\n", client_name.c_str());
		channelListChanged();
	}
}

void NetworkManager::channelListChanged()
{
	std::cout << "NetworkManager: Giving new channel list to UI\n";	

	std::vector<std::string> new_list;
	std::vector<std::string> channels = m_client_pool.getChannelNames();
	for (int i=0;i<channels.size();i++)
	{
		new_list.push_back( channels.at(i) );

		std::vector<std::string> members;
		m_client_pool.getChannelClientNames(channels.at(i), &members);
		for(int j=0;j<members.size();j++)
		{
			new_list.push_back( members.at(j) );
		}
	
		new_list.push_back( "--END--" );
	}	

	m_events->to_ui->pushEvent( UIEvent ("NEW_CHANNEL_LIST", new_list) );
}

void NetworkManager::joinChannel(std::string p_channel_name)
{
	printf("NetworkManager::joinChannel() trying to join %s\n", p_channel_name.c_str());

	m_client_network.changeChannels(p_channel_name, "");		//FIXME: this is hack
}

void NetworkManager::connectToServer(std::string p_address, std::string p_username, std::string p_password)
{
	std::string parsed_ip;
	unsigned int parsed_port;	
	int pos = p_address.find_last_of(':');	
	if (pos == -1)
	{
		parsed_ip = p_address;
		parsed_port = 6789; // twug standard port
	}
	else
	{		
		parsed_ip = p_address.substr(0,pos);
		parsed_port = atoi( p_address.substr(pos+1).c_str());
	}

	printf("NetworkManager::connectToServer() connecting to %s:%d as %s with password %s\n",parsed_ip.c_str(), parsed_port, p_username.c_str(), p_password.c_str());
	fflush(stdout);

	m_lastrequestednick = p_username;
	m_lastrequestedserver = p_address;

	m_events->to_ui->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "CONNECTING", m_lastrequestedserver, m_lastrequestednick ) );

	int returned = m_client_network.connect(parsed_ip, parsed_port);
	printf("returned: \"%d\"\n", returned);


	m_client_network.loginRequest(m_lastrequestednick, p_password);
	m_connectedandorloggedin=1;
	got_here();
}


void NetworkManager::disconnect()
{
	m_client_network.logout();
	m_connectedandorloggedin=0;
	m_client_network.disconnect();


	std::cout << "NetworkManager::disconnect() disconnected\n";

	m_events->to_ui->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "DISCONNECTED" ) );
}

NetworkManager::NetworkManager(UIEvents *p_events)
{
	m_events = p_events;
	m_connectedandorloggedin = 0;

	m_talkbutton = 0;

	m_socket = m_client_network.getSocket();
	
	while(true) // thread main loop
	{
		m_readfd = open(m_events->to_network->getFilePath().c_str(), O_RDONLY);

		if(m_connectedandorloggedin==0) // if we arent connected only select on readfd
		{
			struct timeval tv;

			fd_set read;
			FD_ZERO(&read);
			FD_SET(m_readfd, &read);

			tv.tv_sec = 5;
			tv.tv_usec = 0;
			int select_returned = select(m_readfd+1, &read, NULL, NULL, &tv);
			if(select_returned == -1)
			{
				//report_error(strerror(errno));
			}

			if(FD_ISSET(m_readfd, &read))
			{
				char buf[100];
				::read(m_readfd, buf, 100);
				std::cout << "woke up from UI event (only watching those)" << std::endl;
				processUIEvents();
			}	
		} else { // else select on both fds
			struct timeval tv;

			fd_set read;
			FD_ZERO(&read);
			FD_SET(m_readfd, &read);
			FD_SET(m_socket, &read);

			int bigone;
			if (m_socket>m_readfd)
				bigone=m_socket+1;
			else
				bigone=m_readfd+1; 

			tv.tv_sec = 5;
			tv.tv_usec = 0;
			int select_returned = select(bigone, &read, NULL, NULL, &tv);
			if(select_returned == -1)
			{
				//report_error(strerror(errno));
			}

			if(FD_ISSET(m_socket, &read))
			{
				std::cout << "woke up from network event" << std::endl;
				processNetworkEvents();
			
			}

		 	if(FD_ISSET(m_readfd, &read))
			{
				std::cout << "woke up from UI event" << std::endl;
				char buf[100];
				::read(m_readfd, buf, 100);
				processUIEvents();
			}
		}
		::close(m_readfd);
	}
}
