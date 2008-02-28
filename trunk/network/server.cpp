#include "ServerNetwork.h"
#include "strip.h"
#include "ChannelList.h"

ServerNetwork *g_network;
ChannelList *g_channel_list;

int readPassword(std::string p_str)
{
	std::string username = p_str.substr(0, 20);
	std::string password = p_str.substr(20, 20);

	strip(username);
	strip(password);

	printf("username: %s\n", username.c_str());
	printf("password: %s\n", password.c_str());

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

		readPassword(data_str);

		Data d = Data(SERVER_LOGIN_OK, "", 0);
		g_network->sendData(p_message.getSocket(), d);
	}
	else if(p_message.getData().getType() == CLIENT_LOGOUT)
	{
		printf("socket %d sent CLIENT_LOGOUT", p_message.getSocket());
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
		if(!strip(channel))
			printf("couldn't strip channel at line %s", __LINE__);
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

	Channel *test1;
	if(!g_channel_list->getChannel("test1", test1))
		printf("didnt get test1\n");
	printf("test1 = \"%d\"\n", (unsigned int)test1);
	test1->newSubchannel("sub1");

	g_channel_list->printChannels();

	Message incomming_message;
	while(true)
	{
		g_network->processNetworking();
		if(g_network->getMessage(incomming_message))	//means we have incomming data in incomming_message
		{
			printf("got message\n");
			handle_message(incomming_message);
		}
	}
}

