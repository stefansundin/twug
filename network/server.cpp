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
	Data response;
	std::string temp;

	switch(p_message.getData().getType())
	{
		case CLIENT_LOGIN_REQUEST:
			printf("got \"CLIENT_LOGIN_REQUEST\"\n");

			temp = (char*)p_message.getData().getData();

			readPassword(temp);

			response = Data(SERVER_LOGIN_OK, "", 0);
			g_network->sendData(p_message.getSocket(), response);
			break;

		case CLIENT_TEXT_DATA:
			printf("got \"CLIENT_TEXT_DATA\"\n");

			temp = (char*)p_message.getData().getData();

			

			printf("\"%s\" said \"%s\" to \"%s\"\n", "", "", "");

			break;
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

	//setup the channel list
	g_channel_list = new ChannelList();
	g_channel_list->newChannel("Lobby");
	g_channel_list->defaultChannel("Lobby");

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

