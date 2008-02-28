#include <map>
#include <vector>

#include "Channel.h"

class ChannelList
{
public:
	ChannelList();
	~ChannelList();

	bool newChannel(std::string p_name);
	bool removeChannel(std::string p_name);

	bool getChannel(std::string p_name, Channel *&p_channel);

	void printChannels();		//server debug thingy

private:
	std::map<int, Client*> m_clients;
	std::vector<Channel*> m_channels;
};

