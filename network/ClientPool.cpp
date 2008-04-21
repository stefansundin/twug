#include "ClientPool.h"

ClientPool::ClientPool()
{
}
ClientPool::~ClientPool()
{
}


std::vector<std::string> ClientPool::getClientNames() const
{
	print_me("Retrieving all client names");

	std::vector<std::string> client_names;
	std::string client = "";

	std::map<Channel*, std::vector<Client> >::const_iterator itr;
	for(itr = m_channel_list.begin(); itr != m_channel_list.end(); itr++)
	{
		std::vector<Client> channel_clients = itr->second;
		for(unsigned int i = 0; i < channel_clients.size(); i++)
		{
			client = channel_clients.at(i).getName();
			client_names.push_back(client);
		}
	}

	return client_names;
}
std::vector<std::string> ClientPool::getChannelNames() const
{
	print_me("Getting channel names");

	std::vector<std::string> channel_names;

	if(m_channel_list.size() < 1)
	{
		return channel_names;
	}

	std::map<Channel*, std::vector<Client> >::const_iterator itr;
	for(itr = m_channel_list.begin(); itr != m_channel_list.end(); itr++)
	{
		std::string name = itr->first->getName();
		channel_names.push_back(name);
	}

	return channel_names;
}


//returns 0 if successful
//returns -1 if channel wasn't found
//returns -2 if client already exists
//returns -3 if same socket already exists
int ClientPool::addClient(std::string p_name, std::string p_channel_name, int p_socket)
{
	print_me("Adding client ("+p_name+") to ("+p_channel_name+")");

	bool channel_found = false;
	std::map<Channel*, std::vector<Client> >::iterator channel_itr;

	std::map<Channel*, std::vector<Client> >::iterator itr;
	for(itr = m_channel_list.begin(); itr != m_channel_list.end(); itr++)
	{
		if(itr->first->getName() == p_channel_name)
		{
			channel_found = true;
			channel_itr = itr;

			std::vector<Client> channel_clients = (*itr).second;
			for(unsigned int i = 0; i < channel_clients.size(); i++)
			{
				if(channel_clients.at(i).getName() == p_name)
				{
					return -2;
				}
				if(p_socket != 0 && channel_clients.at(i).getSocket() == p_socket)
				{
					return -3;
				}
			}
		}
	}

	if(channel_found)
	{
		channel_itr->second.push_back(Client(p_name, p_socket));
		return 0;
	}
	else
	{
		return -1;
	}
}
bool ClientPool::addChannel(std::string p_name, std::string p_password)
{
	print_me("Adding channel ("+p_name+")");

	std::map<Channel*, std::vector<Client> >::iterator itr;
	for(itr = m_channel_list.begin(); itr != m_channel_list.end(); itr++)
	{
		if(itr->first->getName() == p_name)
		{
			return false;
		}
	}

	Channel *channel = new Channel(p_name, p_password);
	std::vector<Client> clients;
	m_channel_list[channel] = clients;
	return true;
}
//returns 0 if successful
//returns -1 if the client was not found
bool ClientPool::removeClient(std::string p_name)
{
	print_me("Removing client ("+p_name+")");

	std::map<Channel*, std::vector<Client> >::iterator itr;
	for(itr = m_channel_list.begin(); itr != m_channel_list.end(); itr++)
	{
		std::vector<Client>::iterator iitr;
		for(iitr = itr->second.begin(); iitr != itr->second.end(); iitr++)
		{
			if(iitr->getName() == p_name)
			{
				itr->second.erase(iitr);
				return true;
			}
		}
	}
	return false;
}
//returns 0 if successful
//returns -1 if the client was not found
bool ClientPool::removeClient(int p_socket)
{
	print_me("Removing client with socket:");
	printf("(%d)\n", p_socket);

	std::map<Channel*, std::vector<Client> >::iterator itr;
	for(itr = m_channel_list.begin(); itr != m_channel_list.end(); itr++)
	{
		std::vector<Client>::iterator iitr;
		for(iitr = itr->second.begin(); iitr != itr->second.end(); iitr++)
		{
			if(iitr->getSocket() == p_socket)
			{
				itr->second.erase(iitr);
				return true;
			}
		}
	}
	return false;
}
bool ClientPool::removeChannel(std::string p_name)
{
	print_me("Removing channel ("+p_name+")");

	std::vector<std::string> client_names;
	if(!getChannelClientNames(p_name, client_names))
	{
		log_this("Could not get clients from channel ("+p_name+")");
	}

	for(unsigned int i = 0; i < client_names.size(); i++)
	{
		std::string client_name = client_names.at(i);
		print_me("Channel: ("+p_name+") client: ("+client_name+")");
		int returned = switchClientChannels(client_name, DEFAULT_CHANNEL, "");
		if(returned == 0)
		{
			print_me("Move of client ("+client_name+") to ("+DEFAULT_CHANNEL+") successful");
		}
		else if(returned == -1)
		{
			log_this("Client ("+client_name+") not found");
		}
		else if(returned == -2)
		{
			log_this("eek. This should NOT happen");
		}
		else if(returned == -3)
		{
			log_this("eek. This should NOT happen");
		}
		else if(returned == -4)
		{
			log_this("Client ("+client_name+") could not be removed in switchClientChannels");
		}
		else
		{
			log_this("switchClientChannels returned an unexpected value!");
		}
	}

	std::map<Channel*, std::vector<Client> >::iterator itr;
	for(itr = m_channel_list.begin(); itr != m_channel_list.end(); itr++)
	{
		if(itr->first->getName() == p_name)
		{
			m_channel_list.erase(itr);
			return true;
		}
	}

	return false;
}

