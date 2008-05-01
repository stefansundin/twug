#include "ServerSettings.h"


ServerSettings::ServerSettings()
{
	std::ifstream handle;

	handle.open("server.cfg", std::ifstream::in);	

	if(!handle.fail())
	{
		std::list<std::string> list;
		char str[60];

		while(!handle.eof())
		{
		      handle.getline(str,2000);
		      list.push_back( str );
		}


		list.pop_back();

		if (list.size() >= 1)
		{
			std::list<std::string>::iterator itr;
		
			for(itr = list.begin() ;itr != list.end(); itr++)
			{
				m_channels.push_back(*itr);		
			}
		}
	}
	handle.close();
}

void ServerSettings::saveSettings()
{
	std::ofstream handle;
        handle.open("server.cfg", std::ios::out);           
	for (int i=0;i<m_channels.size();i++)
	{
		if (m_channels.at(i) != "__lobby__")
		{
			handle << m_channels.at(i) << "\n";
		}
	}       
        handle.close();               
}


std::vector<std::string> ServerSettings::getChannelList()
{
	return m_channels;
}

void ServerSettings::setChannelList(std::vector<std::string> p_channels)
{
	m_channels = p_channels;
	saveSettings();
}
