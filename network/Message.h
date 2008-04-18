#ifndef Message_h
#define Message_h

#include "Data.h"

class Message
{
public:
	Message();
	~Message();

	int getSocket();
	Data getData();

	void setSocket(int p_socket);
	void setData(Data p_data);

private:
	int m_socket;
	Data m_data;
};

#endif //Message_h

