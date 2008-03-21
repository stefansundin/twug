#include <gtkmm/main.h>
#include "AppMan.h"

AppMan* app;

void cb0(std::string sender, std::string msg)
{
	app->m_window->m_msghandler->handleMessage(sender,msg);
}

void cb1(std::string channel)
{
	std::cout << "New Channel membership: " << channel << std::endl;
}

int main (int argc, char *argv[])
{
	Gtk::Main kit(argc, argv);

	Handler* handler = new Handler(&cb0,&cb1);

	app = new AppMan(handler);

	kit.run();

	return 0;
}

