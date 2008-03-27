#include <gtkmm/main.h>
#include "AppMan.h"

AppMan* g_app;

void cb_handle_text(std::string sender, std::string msg)
{
	g_app->m_window->m_msghandler->handleMessage(sender,msg);
}

void cb_error_connecting(std::string err)
{
	g_app->m_window->connectionError(0,err);
}

void cb_connected_to_server(std::string ip, std::string name)
{
	g_app->m_window->connectedAs(ip,name);
}

void cb_connection_lost(std::string err)
{
	g_app->m_window->connectionError(1,err);
}

void cb_channel_list_changed()
{
	g_app->m_window->reloadChannels();
}

int main (int argc, char *argv[])
{
	Gtk::Main kit(argc, argv);

	Handler* handler = new Handler(
		&cb_handle_text,
		&cb_error_connecting,
		&cb_connected_to_server,
		&cb_connection_lost,
		&cb_channel_list_changed   );

	g_app = new AppMan(handler);

	kit.run();

	return 0;
}

