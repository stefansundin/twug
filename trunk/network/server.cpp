//kill me now
#define SERVER

#include <vector>

#include "ServerNetwork.h"
#include "strip.h"
#include "ClientPool.h"

ServerNetwork *g_network;
ClientPool *g_client_pool;

void printClientPool(ClientPool *p_client_pool)
{
	std::vector<std::string> channel_names = g_client_pool->getChannelNames();
	std::vector<std::string> client_names;

	printf("----Channels------------\n");

	unsigned int i, j;
	for(i = 0; i < channel_names.size(); i++)
	{
		printf("%s:\n", channel_names.at(i).c_str());
		if(!g_client_pool->getChannelClientNames(channel_names.at(i), client_names))
		{
			log_this("Could not get channel client names.");
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
	print_me("Username: "+p_username+".");
	print_me("Password: "+p_password+".");

	std::vector<std::string> client_names = g_client_pool->getClientNames();
	unsigned int i;
	for(i = 0; i < client_names.size(); i++)
	{
		if(client_names.at(i) == p_username)
			return -3;	//username already logged in
	}

	return 0;
	return -1;	//bad username
	return -2;	//bad password
}

void broadcastSend(Data p_data)
{
	std::vector<std::string> channel_names = g_client_pool->getChannelNames();

	for(unsigned int i = 0; i < channel_names.size(); i++)
	{
		std::vector<std::string> client_names;
		g_client_pool->getChannelClientNames(channel_names.at(i), client_names);

		for(unsigned int j = 0; j < client_names.size(); j++)
		{
			int socket;
			if(!g_client_pool->nameToSocket(client_names.at(j), socket))
			{
				print_me("Could not get socket from name ("+client_names.at(j)+").");
			}
			else
			{
				g_network->sendData(socket, p_data);
			}
		}
	}
}

void sendChannelList(int p_socket)
{
	std::vector<std::string> channel_names = g_client_pool->getChannelNames();

	for(unsigned int i = 0; i < channel_names.size(); i++)
	{
		std::string channel_name = channel_names.at(i);
		fill(channel_name, MESSAGE_FILL);

		Data data = Data(SERVER_ADD_CHANNEL, channel_name);
		g_network->sendData(p_socket, data);

		std::vector<std::string> client_names;
		g_client_pool->getChannelClientNames(channel_names.at(i), client_names);

		for(unsigned int j = 0; j < client_names.size(); j++)
		{
			std::string client_name = client_names.at(j);
			fill(client_name, MESSAGE_FILL);

			std::string to_send = client_name + channel_name;
			data = Data(SERVER_ADD_CLIENT, to_send);
			g_network->sendData(p_socket, data);
		}
	}
}

//this seems a little useless now, about 10 minutes after I wrote it...
void broadcastChannelList()
{
	std::vector<std::string> channel_names = g_client_pool->getChannelNames();

	for(unsigned int i = 0; i < channel_names.size(); i++)
	{
		std::string channel_name = channel_names.at(i);
		fill(channel_name, MESSAGE_FILL);

		std::vector<std::string> client_names;
		g_client_pool->getChannelClientNames(channel_names.at(i), client_names);

		for(unsigned int j = 0; j < client_names.size(); j++)
		{
			std::string client_name = client_names.at(j);
			fill(client_name, MESSAGE_FILL);

			std::string to_send = client_name + channel_name;
			Data data = Data(SERVER_ADD_CLIENT, to_send);
			broadcastSend(data);
		}
	}
}

void handleMessage(Message p_message)
{
	print_me("Handling messages.");

	char *data = (char*)p_message.getData().getData();
	unsigned int length = p_message.getData().getLength();
	Data response;

	std::string data_str;
	unsigned int i;
	for(i = 0; i < length; i++)
	{
		data_str.push_back(data[i]);
	}

	if(p_message.getData().getType() == CLIENT_LOGIN_REQUEST)
	{
		print_me("Got \"CLIENT_LOGIN_REQUEST\".");

		//check that the message is of proper length
		if(data_str.size() != MESSAGE_FILL*2)
		{
			print_me("Message not of proper length.");
			return;
		}

		std::string username = data_str.substr(0, MESSAGE_FILL);
		std::string password = data_str.substr(MESSAGE_FILL, MESSAGE_FILL);
		strip(username);
		strip(password);

		int returned = checkLogin(username, password);
		if(returned == 0)	//this client babba ok kk
		{
			//respond that the login was successful
			response = Data(SERVER_LOGIN_OK, "");
			g_network->sendData(p_message.getSocket(), response);

			//send a complete channel/client list to the new client
			sendChannelList(p_message.getSocket());

			//add the new client to the client pool before we tell everyone to add it
			//this so that it will add itself too
			g_client_pool->addClient(username, DEFAULT_CHANNEL, p_message.getSocket());

			fill(username, MESSAGE_FILL);
			std::string channel_name = DEFAULT_CHANNEL;
			fill(channel_name, MESSAGE_FILL);
			std::string to_send = username + channel_name;
			response = Data(SERVER_ADD_CLIENT, to_send);

			broadcastSend(response);

			printClientPool(g_client_pool);
		}
		else if(returned == -1)		//bad username
		{
			response = Data(SERVER_LOGIN_BAD, "1", 1);
			g_network->sendData(p_message.getSocket(), response);
			print_me("Bad username.");
		}
		else if(returned == -2)		//bad password
		{
			response = Data(SERVER_LOGIN_BAD, "2", 1);
			g_network->sendData(p_message.getSocket(), response);
			print_me("Bad password.");
		}
		else if(returned == -3)		//username already taken
		{
			response = Data(SERVER_LOGIN_BAD, "3", 1);
			g_network->sendData(p_message.getSocket(), response);
			printf("Username already taken.");
		}
	}
/*	//this message is handled by the same as SOCKET_DISCONNECTED for the time being
	else if(p_message.getData().getType() == CLIENT_LOGOUT)
	{
		print_me("Got \"CLIENT_LOGOUT\".");

		std::string remove_name;
		g_client_pool->socketToName(p_message.getSocket(), remove_name);

		g_client_pool->removeClient(p_message.getSocket());
		printClientPool(g_client_pool);

		response = Data(SERVER_REMOVE_CLIENT, remove_name);
		broadcastSend(response);
	}
*/
	else if(p_message.getData().getType() == CLIENT_AUDIO_DATA)
	{
		print_me("Got \"CLIENT_AUDIO_DATA\".");

		if(!g_client_pool->hasSocket(p_message.getSocket()))
		{
			log_this("Dropping message comming from a socket not logged in.");
			return;
		}

		std::string client_name;
		if(!g_client_pool->socketToName(p_message.getSocket(), client_name))
		{
			log_this("Could not get client name.");
			return;
		}
		std::string channel_name;
		if(!g_client_pool->clientNameToChannelName(client_name, channel_name))
		{
			log_this("Could not get channel name.");
			return;
		}
		std::vector<std::string> channel_client_names;
		if(!g_client_pool->getChannelClientNames(channel_name, channel_client_names))
		{
			log_this("Could not get channel client names.");
			return;
		}

		unsigned int i;
		int s;
		for(i = 0; i < channel_client_names.size(); i++)
		{
			if (client_name != channel_client_names.at(i))
			{
				if(!g_client_pool->nameToSocket(channel_client_names.at(i), s))
				{
					log_this("Could not get socket from name.");
					return;
				}
				response = Data(SERVER_AUDIO_DATA, data, length);
				g_network->sendData(s, response);

				std::string to;
				g_client_pool->socketToName(s, to);
				printf("Sent: type=SERVER_AUDIO_DATA to (%s)\n", to.c_str());
			}
		}
	}
	else if(p_message.getData().getType() == CLIENT_TEXT_DATA)
	{
		print_me("Got \"CLIENT_TEXT_DATA\".");

		//check that the message is of proper length
		if(data_str.size() < MESSAGE_FILL)
		{
			print_me("Message not of proper length.");
			return;
		}

		if(!g_client_pool->hasSocket(p_message.getSocket()))
		{
			log_this("Dropping message comming from a socket not logged in.");
			return;
		}

		std::string reciever = data_str.substr(0,MESSAGE_FILL);
		std::string message = data_str.substr(MESSAGE_FILL);

		strip(reciever);
		int recv_socket;
		if(!g_client_pool->nameToSocket(reciever, recv_socket))
		{
			log_this("No client ("+reciever+") found.");
			return;
		}

		std::string sender = "";
		if(!g_client_pool->socketToName(p_message.getSocket(), sender))
		{
			log_this("Could not get name from socket.");
			return;
		}
		fill(sender, MESSAGE_FILL);
		std::string m = sender + message;

		response = Data(SERVER_TEXT_DATA, m);
		g_network->sendData(recv_socket, response);
	}
	else if(p_message.getData().getType() == CLIENT_TEXT_BROADCAST)
	{
		std::string sender = "";
		if(!g_client_pool->socketToName(p_message.getSocket(), sender))
		{
			log_this("Could not get name from socket.");
			return;
		}

		//checking privileges
		int privs = PRIV_NONE;
		if(!g_client_pool->getClientPrivileges(sender, privs))
		{
			log_this("Could not get privileges from name.");
			return;
		}

		if(privs & PRIV_TEXT_BROADCAST)
		{
			//broadcast
			std::string title = "Broadcast";
			fill(title, MESSAGE_FILL);
			std::string message = title + data_str;
			response = Data(SERVER_NOTIFY, message);
			broadcastSend(response);
		}
		else
		{
			std::string denied = "Permission Denied";
			fill(denied, MESSAGE_FILL);
			response = Data(SERVER_ERROR_NOTIFY, denied+"You are not allowed to broadcast text.");
			g_network->sendData(p_message.getSocket(), response);
		}
	}
	else if(p_message.getData().getType() == CLIENT_CHANNEL_CHANGE)
	{
		print_me("Got \"CLIENT_CHANNEL_CHANGE\".");

		//check that the message is of proper length
		if(data_str.size() != MESSAGE_FILL*2)
		{
			print_me("Message not of proper length.");
			return;
		}

		if(!g_client_pool->hasSocket(p_message.getSocket()))
		{
			log_this("Dropping message comming from a socket not logged in.");
			return;
		}

		std::string channel = data_str.substr(0,MESSAGE_FILL);
		strip(channel);
		std::string password = data_str.substr(MESSAGE_FILL,MESSAGE_FILL);
		strip(password);
		print_me("Password: ("+password+").");

		std::string client_name;
		if(!g_client_pool->socketToName(p_message.getSocket(), client_name))
		{
			log_this("Could not get name from socket.");
			return;
		}

		std::string result;
		int returned = g_client_pool->switchClientChannels(client_name, channel, password);
		if(returned == 0)
		{
			print_me("Move of client ("+client_name+") to channel ("+channel+") successful.");
			result = "0";

			fill(client_name, MESSAGE_FILL);
			response = Data(SERVER_REMOVE_CLIENT, client_name);
			broadcastSend(response);

			fill(channel, MESSAGE_FILL);
			response = Data(SERVER_ADD_CLIENT, client_name+channel);
			broadcastSend(response);
		}
		else if(returned == -3)
		{
			report_error("Client not found.");
			result = "3";
		}
		else if(returned == -4)
		{
			report_error("Channel name not found.");
			result = "1";
		}
		else if(returned == -5)
		{
			report_error("Bad password.");
			result = "2";
		}
		else
		{
			result = "3";
		}
		response = Data(SERVER_CHANNEL_CHANGE_RESPONSE, result);
		g_network->sendData(p_message.getSocket(), response);
	}
	else if(p_message.getData().getType() == CLIENT_ADMIN_CREATE_CHANNEL)
	{
		print_me("got \"CLIENT_ADMIN_CREATE_CHANNEL\".");

		//check that the message is of proper length
		if(data_str.size() != MESSAGE_FILL)
		{
			print_me("Message not of proper length.");
			return;
		}

		std::string client_name;
		if(!g_client_pool->socketToName(p_message.getSocket(), client_name))
		{
			log_this("Could not get name from socket.");
			return;
		}

		int privs = PRIV_NONE;
		if(!g_client_pool->getClientPrivileges(client_name, privs))
		{
			log_this("Could not get privileges from name.");
			return;
		}

		if(privs & PRIV_CREATE_CHANNEL)
		{
			std::string channel_name = data_str.substr(0,MESSAGE_FILL);
			strip(channel_name);

			g_client_pool->addChannel(channel_name, "");

			std::vector<std::string> channel_names = g_client_pool->getChannelNames();
			std::vector<std::string> client_names;
			int socket;
			unsigned int i, j;
			for(i = 0; i < channel_names.size(); i++)
			{
				g_client_pool->getChannelClientNames(channel_names.at(i), client_names);
				for(j = 0; j < client_names.size(); j++)
				{
					if(!g_client_pool->nameToSocket(client_names.at(j), socket))
					{
						log_this("Could not get socket from name.");
						return;
					}

					fill(channel_name, MESSAGE_FILL);
					response = Data(SERVER_ADD_CHANNEL, channel_name);
					g_network->sendData(socket, response);

					std::string to;
					g_client_pool->socketToName(p_message.getSocket(), to);
					print_me("Sent SERVER_ADD_CHANNEL ("+channel_name+") to ("+client_names.at(j)+")");
					printf("to socket (%d).\n", p_message.getSocket());
				}
			}
		}
		else
		{
			log_this("Client ("+client_name+") is not allowed to create channels.");
			return;
		}
	}
	else if(p_message.getData().getType() == CLIENT_ADMIN_REMOVE_CHANNEL)
	{
		print_me("Got \"CLIENT_ADMIN_REMOVE_CHANNEL\".");

		//check that the message is of proper length
		if(data_str.size() != MESSAGE_FILL)
		{
			print_me("Message not of proper length.");
			return;
		}

		std::string channel_name = data_str.substr(0, MESSAGE_FILL);
		strip(channel_name);

		std::vector<std::string> client_names;
		if(g_client_pool->removeChannel(channel_name))
		{
			print_me("Channel ("+channel_name+") removed.");
			fill(channel_name, MESSAGE_FILL);
			Data response = Data(SERVER_REMOVE_CHANNEL, channel_name);
			broadcastSend(response);
		}
		else
		{
			log_this("Removal of channel ("+channel_name+") failed.");
		}

		return;
	}
	else if(p_message.getData().getType() == SOCKET_DISCONNECTED
			|| p_message.getData().getType() == CLIENT_LOGOUT)
	{
		print_me("Got \"SOCKET_DISCONNECTED\" or \"CLIENT_LOGOUT\"");

		if(!g_client_pool->hasSocket(p_message.getSocket()))
		{
			log_this("Not telling clients to remove someone not there.");
			return;
		}

		std::string remove_name;
		if(!g_client_pool->socketToName(p_message.getSocket(), remove_name))
		{
			log_this("Could not get name from socket.");
			return;
		}

		if(!g_client_pool->removeClient(p_message.getSocket()))
		{
			log_this("Could not remove client ("+remove_name+").");
			printClientPool(g_client_pool);
			return;
		}
		printClientPool(g_client_pool);

		fill(remove_name, MESSAGE_FILL);
		response = Data(SERVER_REMOVE_CLIENT, remove_name);
		broadcastSend(response);
	}
	else
	{
		log_this("Got unknown message ("+data_str+").");
	}
}

int main()
{
	printf("running server\n");

	//setup networking
	g_network = new ServerNetwork();
	if(!g_network->initSocket("", 6789))
	{
		print_me("Could not initialize networking... :(\nQuiting.");
		return -1;
	}
	else
	{
		print_me("Initialized networking.\n");
	}

	//setup the client pool
	g_client_pool = new ClientPool();
	g_client_pool->addChannel(DEFAULT_CHANNEL, "");

	printClientPool(g_client_pool);

	Message incomming_message;
	while(true)
	{
		g_network->processNetworking();
		while(g_network->getMessage(incomming_message))	//means we have incomming data in incomming_message
		{
			handleMessage(incomming_message);
		}
	}
}

