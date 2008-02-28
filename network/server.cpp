#include "ServerNetwork.h"
#include "strip.h"
#include "ChannelList.h"

ServerNetwork *g_network;
ChannelList *g_channel_list;

int checkLogin(std::string p_username, std::string p_password)
{
	printf("username: %s\n", p_username.c_str());
	printf("password: %s\n", p_password.c_str());

	return 0;
	return -1;	//bad username
	return -2;	//bad password
	return -3;	//username already logged in
}

void handle_message(Message p_message)
{
	printf("handle_message called\n");

	char *data = (char*)p_message.getData().getData();
	std::string data_str = data;
	Data response;
	got_here();

	if(p_message.getData().getType() == CLIENT_LOGIN_REQUEST)
	{
		got_here();
		printf("got \"CLIENT_LOGIN_REQUEST\"\n");
		got_here();
		std::string username = data_str.substr(0, 20);
		got_here();
		std::string password = data_str.substr(20, 20);
		got_here();
		strip(username);
		got_here();
		strip(password);

		got_here();
		int returned = checkLogin(username, password);
		got_here();
		if(returned == 0)	//this client babba ok kk
		{
		got_here();
			g_channel_list->newClient(p_message.getSocket(), username);
		got_here();
			response = Data(SERVER_LOGIN_OK, "", 0);
		got_here();

			g_channel_list->print();
		got_here();
		}
		else if(returned == -1)		//bad username
		{
			int why = 1;
			response = Data(SERVER_LOGIN_BAD, &why, sizeof(why));
		}
		else if(returned == -2)		//bad password
		{
			int why = 2;
			response = Data(SERVER_LOGIN_BAD, &why, sizeof(why));
		}

		g_network->sendData(p_message.getSocket(), response);
	}
	else if(p_message.getData().getType() == CLIENT_LOGOUT)
	{
		printf("socket %d sent CLIENT_LOGOUT", p_message.getSocket());
		g_channel_list->removeClient(p_message.getSocket());
		g_channel_list->print();
	}
	else if(p_message.getData().getType() == CLIENT_AUDIO_DATA)
	{
		printf("socket %d sent CLIENT_AUDIO_DATA", p_message.getSocket());
	}
	else if(p_message.getData().getType() == CLIENT_TEXT_DATA)
	{
		printf("got \"CLIENT_TEXT_DATA\"\n");

		std::string reciever = data_str.substr(0,20);
		strip(reciever);
		std::string message = data_str.substr(20);

		printf("\"%s\" says \"%s\" to \"%s\"\n", "", message.c_str(), reciever.c_str());
	}
	else if(p_message.getData().getType() == CLIENT_CHANNEL_CHANGE)
	{
		printf("socket %d sent CLIENT_CHANNEL_CHANGE", p_message.getSocket());

		std::string channel = data_str.substr(0,20);
		strip(channel);
		std::string password = data_str.substr(20,20);
		strip(password);

		int why;

		int returned = g_channel_list->changeClientChannel(p_message.getSocket(), channel, password);
		if(returned == -1)
		{
			report_error("user name passed too long");
		}
		else if(returned == -2)
		{
			report_error("channel name passed too long");
		}
		else if(returned == -3)
		{
			report_error("client not found");
		}
		else if(returned == -4)
		{
			report_error("channel name not found");
			why =  1;
		}
		else if(returned == -5)
		{
			report_error("bad password");
			why = 2;
		}
		else
			why = 3;
		response = Data(SERVER_CHANNEL_CHANGE_RESPONSE, &why, sizeof(why));
	}
	else if(p_message.getData().getType() == SOCKET_DISCONNECTED)
	{
		printf("socket %d sent SOCKET_DISCONNECTED", p_message.getSocket());
		g_channel_list->removeClient(p_message.getSocket());
		g_channel_list->print();
	}

}

int main()
{
	printf("running server\n");

	//setup networking
	g_network = new ServerNetwork();
	if(!g_network->initSocket("", 6789))
	{
		printf("couldn't initialize networking :(\nquiting\n");
		return -1;
	}
	else
	{
		printf("initialized networking\n");
	}

	//setup the channel list
	g_channel_list = new ChannelList();
	g_channel_list->newChannel("test1");
	g_channel_list->newChannel("test2");

	g_channel_list->print();

	Message incomming_message;
	while(true)
	{
		g_network->processNetworking();
		if(g_network->getMessage(incomming_message))	//means we have incomming data in incomming_message
		{
			handle_message(incomming_message);
		}
	}
}

