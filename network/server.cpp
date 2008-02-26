#include "ServerNetwork.h"

#include "strip.h"

int readPassword(std::string p_str)
{
	std::string username = p_str.substr(0, 20);
	std::string password = p_str.substr(20, 20);

	printf("username: %s\n", username.c_str());
	printf("password: %s\n", password.c_str());

	strip(username);
	strip(password);

	printf("username: %s\n", username.c_str());
	printf("password: %s\n", password.c_str());

	return 0;
}

int main()
{
	printf("running server\n");

	ServerNetwork n = ServerNetwork();
	if(!n.initSocket("", 6789))
	{
		printf("couldn't initialize networking :(\nquiting\n");
		return -1;
	}

	int selected_socket = 0;
	Message incomming_message;

	while(true)
	{
		n.processNetworking();

		if(n.getMessage(incomming_message))		//means we have incomming data in incomming_message
		{
			printf("got message\n");

			switch(incomming_message.getData().getType())
			{
				case CLIENT_LOGIN_REQUEST:
					Data d = Data(SERVER_LOGIN_OK, "", 0);
					std::string temp = (char*)incomming_message.getData().getData();
					printf("temp.c_str(): \"%s\"\n", temp.c_str());
					printf("temp.size(): %d\n", temp.size());

					n.sendData(incomming_message.getSocket(), d);
					printf("got login request\n");

					readPassword(temp);
					break;
			}
		}
	}
}

