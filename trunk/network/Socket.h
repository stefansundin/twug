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
public:
	Socket(int domain, int type);
	Socket(int file_descriptor);
	~Socket();

	int connect(std::string address, int port);
	int bind(std::string, int port);
	int listen(int backlog);
	Socket accept();

	int recv(std::string &data, int max_recive);
	int send(std::string data);

	int shutdown(int how);
	int close();
private:
	struct sockaddr_in addr;
	int sock;
};
