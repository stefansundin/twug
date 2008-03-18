#include "ClientNetwork.h"
#include "ClientPool.h"

char *G_SERVER_IP = "127.0.0.1";
int G_SERVER_PORT = 6789;

ClientPool *g_client_pool;

void printClientPool(ClientPool *p_client_pool)
{
	std::vector<std::string> channel_names = g_client_pool->getChannelNames();
	std::vector<std::string> client_names;

	printf("----Channels------------\n");

	int i, j;
	for(i = 0; i < channel_names.size(); i++)
	{
		printf("%s:\n", channel_names.at(i).c_str());
		if(!g_client_pool->getChannelClientNames(channel_names.at(i), &client_names))
		{
			log_this("couldn't get channel client names");
			return;
		}
		for(j = 0; j < client_names.size(); j++)
		{
			printf("\t%s\n", client_names.at(j).c_str());
		}
	}
	printf("------------------------\n");
}

void handleMessage(Message p_message)
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
			got_here();
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

		printf("should add \"%s\" to \"%s\"\n", client_name.c_str(), channel_name.c_str());
	}
	else if(p_message.getData().getType() == SERVER_MOVE_CLIENT)
	{
		printf("got \"SERVER_MOVE_CLIENT\"\n");

		std::string client_name = data_str.substr(0, 20);
		std::string channel_name = data_str.substr(20);
		strip(client_name);
		strip(channel_name);

		printf("should move \"%s\" to \"%s\"\n", client_name.c_str(), channel_name.c_str());
	}
	else if(p_message.getData().getType() == SERVER_REMOVE_CLIENT)
	{
		printf("got \"SERVER_REMOVE_CLIENT\"\n");

		std::string client_name = data_str.substr(0, 20);
		strip(client_name);

		printf("should remove \"%s\"\n", client_name.c_str());
	}
}

int main(int argc, char **argv)
{
	printf("running client\n");

	std::string username = "average_joe", password = "1234";

	//check arguments
	int i;
	for(i = 0; i < argc; i++)
	{
		if(strcmp(argv[i],"-ip") == 0)	//argv[i] is "-p"
		{
			if(argc > i+1)	//is there an argument after "-p"?
			{
				G_SERVER_IP = argv[i+1];
				printf("ip set to %s\n", G_SERVER_IP);
			}
		}
		else if(strcmp(argv[i],"-port") == 0)
		{
			if(argc > i+1)
			{
				G_SERVER_PORT = atoi(argv[i+1]);
				printf("port set to %d\n", G_SERVER_PORT);
			}
		}
		else if(strcmp(argv[i],"-username") == 0)
		{
			if(argc > i+1)
			{
				username = argv[i+1];
				printf("username set to %s\n", username.c_str());
			}
		}
		else if(strcmp(argv[i],"-password") == 0)
		{
			if(argc > i+1)
			{
				password = argv[i+1];
				printf("password set to %s\n", password.c_str());
			}
		}
	}

	//setup networking
	ClientNetwork n = ClientNetwork();
	if(n.connect(G_SERVER_IP, G_SERVER_PORT) == 0)
	{
		printf("connection successful!\n");
	}
	else
	{
		printf("connection failed :(\n");
	}

	g_client_pool = new ClientPool();

	//use argument 1 and 2 for username and password and send
	n.loginRequest(username, password);

	n.sendText("OMG", "hejsan kanin!");

	//check messages
	Message incomming_message;
	while(n.processNetworking())
	{
		if(n.getMessage(incomming_message))		//means we have incomming data in incomming_message
		{
			printf("got message\n");
			handleMessage(incomming_message);
		}
	}

	printf("disconnected\n");
	return 0;
}

