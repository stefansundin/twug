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

int main (int argc, char *argv[])
{
	Gtk::Main kit(argc, argv);

	Handler* handler = new Handler(&cb0,&cb1);

	g_app = new AppMan(handler);

	kit.run();

	return 0;
}

