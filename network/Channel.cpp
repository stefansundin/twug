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
			delete *sitr;
			m_subchannels.erase(sitr);
			return true;
		}
	}
	return false;
}


void Channel::print(int p_extra_tab)		//server debug thingy
{
	for(int i = 0; i < p_extra_tab; i++)
		printf("\t");
	printf("%s\n", m_name.c_str());

	int ch, cl;
	for(cl = 0; cl < m_clients.size(); cl++)
	{
		for(int i = 0; i <= p_extra_tab; i++)
			printf("\t");
		printf("%s\n", m_clients.at(cl)->getUsername().c_str());
	}
	for(ch = 0; ch < m_subchannels.size(); ch++)
	{
		m_subchannels.at(ch)->print(p_extra_tab+1);
	}
}