//return values:
//0 on success
//-1 client was not found
//-2 bad password
//-3 channel not found
//-4 client could not be removed
int ClientPool::switchClientChannels(std::string p_client_name, std::string p_channel_name, std::string p_password)
{
	print_me("Moving client ("+p_client_name+") to channel ("+p_channel_name+")");

	Client client;
	bool client_found = false;

	std::map<Channel*, std::vector<Client> >::iterator itr;
	for(itr = m_channel_list.begin(); itr != m_channel_list.end(); itr++)
	{
		std::vector<Client>::iterator iitr;
		for(iitr = itr->second.begin(); iitr != itr->second.end(); iitr++)
		{
			if(iitr->getName() == p_client_name)
			{
				client_found = true;
				client = (*iitr);
				break;
			}
		}
	}
	if(!client_found)
	{
		return -1;
	}

	bool channel_found = false;
	for(itr = m_channel_list.begin(); itr != m_channel_list.end(); itr++)
	{
		if(itr->first->getName() == p_channel_name)
		{
			if(itr->first->getPassword() != p_password)
			{
				return -2;
			}
			channel_found = true;
			break;
		}
	}
	if(channel_found)
	{
		if(!removeClient(p_client_name))
		{
			return -4;
		}
		itr->second.push_back(client);
	}
	else
	{
		return -3;
	}

	//success!
	return 0;
}


