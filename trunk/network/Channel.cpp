#include "Channel.h"

Channel::Channel()
{
	m_name = "no_name_channel";
}
Channel::Channel(std::string p_name)
{
	m_name = p_name;
}
Channel::~Channel()
{
}

std::string Channel::getName()
{
	return m_name;
}
std::string Channel::getPassword()
{
	return m_password;
}
bool Channel::setName(std::string p_name)
{
	if(p_name.size() <= 20)
	{
		m_name = p_name;
		return true;
	}
	else
		return false;
}
bool Channel::setPassword(std::string p_password)
{
	if(p_password.size() <= 20)
	{
		m_password = p_password;
		return true;
	}
	else
		return false;
}

bool Channel::addClient(Client *p_client)
{
	std::vector<Client*>::iterator citr;
	for(citr = m_clients.begin(); citr != m_clients.end(); citr++)
	{
		if((*citr)->getUsername() == p_client->getUsername())	
		{
			return false;
		}
	}
	m_clients.push_back(p_client);
	return true;
}
bool Channel::removeClient(Client *p_client)
{
	std::vector<Client*>::iterator citr;
	for(citr = m_clients.begin(); citr != m_clients.end(); citr++)
	{
		if((*citr)->getUsername() == p_client->getUsername())	
		{
			m_clients.erase(citr);
			return true;
		}
	}
	return false;
}
Client* Channel::getClient(int p_socket)
{
	int ch, cl;
	Client *c;
	for(cl = 0; cl < m_clients.size(); cl++)
	{
		if(m_clients.at(cl)->getSocket() == p_socket)
			return m_clients.at(cl);
	}
	for(ch = 0; ch < m_subchannels.size(); ch++)
	{
		c = m_subchannels.at(ch)->getClient(p_socket);
		if(c != NULL)
			return c;
	}
	return NULL;	//no client found
}
bool Channel::removeClient(int p_socket)
{
	int ch;
	Client *c;
	std::vector<Client*>::iterator clitr;
	for(clitr = m_clients.begin(); clitr != m_clients.end(); clitr++)
	{
		if((*clitr)->getSocket() == p_socket)
		{
			m_clients.erase(clitr);
			return true;
		}
	}
	for(ch = 0; ch < m_subchannels.size(); ch++)
	{
		if(m_subchannels.at(ch)->removeClient(p_socket))
			return true;
	}
	return false;	//no client removed
}


bool Channel::newSubchannel(std::string p_name)
{
	std::vector<Channel*>::iterator citr;
	for(citr = m_subchannels.begin(); citr != m_subchannels.end(); citr++)
	{
		if((*citr)->getName() == p_name)
		{
			return false;
		}
	}

	m_subchannels.push_back(new Channel(p_name));
	return true;
}
bool Channel::addSubchannel(Channel *p_channel)
{
	for(int i = 0; i < m_subchannels.size(); i++)
	{
		if(m_subchannels.at(i)->getName() == p_channel->getName())
			return false;
	}
	m_subchannels.push_back(p_channel);
	return true;
}
bool Channel::removeSubchannel(Channel *p_channel)
{
	std::vector<Channel*>::iterator sitr;
	for(sitr = m_subchannels.begin(); sitr != m_subchannels.end(); sitr++)
	{
		if((*sitr)->getName() == p_channel->getName())	
		{
			m_subchannels.erase(sitr);
			return true;
		}
	}
	return false;
}

Channel* Channel::getSubchannel(std::string p_name)
{
	std::vector<Channel*>::iterator sitr;
	for(sitr = m_subchannels.begin(); sitr != m_subchannels.end(); sitr++)
	{
		if((*sitr)->getName() == p_name)	
		{
			return *sitr;
		}
	}
	return NULL;
}


std::vector<Client*> Channel::getClients()
{
	return m_clients;
}


void Channel::print(int p_layer)		//server debug thingy
{
	for(int i = 0; i < p_layer; i++)
		printf("\t");
	printf("%s\n", m_name.c_str());

	int ch, cl;
	for(cl = 0; cl < m_clients.size(); cl++)
	{
		for(int i = 0; i <= p_layer; i++)
			printf("\t");
		printf("%s\n", m_clients.at(cl)->getUsername().c_str());
	}
	for(ch = 0; ch < m_subchannels.size(); ch++)
	{
		m_subchannels.at(ch)->print(p_layer+1);
	}
}

