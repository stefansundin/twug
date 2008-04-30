#ifndef CLIENTSETTINGS_H
#define CLIENTSETTINGS_H

#include <string>
#include <fstream>
#include <list>
#include <vector>
#include <iostream>

class ClientSettings
{
private:
	std::vector<std::string> m_servers;
	std::string m_nick;	

	void saveSettings();
public:
	ClientSettings();

	std::vector<std::string> getServerList();
	void setServerList(std::vector<std::string> p_servers);

	std::string getNick();
	void setNick(std::string p_nick);
};

#endif
