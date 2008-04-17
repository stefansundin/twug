#ifndef NetworkManager_h
#define NetworkManager_h

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
	void handleNetworkMessage(Message p_message);
	void channelListChanged();
	void joinChannel(std::string p_channel_name);
	void connectToServer(std::string p_address, std::string p_username, std::string p_password);

	void disconnect();

	int m_readfd;
	int m_socket;
	UIEventsNetwork* m_events;
	bool m_talk_button;
	UIEventQueue* m_to_network;

	std::string m_connected_to;
	std::string m_last_requested_nick;
	std::string m_last_requested_server;

	ClientPool m_client_pool;
	ClientNetwork m_client_network;

	DataKeeper* m_data;
};

#endif //NetworkManager_h

