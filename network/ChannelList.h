#ifndef CHANNELLIST_H_
#define CHANNELLIST_H_

#include <map>
#include <vector>

#include "Client.h"
#include "Channel.h"

#define LOBBY "__lobby__"

class ChannelList
{
public:
	ChannelList();
	~ChannelList();

	bool newChannel(std::string p_name);
	bool removeChannel(std::string p_name);

	bool newClient(int p_socket, std::string p_name);
	bool removeClient(std::string p_name);
	bool removeClient(int p_socket);

	int changeClientChannel(std::string p_client_name, std::string p_channel_name, std::string p_password);
	int changeClientChannel(int p_socket, std::string p_channel_name, std::string p_password);

	void print();		//server debug thingy

private:
	std::map<int, Client*> m_clients;
	std::vector<Channel*> m_channels;
};

#endif //CHANNELLIST_H_

