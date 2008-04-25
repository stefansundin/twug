#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "Handler.h"

Handler *g_handler;
int g_socket;

void cb_handle_text(std::string sender, std::string msg)
{
	printf("%s: %s\n", sender.c_str(), msg.c_str());
}
void cb_error_connecting(std::string err)
{
	printf("error connecting: (%s)\n", err.c_str());
}
void cb_connected_to_server(std::string ip, std::string name)
{
	printf("connected as (%s) on (%s)\n", name.c_str(), ip.c_str());
}
void cb_connection_lost(std::string err)
{
	printf("connection lost: (%s)\n", err.c_str());
}
void cb_channel_list_changed()
{
	printf("channel list changed\n");
}
int cb_read_from_socket(char* buf, unsigned int count)
{
	return 0;
}
int cb_write_to_socket(const char* buf, unsigned int count)
{
	return 0;
}
void cb_setupsocket()
{
	printf("setting up socket\n");
	g_socket = g_handler->getSocket();

	while(true)
	{
		if(g_socket != 0)
		{
			struct timeval tv;

			fd_set read;
			FD_ZERO(&read);
			FD_SET(g_socket, &read);

			tv.tv_sec = 5;
			tv.tv_usec = 0;
			int select_returned = select(g_socket+1, &read, NULL, NULL, &tv);
			if(select_returned == -1)
			{
				report_error(strerror(errno));
			}

			if(FD_ISSET(g_socket, &read))
			{
				g_handler->update();
			}
		}
	}

}

int main()
{
	g_socket = 0;
	g_handler = new Handler(
		&cb_handle_text,
		&cb_error_connecting,
		&cb_connected_to_server,
		&cb_connection_lost,
		&cb_channel_list_changed,
		&cb_read_from_socket,
		&cb_write_to_socket,
		&cb_setupsocket);

	g_handler->connectToServer("127.0.0.1:6789", "average_joe", "kaka");

	return 0;
}