//returns true if successful
//returns false if the socket was not found
bool ClientPool::socketToName(int p_socket, std::string &p_name)
{
	print_me("Retrieving name from socket:");
	printf("(%d)\n", p_socket);

	std::map<Channel*, std::vector<Client> >::iterator itr;
	for(itr = m_channel_list.begin(); itr != m_channel_list.end(); itr++)
	{
		std::vector<Client> channel_clients = itr->second;
		for(unsigned int i = 0; i < channel_clients.size(); i++)
		{
			if(channel_clients.at(i).getSocket() == p_socket)
			{
				p_name = channel_clients.at(i).getName();
				return true;
			}
		}
	}
	return false;
}
//returns true if successful
//returns false if the name was not found
bool ClientPool::nameToSocket(std::string p_name, int &p_socket)
{
	print_me("Retrieving socket from name: ("+p_name+")");

	std::map<Channel*, std::vector<Client> >::iterator itr;
	for(itr = m_channel_list.begin(); itr != m_channel_list.end(); itr++)
	{
		std::vector<Client> channel_clients = itr->second;
		for(unsigned int i = 0; i < channel_clients.size(); i++)
		{
			if(channel_clients.at(i).getName() == p_name)
			{
				p_socket = channel_clients.at(i).getSocket();
				return true;
			}
		}
	}
	return false;
}
bool ClientPool::clientNameToChannelName(std::string p_client_name, std::string &p_channel_name)
{
	print_me("Retrieving channel name from client name: ("+p_client_name+")");

	std::map<Channel*, std::vector<Client> >::iterator itr;
	for(itr = m_channel_list.begin(); itr != m_channel_list.end(); itr++)
	{
		std::vector<Client> channel_clients = itr->second;
		for(unsigned int i = 0; i < channel_clients.size(); i++)
		{
			if(channel_clients.at(i).getName() == p_client_name)
			{
				p_channel_name = itr->first->getName();
				return true;
			}
		}
	}
	return false;
}


bool ClientPool::hasSocket(int p_socket)
{
	print_me("Checking if there is a client with socket:");
	printf("(%d)\n", p_socket);

	std::map<Channel*, std::vector<Client> >::iterator itr;
	for(itr = m_channel_list.begin(); itr != m_channel_list.end(); itr++)
	{
		std::vector<Client> channel_clients = itr->second;
		for(unsigned int i = 0; i < channel_clients.size(); i++)
		{
			if(channel_clients.at(i).getSocket() == p_socket)
			{
				return true;
			}
		}
	}
	return false;
}


bool ClientPool::getChannelClientNames(std::string p_channel_name, std::vector<std::string> &p_channel) const
{
	print_me("Getting clients of channel: ("+p_channel_name+")");

	std::vector<std::string> client_names;

	std::map<Channel*, std::vector<Client> >::const_iterator itr;
	for(itr = m_channel_list.begin(); itr != m_channel_list.end(); itr++)
	{
		if(itr->first->getName() == p_channel_name)
		{
			for(unsigned int i = 0; i < itr->second.size(); i++)
			{
				print_me("Got client ("+itr->second.at(i).getName()+") from channel ("+p_channel_name+")");
				client_names.push_back(itr->second.at(i).getName());
			}

			p_channel = client_names;
			return true;
		}
	}
	return false;
}
bool ClientPool::getChannelPassword(std::string p_name, std::string &p_password) const
{
	print_me("Getting password of channel ("+p_name+")");

	std::map<Channel*, std::vector<Client> >::const_iterator itr;
	for(itr = m_channel_list.begin(); itr != m_channel_list.end(); itr++)
	{
		if(itr->first->getName() == p_name)
		{
			p_password = itr->first->getPassword();
			return true;
		}
	}
	return false;
}
bool ClientPool::getClientPrivileges(std::string p_name, int &p_privileges) const
{
	print_me("Getting privileges of client: ("+p_name+")");

	std::map<Channel*, std::vector<Client> >::const_iterator itr;
	for(itr = m_channel_list.begin(); itr != m_channel_list.end(); itr++)
	{
		std::vector<Client> channel_clients = itr->second;
		for(unsigned int i = 0; i < channel_clients.size(); i++)
		{
			if(channel_clients.at(i).getName() == p_name)
			{
				p_privileges = channel_clients.at(i).getPrivileges();
				return true;
			}
		}
	}
	return false;
}


bool ClientPool::setChannelPassword(std::string p_name, std::string p_password)
{
	print_me("Changing password of channel ("+p_name+") to ("+p_password+")");

	std::map<Channel*, std::vector<Client> >::iterator itr;
	for(itr = m_channel_list.begin(); itr != m_channel_list.end(); itr++)
	{
		if(itr->first->getName() == p_name)
		{
//			int ip= (int)&((*itr).first);
//			Channel *p = (Channel*)ip;
//			p->setPassword(p_password);
			itr->first->setPassword(p_password);
			return true;
		}
	}
	return false;
}

