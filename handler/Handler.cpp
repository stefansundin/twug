#include "Handler.h"


Handler::Handler(
	void (*p_cb0)(std::string,std::string),
	void (*p_cb1)(std::string),
	void (*p_cb2)(std::string,std::string),
	void (*p_cb3)(std::string)
	)
{
	m_cb0 = p_cb0;
	m_cb1 = p_cb1;
	m_cb2 = p_cb2;
	m_cb3 = p_cb3;

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

void Handler::joinChannel(std::string channel_name)
{
	std::cout << "Handler: Trying to join channel \"" << channel_name << "\"\n"; 

	m_cb1(channel_name);
}

void Handler::connectToServer(std::string p_ip, std::string p_name)
{
	std::cout << "Handler: attempting to connect to server \"" << p_ip << "\"\n"; 

	if (p_ip=="192.168.0.1")
	{
		std::cout << "Handler: connection failed\n";
		m_cb3(p_ip);
	} else {
		std::cout << "Handler: connected to server \"" << p_ip << "\"\n";
		m_cb2(p_ip,p_name);
	}
}

void Handler::disconnect()
{
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

	m_cb0(destination,"(Bounce back!) " + contents); 	
}

int Handler::getSocket()
{
	return 0; //temp	
}

void Handler::update()
{
	std::cout << "lol\n";	
}
