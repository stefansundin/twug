#ifndef SERVERNETWORK_H_
#define SERVERNETWORK_H_

#include <string>

#ifdef __linux__
#include <sys/select.h>
#endif

#include "Network.h"

typedef void (*disconnect_callback_t)(int);

class ServerNetwork : public Network
{
public:
	ServerNetwork();
	~ServerNetwork();

	bool initSocket(std::string p_bind_address, unsigned int p_bind_port);
	void processNetworking();
	void disconnectClient(int p_socket);
	void setDisconnectClientCallback(disconnect_callback_t p_callback);

private:
	int m_accepting_socket;
	disconnect_callback_t m_disconnect_callback;
};

#endif /*SERVERNETWORK_H_*/
