#include "ClientPool.h"

ClientPool::ClientPool()
{
}
ClientPool::~ClientPool()
{
}


std::vector<std::string> ClientPool::getClientNames()
{
	std::vector<std::string> client_names;
	int i;
	for(i = 0; i < m_clients.size(); i++)
	{
		client_names.push_back(m_clients.at(i).getName());
	}

	return client_names;
}


bool ClientPool::addClient(std::string p_name, std::string p_channel_name, int p_socket)
{
	int i;
	for(i = 0; i < m_clients.size(); i++)
	{
		if(m_clients.at(i).getName() == p_name)
		{
			return false;
		}
		if(m_clients.at(i).getSocket() == p_socket)
		{
			return false;
		}
	}

	m_clients.push_back(client(p_name, p_channel_name, p_socket));
	return true;
}
bool ClientPool::removeClient(int p_socket)
{
	std::vector<client>::iterator citr;
	for(citr = m_clients.begin(); citr != m_clients.end(); citr++)
	{
		if(citr->getSocket() == p_socket)
		{
			m_clients.erase(citr);
			return true;
		}
	}
	return false;
}


bool ClientPool::socketToName(int p_socket, std::string *p_name)
{
	int i;
	for(i = 0; i < m_clients.size(); i++)
	{
		if(m_clients.at(i).getSocket() == p_socket)
		{
			*p_name = m_clients.at(i).getName();
			return true;
		}
	}
	return false;
}
bool ClientPool::nameToSocket(std::string p_name, int *p_socket)
{
	int i;
	for(i = 0; i < m_clients.size(); i++)
	{
		if(m_clients.at(i).getName() == p_name)
		{
			*p_socket = m_clients.at(i).getSocket();
			return true;
		}
	}
	return false;
}
bool ClientPool::clientNameToChannelName(std::string p_client_name, std::string *p_channel_name)
{
	std::vector<std::string> channel_names = getChannelNames();
	std::vector<std::string> client_names;
	int i, j;
	for(i = 0; i < channel_names.size(); i++)
	{
		if(!getChannelClientNames(channel_names.at(i), &client_names))
		{
			log_this("couldn't get channel client names");
			return false;
		}
		for(j = 0; j < client_names.size(); j++)
		{
			if(client_names.at(j) == p_client_name)
			{
				*p_channel_name = channel_names.at(i);
				return true;
			}
		}
	}
	return false;
}


bool ClientPool::hasSocket(int p_socket)
{
	int i;
	for(i = 0; i < m_clients.size(); i++)
	{
		if(m_clients.at(i).getSocket() == p_socket)
		{
			return true;
		}
	}
	return false;
}


bool ClientPool::getChannelClients(std::string p_channel_name, std::vector<std::string> *p_channel)
{
	bool return_value = false;

	std::vector<std::string> channel;

	int i;
	for(i = 0; i < m_clients.size(); i++)
	{
		if(m_clients.at(i).getChannelName() == p_channel_name)
		{
			channel.push_back(m_clients.at(i).getName());
			return_value = true;
		}
	}

	*p_channel = channel;
	return return_value;
}
std::vector<std::string> ClientPool::getChannelNames()
{
	std::vector<std::string> channel_names;

	int i, j;
	for(i = 0; i < m_clients.size(); i++)
	{
		bool channel_name_exists = false;
		for(j = 0; j < channel_names.size(); j++)
		{
			if(m_clients.at(i).getChannelName() == channel_names.at(j))
				channel_name_exists = true;
		}
		if(!channel_name_exists)
			channel_names.push_back(m_clients.at(i).getChannelName());
	}

	return channel_names;
}
bool ClientPool::getChannelClientNames(std::string p_channel_name, std::vector<std::string> *p_client_names)
{
	bool clients_found = false;
	std::vector<std::string> client_names;

	int i, j;
	for(i = 0; i < m_clients.size(); i++)
	{
		if(m_clients.at(i).getChannelName() == p_channel_name)
		{
			client_names.push_back(m_clients.at(i).getName());
			clients_found = true;
		}
	}

	*p_client_names = client_names;
	return clients_found;
}

