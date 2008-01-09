#ifndef NETWORK_H_
#define NETWORK_H_

#include <string>

#include "header.h"
#include "message_types.h"
#include "Data.h"
#include "Socket.h"

#define NETWORK_VERSION = 0

class Network
{
public:
	Network();
	virtual ~Network();

	int connect(std::string p_address, int p_port);
	int disconnect();

	int sendData(Data p_data);
	int getData(Data &p_data);
protected:
	Socket m_socket;
};

#endif /*NETWORK_H_*/
