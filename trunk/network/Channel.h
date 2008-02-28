#include <vector>

#include "Client.h"

#include "../debug.h"

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
	Client* getClient(int p_socket);
	bool removeClient(int p_socket);

	bool newSubchannel(std::string p_name);
	bool addSubchannel(Channel *p_channel);
	bool removeSubchannel(Channel *p_channel);
	Channel* getSubchannel(std::string p_name);

	std::vector<Client*> getClients();

	void print(int p_layer);		//server debug thingy

private:
	std::string m_name;
	std::string m_password;

	std::vector<Client*> m_clients;
	std::vector<Channel*> m_subchannels;
};

