#include "NetworkManager.h"

NetworkManager::NetworkManager(UIEventQueue* p_to_ui, UIEventQueue* p_to_network, DataKeeper* p_data)
{
	m_to_network = p_to_network;
	m_data = p_data;

	m_readfd = open(p_to_network->getFilePath().c_str(), O_RDONLY); //opens to_network for reading

	m_events = new UIEventsNetwork(p_to_ui); // opens to_ui for writing

	m_talk_button = false; // means we will not send audio data

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
		print_me("start of network thread loop");

		bool new_connection_status = m_client_network.getConnectionStatus();

		if (last_connected_status && !new_connection_status) //means if we where connected last time and not this time
		{
			m_events->to_ui->pushEvent(UIEvent("NEW_CONNECTION_STATUS", "CONNECTION_LOST", m_connected_to));
		}

		struct timeval tv;
		fd_set read;
		FD_ZERO(&read);

		FD_SET(m_readfd, &read);

		if(!new_connection_status) // if we arent connected only select on readfd
		{
			tv.tv_sec = 5;
			tv.tv_usec = 0;

			print_me("selecting on readfd");
			int select_returned = select(m_readfd+1, &read, NULL, NULL, &tv);
			print_me("selected on readfd");

			if(select_returned == -1)
			{
				report_error(strerror(errno));
			}

			if(FD_ISSET(m_readfd, &read))
			{
				print_me("woke up from UI event (only watching those)");
				char buf[100];
				::read(m_readfd, buf, 100);
				processUIEvents();
			}	
		} else { //selecting on both fds

			FD_SET(m_socket, &read);

			if (m_talk_button) // means we need to wake up more often in order to send audio data
			{
				tv.tv_sec = 5;
				tv.tv_usec = 0;

				// need to send audio data here, too
			} else {
				tv.tv_sec = 0;
				tv.tv_usec = 10000;
			}		

			int bigone;
			if (m_socket>m_readfd) {
				bigone = m_socket+1;
			} else {
				bigone = m_readfd+1;
			}
			
			print_me("selecting on both fds");
			int select_returned = select(bigone, &read, NULL, NULL, &tv);
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

		 	if(FD_ISSET(m_readfd, &read))
			{
				print_me("got fifo data");
				char buf[100];
				::read(m_readfd, buf, 100);
				processUIEvents();
				print_me("processed UI events");
			}
		}
		last_connected_status = new_connection_status;	
	}
}

void NetworkManager::processNetworkEvents()
{
	print_me("start of processNetworkEvents()");
	Message incoming_message;
	while(m_client_network.processNetworking())
	{
		print_me("start of processNetworking() loop");
		while(m_client_network.getMessage(incoming_message))
		{
			print_me("start of getMessage() loop");
			handleNetworkMessage(incoming_message);
			print_me("end of getMessage() loop");
		}
		print_me("end of processNetworking() loop");
	}
	print_me("leaving processNetworkEvents()");
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
				m_talk_button = true;
		} else if (event.getType() == "I_STOP_TALKING") {
				m_talk_button = false;
		} else if (event.getType() == "SEND_TEXT") {
				std::string destination = event.pop_first();
				std::string msg = event.pop_first();
	
				print_me("NetworkManager::sendText() sending \""+msg+"\" to \""+destination+"\"");

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
	std::string data_str = data;
	Data response;

	if(p_message.getData().getType() == SERVER_LOGIN_OK)
	{
			print_me("got \"SERVER_LOGIN_OK\"");
			m_connected_to = m_last_requested_server;
			m_events->to_ui->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "CONNECTED", m_last_requested_server, m_last_requested_nick ) );
			channelListChanged();
			
	}
	else if(p_message.getData().getType() == SERVER_LOGIN_BAD)
	{
		printf("got \"SERVER_LOGIN_BAD\"\n");

		int why = *((int*)data);
		if(why == 1)
		{
			print_me("bad username");
			m_events->to_ui->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "ERROR_CONNECTING", "bad nickname" ) );
		} else if(why == 2) {
			print_me("bad password");
			m_events->to_ui->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "ERROR_CONNECTING", "ERR_IS_PASSWORD" ) );
		} else if(why == 3) {
			print_me("there is already a user with that name");
			m_events->to_ui->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "ERROR_CONNECTING", "Nickname busy" ) );
		}
	}
	else if(p_message.getData().getType() == SERVER_DISCONNECT)
	{
		print_me("got \"SERVER_DISCONNECT\"");
		m_events->to_ui->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "CONNECTION_LOST", m_connected_to ) );

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
		std::string str = data;
		print_me("got message: ("+str+")");
		std::string sender = str.substr(0, 20);
		strip(sender);
		std::string message = str.substr(20);

		//remove this later, this is only for the dirty hack "pumping"
		if (sender != "server") {
			m_events->to_ui->pushEvent(UIEvent("TEXT_MESSAGE", sender, message));
			print_me(sender+": "+message);
		} else {
			print_me("server pump");
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
		std::string client_name = data_str.substr(0, 20);
		std::string channel_name = data_str.substr(20);
		strip(client_name);
		strip(channel_name);

		m_client_pool.addClient(client_name, channel_name, 0);
		print_me("added \""+client_name+"\" to \""+channel_name+"\"");
		channelListChanged();
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
	print_me("NetworkManager::joinChannel() trying to join "+p_channel_name);

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

	print_me("NetworkManager::connectToServer() connecting to "+parsed_ip+":"+p_address.substr(pos+1).c_str()+" as "+p_username+" with password "+p_password);

	m_last_requested_nick = p_username;
	m_last_requested_server = p_address;

	m_events->to_ui->pushEvent( UIEvent ("NEW_CONNECTION_STATUS", "CONNECTING", m_last_requested_server, m_last_requested_nick ) );

	int returned = m_client_network.connect(parsed_ip, parsed_port);
	print_me("returned:");
	printf("%d\n", returned);
	if (returned == 0) {
		print_me("Sending login request");
		m_client_network.loginRequest(m_last_requested_nick, p_password);
		print_me("Sent login request");
	} else {
		m_events->to_ui->pushEvent(UIEvent("NEW_CONNECTION_STATUS", "ERROR_CONNECTING", m_last_requested_server));
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

	m_events->to_ui->pushEvent(UIEvent("NEW_CONNECTION_STATUS", "DISCONNECTED"));
}

