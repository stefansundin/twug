#ifndef NETWORK_H_
#define NETWORK_H_

#include <string>
#include <queue>

#include "header.h"
#include "message_types.h"
#include "fill.h"
#include "strip.h"
#include "Data.h"
#include "Socket.h"

#define NETWORK_VERSION 0

class Network
{
public:
	Network();
	virtual ~Network();

	int connect(std::string p_address, int p_port);
	int disconnect();

	void loginRequest(std::string p_user, std::string p_password);
	void logout();

	bool getMessage(Data &p_data);
	void update();

protected:
	int sendData(Data &p_data);
	int getData(Data &p_data);

	Socket m_socket;
	bool m_connected;

	std::queue<Data> m_message_buffer;
	char *m_buffer;
	unsigned int m_buffer_length;
};

#endif /*NETWORK_H_*/

