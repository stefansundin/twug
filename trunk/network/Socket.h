#include <string>

#ifdef __linux__
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

#if defined(_MSV_VER)
#endif

class Socket
{
friend select;

public:
	Socket();
	Socket(int domain, int type);
	Socket(int file_descriptor);
	~Socket();

	int connect(std::string address, unsigned int port);
	int bind(std::string, unsigned int port);
	int listen(int backlog);
	Socket accept();

	int recv(void *p_buffer, unsigned int p_max_recive);
	int send(const void *p_buffer, unsigned int p_length);

	int shutdown(int how);
	int close();
private:
	struct sockaddr_in addr;
	int sock;
};

