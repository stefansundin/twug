#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <vector>

#include "Client.h"

#include "../debug.h"

class Client;

class Channel
{
public:
	Channel();
	Channel(std::string p_name);
	~Channel();

	std::string getName();
	std::string getPassword();
	bool setName(std::string p_name);
	bool setPassword(std::string p_password);

	bool addClient(Client *p_client);
	bool removeClient(Client *p_client);

	bool addSubchannel(Channel *p_channel);
	bool removeSubchannel(Channel *p_channel);

	void print(int p_extra_tab);		//server debug thingy

private:
	std::string m_name;
	std::string m_password;

	std::vector<Client*> m_clients;
	std::vector<Channel*> m_subchannels;
};

#endif //CHANNEL_H_

