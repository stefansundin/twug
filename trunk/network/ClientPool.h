#ifndef ClientPool_h
#define ClientPool_h

#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <iostream>

#include "../debug.h"

#include "Client.h"
#include "Channel.h"

class ClientPool
{
public:
	ClientPool();
	~ClientPool();

	std::vector<std::string> getClientNames() const;
	std::vector<std::string> getChannelNames() const;

	int addClient(std::string p_name, std::string p_channel_name, int p_socket);
	bool addChannel(std::string p_name, std::string p_password);
	bool removeClient(std::string p_name);
	bool removeClient(int p_socket);

	bool socketToName(int p_socket, std::string &p_string);
	bool nameToSocket(std::string p_name, int &p_socket);
	bool clientNameToChannelName(std::string p_client_name, std::string &p_channel_name);

	bool hasSocket(int p_socket);

	bool getChannelClients(std::string p_channel_name, std::vector<std::string> &p_channel) const;
	bool getChannelClientNames(std::string p_channel_name, std::vector<std::string> &p_client_names) const;
	bool getChannelPassword(std::string p_name, std::string &p_password) const;
	bool getClientPrivileges(std::string p_name, int &p_privileges) const;

	bool setChannelPassword(std::string p_name, std::string p_password);

private:
	std::map<Channel*, std::vector<Client> > m_channel_list;
};

#endif //ClientPool_h

