#ifndef CLIENTNETWORK_H_
#define CLIENTNETWORK_H_

#include "Network.h"

class ClientNetwork : public Network
{
public:
	ClientNetwork();
	~ClientNetwork();

	//sendable messages
	int connect(std::string p_address, int p_port);
	int disconnect();
	void loginRequest(std::string p_user, std::string p_password);
	void logout();
	void sendText(std::string p_to_username, std::string p_message);

	bool processNetworking();

	int getSocket();		//this is usefull when one needs tell GTK to watch the socket

private:
	int m_socket;
	bool m_connected;
};

#endif /*CLIENTNETWORK_H_*/
