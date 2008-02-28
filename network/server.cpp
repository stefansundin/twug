#include "ServerNetwork.h"
#include "strip.h"
#include "Channel.h"

ServerNetwork *g_network;
Channel *g_channel_list;

void disconnect_client(int p_socket)
{
	g_channel_list->removeClient(p_socket);
	g_channel_list->print(0);
}

int checkLogin(std::string p_username, std::string p_password)
{
	printf("username: %s\n", p_username.c_str());
	printf("password: %s\n", p_password.c_str());

	return 0;
}

void handle_message(Message p_message)
{
	printf("handle_message called\n");
/*
#define CLIENT_LOGIN_REQUEST 0	//message is a string containing username and password concatenated. both should be filled out with 0 until they are 20 chars long (0 come at the end).
#define CLIENT_LOGOUT 1			//no message
#define CLIENT_AUDIO_DATA 2		//message is audio data
#define CLIENT_TEXT_DATA 3		//message is the username the text message is being sent to (filled out with 0 at the end until it is 20 chars long) concatenated with the text message.

#define CLIENT_CHANNEL_CHANGE 4	//message is the name of the channel the user wants to change to concatenated with the channel password both filled out with 0 until they are 20 chars long (filled at the end).
*/

	char *data = (char*)p_message.getData().getData();
	std::string data_str = data;

	if(p_message.getData().getType() == CLIENT_LOGIN_REQUEST)
	{
		printf("got \"CLIENT_LOGIN_REQUEST\"\n");

		Data response;

		std::string username = data_str.substr(0, 20);
		std::string password = data_str.substr(20, 20);
		strip(username);
		strip(password);

		if(checkLogin(username, password) == 0)	//this client babba ok kk
		{
			Client *c = new Client(p_message.getSocket(), username);
			g_channel_list->addClient(c);
			response = Data(SERVER_LOGIN_OK, "", 0);

			g_channel_list->print(0);
		}
		else if(checkLogin(username, password) == -1)		//bad username
		{
			int why = 1;
			response = Data(SERVER_LOGIN_BAD, &why, sizeof(why));
		}
		else if(checkLogin(username, password) == -2)		//bad password
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
		g_channel_list->print(0);
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

		Client *c = g_channel_list->getChannel(p_message.getSocket());
		if(!strip(channel))
			printf("couldn't strip channel at line %s", __LINE__);
	}

}

int main()
{
	printf("running server\n");

	//setup networking
	g_network = new ServerNetwork();
	g_network->setDisconnectClientCallback(disconnect_client);
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
	g_channel_list = new Channel("Lobby");
	g_channel_list->newSubchannel("test1");
	g_channel_list->newSubchannel("test2");

	Channel *test1 = g_channel_list->getSubchannel("test1");
	if(test1 == NULL)
		printf("didnt get test1\n");
	else
	{
		test1->newSubchannel("sub1");
	}

	printf("----Channels---------------\n");
	g_channel_list->print(0);
	printf("---------------------------\n");

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

