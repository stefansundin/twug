#include "ServerNetwork.h"

int main()
{
	printf("running server\n");

	ServerNetwork n = ServerNetwork();
	if(!n.initSocket("", 6789))
	{
		printf("couln't initialize networking :(\nquiting\n");
		return -1;
	}

	int selected_socket = 0;
	message_t incomming_message;

	while(true)
	{
		printf("calling processNetworking()\n");
		n.processNetworking();
		printf("processNetworking() returned\n");

		if(n.getMessage(incomming_message))		//means we have incomming data in incomming_message
		{
			printf("got message\n");

			switch(incomming_message.m_data.getType())
			{
				case CLIENT_LOGIN_REQUEST:
					Data d = Data(SERVER_LOGIN_OK, "", 0);
					n.sendData(incomming_message.m_socket, d);
					break;
			}
		}
	}
}

