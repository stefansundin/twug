#include "ChannelList.h"

ChannelList::ChannelList()
{
}
ChannelList::~ChannelList()
{
}


bool ChannelList::newChannel(std::string p_name)
	{
	for(int i = 0; i < m_channels.size(); i++)
	{
		if(m_channels.at(i)->getName() == p_name)
			return false;
	}
	Channel *c = new Channel(p_name);
	printf("c = \"%d\"\n", (unsigned int)c);
	m_channels.push_back(c);
	return true;
}
bool ChannelList::removeChannel(std::string p_name)
{
	std::vector<Channel*>::iterator citr;
	for(citr = m_channels.begin(); citr != m_channels.end(); citr++)
	{
		if((*citr)->getName() == p_name)
		{
			m_channels.erase(citr);
			return true;
		}
	}
	return false;
}


bool ChannelList::getChannel(std::string p_name, Channel *&p_channel)
{
	for(int i = 0; i < m_channels.size(); i++)
	{
		if(m_channels.at(i)->getName() == p_name)
		{
			p_channel = m_channels.at(i);
			return true;
		}
	}
	return false;
}


void ChannelList::printChannels()		//server debug thingy
{
	printf("Channels:\n");

	int ch, cl;
	for(cl = 0; cl < m_clients.size(); cl++)
	{
		printf("\t%s\n", m_clients.at(cl)->getUsername().c_str());
	}
	for(ch = 0; ch < m_channels.size(); ch++)
	{
		m_channels.at(ch)->print();
	}
}





