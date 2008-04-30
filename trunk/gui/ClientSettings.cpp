#include "ClientSettings.h"


ClientSettings::ClientSettings()
{
	std::ifstream handle;

	handle.open("twug.cfg", std::ifstream::in);	

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

		if (!list.empty())
		{
			std::list<std::string>::iterator itr = list.begin();
		
			m_nick = (*itr);

			if (list.size() >= 2)
			{
				for(itr++;itr != list.end(); itr++)
				{
					m_servers.push_back(*itr);		
				}
			}
		}
	}
	handle.close();
}

void ClientSettings::saveSettings()
{
	std::ofstream handle;
        handle.open("twug.cfg", std::ios::out);           
        handle << m_nick << "\n";
	for (int i=0;i<m_servers.size();i++)
	{
		handle << m_servers.at(i) << "\n";
	}       
        handle.close();               
}


std::vector<std::string> ClientSettings::getServerList()
{
	return m_servers;
}

void ClientSettings::setServerList(std::vector<std::string> p_servers)
{
	m_servers = p_servers;
	saveSettings();
}

std::string ClientSettings::getNick()
{
	return m_nick;
}

void ClientSettings::setNick(std::string p_nick)
{
	m_nick = p_nick;
	saveSettings();
}

