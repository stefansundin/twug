#include "ClientNetwork.h"

int main()
{
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

	n.loginRequest("basse", "omgkaka");

	send(n.m_socket, "hejsan", strlen("hejsan"), 0);

	message_t incomming_message;

	while(n.processNetworking())
	{
		printf("still connected\n");

		if(n.getMessage(incomming_message))		//means we have incomming data in incomming_message
		{
			printf("got message\n");

			switch(incomming_message.m_data.getType())
			{
				case SERVER_LOGIN_OK:
					printf("login ok\n");
					break;
			}
		}
	}
	printf("disconnected\n");
}
