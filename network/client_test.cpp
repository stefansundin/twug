#include "ClientNetwork.h"

int main(int argc, char **argv)
{
	if(argc < 3)
	{
		printf("usage: <appname> <username> <password>\n");
		return 0;
	}

	ClientNetwork n = ClientNetwork();

	int status = n.connect("127.0.0.1", 6789);
	printf("connect returned: %d\n", status);

	if(status == 0)
	{
		printf("connection successful!\n");
	}
	else
	{
		printf("connection failed :(\n");
	}

	n.loginRequest(argv[1], argv[2]);

	Message incomming_message;

	while(n.processNetworking())
	{
		if(n.getMessage(incomming_message))		//means we have incomming data in incomming_message
		{
			printf("got message\n");

			switch(incomming_message.getData().getType())
			{
				case SERVER_LOGIN_OK:
					char *t = (char*)incomming_message.getData().getData();
					char *k = new char[incomming_message.getData().getLength()+1];
					memcpy(k, t, incomming_message.getData().getLength());
					k[incomming_message.getData().getLength()] = '\0';

					std::string temp = k;
					printf("temp.c_str(): \"%s\"\n", temp.c_str());
					printf("temp.size(): %d\n", temp.size());
					printf("login ok\n");
					break;
			}
		}
	}
	printf("disconnected\n");
}

