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
		n.processNetworking();

		if(n.getMessage(incomming_message))		//means we have incomming data in incomming_message
		{
			printf("got message\n");

			switch(incomming_message.m_data.getType())
			{
				case CLIENT_LOGIN_REQUEST:
					Data d = Data(SERVER_LOGIN_OK, "", 0);
					std::string temp = (char*)incomming_message.m_data.getData();
					printf("temp.c_str(): \"%s\"\n", temp.c_str());
					printf("temp.size(): %d\n", temp.size());

					n.sendData(incomming_message.m_socket, d);
					printf("got login request\n");
					break;
			}
		}
	}
}

