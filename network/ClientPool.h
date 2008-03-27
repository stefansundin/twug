#ifndef ClientPool_h
#define ClientPool_h

#include <string>
#include <vector>

#include "../debug.h"

class client
{
public:
	client(std::string p_name, std::string p_channel_name, int p_socket)
	{
		m_name = p_name;
		m_channel_name = p_channel_name;
		m_socket = p_socket;
	}
	~client()
	{
	}

	std::string getName()
	{
		return m_name;
	}
	std::string getChannelName()
	{
		return m_channel_name;
	}
	int getSocket()
	{
		return m_socket;
	}

private:
	std::string m_name;
	std::string m_channel_name;
	int m_socket;
};

class ClientPool
{
public:
	ClientPool();
	~ClientPool();

	std::vector<std::string> getClientNames();

	bool addClient(std::string p_name, std::string p_channel_name, int p_socket);
	bool removeClient(std::string p_name);
	bool removeClient(int p_socket);

	bool socketToName(int p_socket, std::string *p_string);
	bool nameToSocket(std::string p_name, int *p_socket);
	bool clientNameToChannelName(std::string p_client_name, std::string *p_channel_name);

	bool hasSocket(int p_socket);

	bool getChannelClients(std::string p_channel_name, std::vector<std::string> *p_channel);
	std::vector<std::string> getChannelNames();
	bool getChannelClientNames(std::string p_channel_name, std::vector<std::string> *p_client_names);

private:
	std::vector<client> m_clients;
};

#endif //ClientPool_h

