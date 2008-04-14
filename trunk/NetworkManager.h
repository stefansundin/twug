#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
#include <cstdio>
#include <iostream>
#include "UIEventsNetwork.h"

#include <vector>
#include <string>
#include <cstdlib>

#include "debug.h"
#include "network/ClientNetwork.h"
#include "network/ClientPool.h"

#include "DataKeeper.h"

class NetworkManager
{
public:
	NetworkManager(UIEventQueue* p_to_ui, UIEventQueue* p_to_network, DataKeeper* p_data);
	~NetworkManager();

	void run(); //main loop

	void processUIEvents();
	void processNetworkEvents();

private:
	int m_readfd;
	int m_socket;
	UIEventsNetwork* m_events;
	bool m_connectedandorloggedin;
	bool m_talkbutton;
	UIEventQueue* m_to_network;

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

	DataKeeper* m_data;
};
