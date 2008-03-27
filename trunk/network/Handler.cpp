#include "Handler.h"


Handler::Handler(
	void (*p_cb0)(std::string,std::string),
	void (*p_cb1)(std::string),
	void (*p_cb_connected_to_server)(std::string, std::string),
	void (*p_cb_connection_lost)(std::string),
	void (*p_cb_channel_list_changed)()
	)
{
	m_cb0 = p_cb0;
	m_cb1 = p_cb1;
	m_cb_connected_to_server	=	p_cb_connected_to_server;
	m_cb_connection_lost		=	p_cb_connection_lost;
	m_cb_channel_list_changed	=	p_cb_channel_list_changed;

	m_client_pool.addClient("Basse", "__lobby__", 0);
	m_client_pool.addClient("Loladin", "__lobby__", 0);

	m_client_pool.addClient("Recover", "channel 1", 0);
	m_client_pool.addClient("Toaefge", "channel 1", 0);

	std::cout << "Handler: constructed" << std::endl;
}

Handler::~Handler()
{
}

std::vector<std::string> Handler::getChannelMembers(std::string p_channel_name)
{
	std::vector<std::string> client_names;
	m_client_pool.getChannelClientNames(p_channel_name, &client_names);
	return client_names;
}

std::vector<std::string> Handler::getChannels()
{
	return m_client_pool.getChannelNames();
}

void Handler::joinChannel(std::string channel_name)
{
	std::cout << "Handler: Trying to join channel \"" << channel_name << "\"\n"; 

	m_mychannel = channel_name;
	m_cb_channel_list_changed(); // channel list changed (callback)
}

void Handler::connectToServer(std::string p_address, unsigned int p_port, std::string p_username, std::string p_password)
{
	printf("Handler::connectToServer() connecting to %s:%d as %s with password %s\n", p_address.c_str(), p_port, p_username.c_str(), p_password.c_str());

	p_client_network.connect(p_address, p_port);
	p_client_network.loginRequest(p_username, p_password);

	m_mynick = p_username;

	m_cb_connected_to_server(p_address, p_username);		//add p_port to this maybe?
}


void Handler::disconnect()
{
	m_client_network.logout();
	m_client_network.disconnect();

	std::cout << "Handler: disconnected\n";
}


void Handler::iStartTalking()
{
	std::cout << "Handler: Push-to-talk now active\n";
}

void Handler::iStopTalking()
{
	std::cout << "Handler: Push-to-talk now inactive\n";
}


void Handler::postMessage(std::string destination, std::string contents)
{
	std::cout << "Handler: Message \"" << contents << "\" posted to user named \"" << destination << "\"\n";

	m_cb0(destination,"(Bounce back!) " + contents); // give message (callback)	
}

int Handler::getSocket()
{
	return 	m_client_network.getSocket();
}

void Handler::update()
{
	m_client_network.processNetworking();
}

