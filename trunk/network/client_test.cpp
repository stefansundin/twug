#include "ClientNetwork.h"

#define SERVER_IP "10.200.34.254"
#define SERVER_PORT 6789

void handleMessage(Message p_message)
{
	switch(p_message.getData().getType())
	{
		/*
			char *t = (char*)incomming_message.getData().getData();
			char *k = new char[incomming_message.getData().getLength()+1];
			memcpy(k, t, incomming_message.getData().getLength());
			k[incomming_message.getData().getLength()] = '\0';
		*/
		case SERVER_LOGIN_OK:
			printf("got \"SERVER_LOGIN_OK\"\n");
			break;
	}
}

int main(int argc, char **argv)
{
	printf("running client\n");

	//check arguments
	if(argc < 3)
	{
		printf("usage: <appname> <username> <password>\n");
		return 0;
	}

	//setup networking
	ClientNetwork n = ClientNetwork();
	if(n.connect(SERVER_IP, SERVER_PORT) == 0)
	{
		printf("connection successful!\n");
	}
	else
	{
		printf("connection failed :(\n");
	}

	//user argument 1 and 2 for username and password and send
	n.loginRequest(argv[1], argv[2]);

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

