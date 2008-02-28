#include "ClientNetwork.h"

char *G_SERVER_IP = "127.0.0.1";
int G_SERVER_PORT = 6789;

void handleMessage(Message p_message)
{
/*
#define SERVER_LOGIN_OK 0		//no message
#define SERVER_LOGIN_BAD 1		//int why: bit 1 set for bad username, bit 2 set for bad password
#define SERVER_DISCONNECT 2		//int why: ?

#define SERVER_AUDIO_DATA 3		//raw sound data: 
#define SERVER_TEXT_DATA 4		//message is the username the text message comes from (filled out with 0 at the end until it is 20 chars long) concatenated with the text message.

#define SERVER_BUDDY_LIST_UPDATE 5	//
*/

	char *data = (char*)p_message.getData().getData();

	if(p_message.getData().getType() == SERVER_LOGIN_OK)
	{
			printf("got \"SERVER_LOGIN_OK\"\n");
	}
	else if(p_message.getData().getType() == SERVER_LOGIN_BAD)
	{
		int why = *((int*)data);

		printf("got \"SERVER_LOGIN_BAD\"\n");
		if(why & 0x1)
			printf("bad username\n");
		if(why & 0x2)
			printf("bad password\n");
	}
	else if(p_message.getData().getType() == SERVER_DISCONNECT)
	{
		int why = *((int*)data);

		printf("got \"SERVER_DISCONNECT\"\n");
		if(why & 0x1)
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
		std::string message = str.substr(20);

		printf("\"%s\": \"%s\"\n", sender.c_str(), message.c_str());
	}

		/*
			char *t = (char*)incomming_message.getData().getData();
			char *k = new char[incomming_message.getData().getLength()+1];
			memcpy(k, t, incomming_message.getData().getLength());
			k[incomming_message.getData().getLength()] = '\0';
		*/
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
				printf("username set to %d\n", username.c_str());
			}
		}
		else if(strcmp(argv[i],"-password") == 0)
		{
			if(argc > i+1)
			{
				password = argv[i+1];
				printf("password set to %d\n", password.c_str());
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

	//user argument 1 and 2 for username and password and send
	n.loginRequest(username, password);

	n.sendText("oolong", "hejsan kanin!");

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

