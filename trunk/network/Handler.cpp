#include "Handler.h"


Handler::Handler(
	void (*p_cb_got_text_message)(std::string,std::string),
	void (*p_cb1)(std::string),
	void (*p_cb_connected_to_server)(std::string, std::string),
	void (*p_cb_connection_lost)(std::string),
	void (*p_cb_channel_list_changed)()
	)
{
	m_cb_got_text_message		=	p_cb_got_text_message;
	m_cb1 = p_cb1;
	m_cb_connected_to_server	=	p_cb_connected_to_server;
	m_cb_connection_lost		=	p_cb_connection_lost;
	m_cb_channel_list_changed	=	p_cb_channel_list_changed;

/*
	m_client_pool.addClient("Basse", "__lobby__", 0);
	m_client_pool.addClient("Loladin", "__lobby__", 0);

	m_client_pool.addClient("Recover", "channel 1", 0);
	m_client_pool.addClient("Toaefge", "channel 1", 0);
*/

	std::cout << "Handler: constructed" << std::endl;
}

Handler::~Handler()
{
}

std::vector<std::string> Handler::getChannelMembers(std::string p_channel_name)
{
	std::vector<std::string> client_names;
	m_client_pool.getChannelClientNames(p_channel_name, &client_names);
	return client_names;
}

std::vector<std::string> Handler::getChannels()
{
	return m_client_pool.getChannelNames();
}

void Handler::joinChannel(std::string p_channel_name)
{
	printf("Handler::joinChannel() trying to join %s\n", p_channel_name.c_str());

	m_client_network.changeChannels(p_channel_name);

	m_mychannel = channel_name;
	m_cb_channel_list_changed(); // channel list changed (callback)
}

void Handler::connectToServer(std::string p_address, unsigned int p_port, std::string p_username, std::string p_password)
{
	printf("Handler::connectToServer() connecting to %s:%d as %s with password %s\n", p_address.c_str(), p_port, p_username.c_str(), p_password.c_str());

	p_client_network.connect(p_address, p_port);
	p_client_network.loginRequest(p_username, p_password);

	m_mynick = p_username;

	m_cb_connected_to_server(p_address, p_username);		//add p_port to this maybe?
}


void Handler::disconnect()
{
	m_client_network.logout();
	m_client_network.disconnect();

	std::cout << "Handler: disconnected\n";
}


void Handler::iStartTalking()
{
	std::cout << "Handler: Push-to-talk now active\n";
}

void Handler::iStopTalking()
{
	std::cout << "Handler: Push-to-talk now inactive\n";
}

void Handler::sendText(std::string p_to_username, std::string p_message)
{
	printf("Handler::sendText() sending \"%s\" to \"%s\"\n", p_to_username.c_str(), p_message.c_str());

	m_client_network.sendText(p_to_username, p_message);
}

int Handler::getSocket()
{
	return 	m_client_network.getSocket();
}

void Handler::update()
{
	Message incomming_message;
	while(n.processNetworking())
	{
//		printf("processed network\n");
		while(n.getMessage(incomming_message))	//means we have incomming data in incomming_message
		{
//			printf("got message\n");
			handleMessage(incomming_message);
		}
	}
}

void Handler::handleMessage(Message p_message)
{
	char *data = (char*)p_message.getData().getData();
	int length = p_message.getData().getLength();
	std::string data_str = data;
	Data response;

	if(p_message.getData().getType() == SERVER_LOGIN_OK)
	{
			printf("got \"SERVER_LOGIN_OK\"\n");
	}
	else if(p_message.getData().getType() == SERVER_LOGIN_BAD)
	{
		int why = *((int*)data);

		printf("got \"SERVER_LOGIN_BAD\"\n");
		if(why == 1)
			printf("bad username\n");
		else if(why == 2)
			printf("bad password\n");
		else if(why == 3)
			printf("there is already a user with that name\n");
	}
	else if(p_message.getData().getType() == SERVER_DISCONNECT)
	{
		printf("got \"SERVER_DISCONNECT\"\n");

		int why = *((int*)data);
		if(why & 1)
		{
			printf("I SAY GOT HERE!\n");
		}
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

		m_cb_got_text_message(sender, message);
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
	}
	else if(p_message.getData().getType() == SERVER_MOVE_CLIENT)
	{
		printf("got \"SERVER_MOVE_CLIENT\"\n");

		std::string client_name = data_str.substr(0, 20);
		std::string channel_name = data_str.substr(20);
		strip(client_name);
		strip(channel_name);

		printf("should move \"%s\" to channel \"%s\"\n", client_name.c_str(), channel_name.c_str());
	}
	else if(p_message.getData().getType() == SERVER_REMOVE_CLIENT)
	{
		printf("got \"SERVER_REMOVE_CLIENT\"\n");

		std::string client_name = data_str.substr(0, 20);
		strip(client_name);

		m_client_pool.removeClient(client_name);
		printf("removed \"%s\"\n", client_name.c_str());
	}
}

