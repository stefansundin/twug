#ifndef CLIENTNETWORK_H_
#define CLIENTNETWORK_H_

#include "Network.h"

class ClientNetwork : public Network
{
public:
	ClientNetwork();
	~ClientNetwork();

	int connect(std::string p_address, int p_port);
	int disconnect();
	void loginRequest(std::string p_user, std::string p_password);
	void logout();

	bool processNetworking();

	int m_socket;		//should be private
private:
	bool m_connected;
};

#endif /*CLIENTNETWORK_H_*/
