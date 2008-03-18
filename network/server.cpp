#include <vector>

#include "ServerNetwork.h"
#include "strip.h"
#include "ClientPool.h"

#define DEFAULT_CHANNEL "lobby"

ServerNetwork *g_network;
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

int checkLogin(std::string p_username, std::string p_password)
{
	printf("username: %s\n", p_username.c_str());
	printf("password: %s\n", p_password.c_str());

	std::vector<std::string> client_names = g_client_pool->getClientNames();
	int i;
	for(i = 0; i < client_names.size(); i++)
	{
		if(client_names.at(i) == p_username)
			return -3;	//username already logged in
	}

	return 0;
	return -1;	//bad username
	return -2;	//bad password
}

void handle_message(Message p_message)
{
	printf("handle_message called\n");

	char *data = (char*)p_message.getData().getData();
	int length = p_message.getData().getLength();
	std::string data_str = data;
	Data response;

	if(p_message.getData().getType() == CLIENT_LOGIN_REQUEST)
	{
		printf("got \"CLIENT_LOGIN_REQUEST\"\n");
		std::string username = data_str.substr(0, 20);
		std::string password = data_str.substr(20, 20);
		strip(username);
		strip(password);

		int why = 0;
		int returned = checkLogin(username, password);
		if(returned == 0)	//this client babba ok kk
		{
			g_client_pool->addClient(username, DEFAULT_CHANNEL, p_message.getSocket());
			response = Data(SERVER_LOGIN_OK, "", 0);
			g_network->sendData(p_message.getSocket(), response);

			std::string channel = DEFAULT_CHANNEL;
			fill(username, 20);
			fill(channel, 20);
			std::string m = username + channel;
			response = Data(SERVER_ADD_CLIENT, m.c_str(), m.size()+1);
			std::vector<std::string> client_names = g_client_pool->getClientNames();
			int i, s;
			for(i = 0; i < client_names.size(); i++)
			{
				g_client_pool->nameToSocket(client_names.at(i), &s);
				g_network->sendData(s, response);
			}

			printClientPool(g_client_pool);
		}
		else if(returned == -1)		//bad username
		{
			why = 1;
			printf("bad username\n");
		}
		else if(returned == -2)		//bad password
		{
			why = 2;
			printf("bad password\n");
		}
		else if(returned == -3)		//username already taken
		{
			why = 3;
			printf("username already taken\n");
		}

		response = Data(SERVER_LOGIN_BAD, &why, sizeof(why));
		g_network->sendData(p_message.getSocket(), response);
	}
	else if(p_message.getData().getType() == CLIENT_LOGOUT)
	{
		printf("socket %d sent CLIENT_LOGOUT", p_message.getSocket());

		std::string remove_name;
		g_client_pool->socketToName(p_message.getSocket(), &remove_name);

		g_client_pool->removeClient(p_message.getSocket());
		printClientPool(g_client_pool);

		response = Data(SERVER_REMOVE_CLIENT, remove_name.c_str(), remove_name.size()+1);
		g_network->sendData(p_message.getSocket(), response);
	}
	else if(p_message.getData().getType() == CLIENT_AUDIO_DATA)
	{
		printf("socket %d sent CLIENT_AUDIO_DATA", p_message.getSocket());

		if(!g_client_pool->hasSocket(p_message.getSocket()))
		{
			log_this("dropping message comming from a socket not logged in");
			return;
		}

		std::string client_name;
		if(!g_client_pool->socketToName(p_message.getSocket(), &client_name))
		{
			log_this("couldn't get client name");
			return;
		}
		std::string channel_name;
		if(!g_client_pool->clientNameToChannelName(client_name, &channel_name))
		{
			log_this("couldn't get channel name");
			return;
		}
		std::vector<std::string> channel_client_names;
		if(!g_client_pool->getChannelClientNames(channel_name, &channel_client_names))
		{
			log_this("couldn't get channel client names");
			return;
		}

		int i, s;
		for(i = 0; i < channel_client_names.size(); i++)
		{
			if(!g_client_pool->nameToSocket(channel_client_names.at(i), &s))
			{
				log_this("couldn't get socket from name");
				return;
			}
			response = Data(SERVER_AUDIO_DATA, data, length);
			g_network->sendData(s, response);
		}
	}
	else if(p_message.getData().getType() == CLIENT_TEXT_DATA)
	{
		printf("got \"CLIENT_TEXT_DATA\"\n");

		if(!g_client_pool->hasSocket(p_message.getSocket()))
		{
			log_this("dropping message comming from a socket not logged in");
			return;
		}

		std::string reciever = data_str.substr(0,20);
		std::string message = data_str.substr(20);

		strip(reciever);
		int recv_socket;
		if(!g_client_pool->nameToSocket(reciever, &recv_socket))
		{
			printf("no client named \"%s\" found\n", reciever.c_str());
			return;
		}
		fill(reciever, 20);

		std::string m = reciever + message;

		response = Data(SERVER_TEXT_DATA, m.c_str(), m.size()+1);
		g_network->sendData(recv_socket, response);

		std::string sender;
		g_client_pool->socketToName(p_message.getSocket(), &sender);
		strip(reciever);
		printf("\"%s\" says \"%s\" to \"%s\"\n", sender.c_str(), message.c_str(), reciever.c_str());
	}
	else if(p_message.getData().getType() == CLIENT_CHANNEL_CHANGE)
	{
		printf("socket %d sent CLIENT_CHANNEL_CHANGE", p_message.getSocket());

		if(!g_client_pool->hasSocket(p_message.getSocket()))
		{
			log_this("dropping message comming from a socket not logged in");
			return;
		}

		std::string channel = data_str.substr(0,20);
		strip(channel);
		std::string password = data_str.substr(20,20);
		strip(password);

		int why;

		int returned = 0;
		if(returned == -1)
		{
			report_error("user name passed too long");
		}
		else if(returned == -2)
		{
			report_error("channel name passed too long");
		}
		else if(returned == -3)
		{
			report_error("client not found");
		}
		else if(returned == -4)
		{
			report_error("channel name not found");
			why =  1;
		}
		else if(returned == -5)
		{
			report_error("bad password");
			why = 2;
		}
		else
			why = 3;
		response = Data(SERVER_CHANNEL_CHANGE_RESPONSE, &why, sizeof(why));
	}
	else if(p_message.getData().getType() == SOCKET_DISCONNECTED)
	{
		printf("socket %d sent SOCKET_DISCONNECTED\n", p_message.getSocket());

		if(!g_client_pool->hasSocket(p_message.getSocket()))
		{
			log_this("not telling clients to remove someone not there");
			return;
		}

		std::string remove_name;
		g_client_pool->socketToName(p_message.getSocket(), &remove_name);

		g_client_pool->removeClient(p_message.getSocket());
		printClientPool(g_client_pool);

		response = Data(SERVER_REMOVE_CLIENT, remove_name.c_str(), remove_name.size()+1);
		std::vector<std::string> client_names = g_client_pool->getClientNames();
		int i, s;
		for(i = 0; i < client_names.size(); i++)
		{
			g_client_pool->nameToSocket(client_names.at(i), &s);
			g_network->sendData(s, response);
		}
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

	//setup the client pool
	g_client_pool = new ClientPool();

	printClientPool(g_client_pool);

	Message incomming_message;
	while(true)
	{
		g_network->processNetworking();
		if(g_network->getMessage(incomming_message))	//means we have incomming data in incomming_message
		{
			handle_message(incomming_message);
		}
	}
}

