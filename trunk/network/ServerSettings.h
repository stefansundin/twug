#ifndef SERVERSETTINGS_H
#define SERVERSETTINGS_H

#include <string>
#include <fstream>
#include <list>
#include <vector>
#include <iostream>

class ServerSettings
{
private:
	std::vector<std::string> m_channels;	

	void saveSettings();
public:
	ServerSettings();

	std::vector<std::string> getChannelList();
	void setChannelList(std::vector<std::string> p_channels);
};

#endif
