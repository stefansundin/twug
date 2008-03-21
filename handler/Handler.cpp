#include "Handler.h"
#include <ctime>


Handler::Handler()
{
	m_connectedTo = "0";

	channel kaka;
	kaka.name = "__lobby__";
	kaka.members.push_back("Basse");
	kaka.members.push_back("Loladin");
	m_channels.push_back(kaka);

	kaka.members.clear();
	kaka.name = "channel 1";
	kaka.members.push_back("Recover");
	kaka.members.push_back("Toaefge");
	m_channels.push_back(kaka);

	std::cout << "Handler: constructed" << std::endl;
}

Handler::~Handler()
{
}

std::vector<std::string> Handler::getChannelMembers(std::string channel_name)
{
	for(int i=0;i<m_channels.size();i++)
	{
		if(m_channels.at(i).name == channel_name)
		{
			return m_channels.at(i).members;
		}
	}
}

std::vector<std::string> Handler::getChannels()
{;
	std::vector<std::string> temp;
	
	for(int i=0;i<m_channels.size();i++)
		temp.push_back(m_channels.at(i).name);
	
	return temp;	
}

bool Handler::joinChannel(std::string channel_name)
{
	
}

bool Handler::connectToServer(std::string p_ip)
{
	if(m_connectedTo!=p_ip)
	{
		if(m_connectedTo!="0")
			disconnect();
		
		if (p_ip=="0")
			return true;

		std::cout << "Handler: connecting to server \"" << p_ip << "\"\n"; 

		sleep(1);
		if (p_ip=="192.168.0.1")
		{
			std::cout << "Handler: connection failed\n";
			return false;
		} else {
			std::cout << "Handler: connected to server \"" << p_ip << "\"\n";
			m_connectedTo = p_ip; 
			return true;
		}
	} else {
		std::cout << "Handler: already connected to " << p_ip << ", doing nothing\n";
		return true;
	}
}

void Handler::disconnect()
{
		m_connectedTo = "0";
		std::cout << "Handler: disconnected\n";
}


void Handler::iStartTalking()
{
	if (m_connectedTo!="0")
		std::cout << "Handler: Push-to-talk now active\n";
}

void Handler::iStopTalking()
{
	if (m_connectedTo!="0")
		std::cout << "Handler: Push-to-talk now inactive\n";
}


void Handler::postMessage(std::string destination, std::string contents)
{
	std::cout << "Handler: Message \"" << contents << "\" posted to user named \"" << destination << "\"\n"; 	
}

int Handler::getSocket()
{
	return 0; //temp	
}

void Handler::update()
{
	std::cout << "lol\n";	
}
