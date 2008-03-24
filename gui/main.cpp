#include <gtkmm/main.h>
#include "AppMan.h"

AppMan* g_app;

void cb0(std::string sender, std::string msg)
{
	g_app->m_window->m_msghandler->handleMessage(sender,msg);
}

void cb1(std::string channel)
{
	std::cout << "New Channel membership: " << channel << std::endl;
}

void cb2(std::string ip, std::string name)
{
	g_app->m_window->connectedAs(ip,name);
}

void cb3(std::string ip)
{
	g_app->m_window->connectionLost(ip);
}

int main (int argc, char *argv[])
{
	Gtk::Main kit(argc, argv);

	Handler* handler = new Handler(&cb0,&cb1,&cb2,&cb3);

	g_app = new AppMan(handler);

	kit.run();

	return 0;
}

