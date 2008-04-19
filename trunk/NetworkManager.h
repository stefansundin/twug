#ifndef NetworkManager_h
#define NetworkManager_h

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
#include <cstdio>
#include <iostream>
#include "UIEventQueueHolder.h"

#include <vector>
#include <string>
#include <cstdlib>

#include "debug.h"
#include "network/ClientNetwork.h"
#include "network/ClientPool.h"

#include "DataKeeper.h"

class EventsToUI : public UIEventQueueHolder
{
public:
	EventsToUI(UIEventQueue* p_eventqueue) : UIEventQueueHolder(p_eventqueue)
	{
		m_fd = m_eventqueue->getWriteFd();
	}
	void pushEvent(UIEvent p_event)
	{
		m_eventqueue->pushEvent(p_event);
		
		char kaka = '\n';
		write(m_fd, (void*)&kaka, 1);
	}
private:
	int m_fd;
};

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
	EventsToUI* m_events;
	UIEventQueue* m_to_network;

	std::string m_connected_to;
	std::string m_last_requested_nick;
	std::string m_last_requested_server;

	ClientPool m_client_pool;
	ClientNetwork m_client_network;

	DataKeeper* m_data;
};

#endif //NetworkManager_h
