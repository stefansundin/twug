#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
#include <cstdio>
#include <iostream>
#include "UIEvents.h"

#include <vector>
#include <string>
#include <cstdlib>

#include "network/ClientNetwork.h"
#include "network/ClientPool.h"

class NetworkManager{
private:
	int m_readfd;
	int m_socket;
	UIEvents* m_events;
	bool m_connectedandorloggedin;
	bool m_talkbutton;

	std::string m_connectedTo;
	std::string m_lastrequestednick;
	std::string m_lastrequestedserver;

	void handleNetworkMessage(Message p_message);
	void channelListChanged();
	void joinChannel(std::string p_channel_name);
	void connectToServer(std::string p_address, std::string p_username, std::string p_password);

	void disconnect();

	ClientPool m_client_pool;
	ClientNetwork m_client_network;
public:
	NetworkManager(UIEvents *p_events);

	void processUIEvents();
	void processNetworkEvents();
};
