#include "AppMan.h"

void AppMan::setupSocket()
{
	int fd = m_handler->getSocket();
	
	std::cout << "AppMan: Setting up socket " << fd << " as new event source\n";	

	/*
	m_io_channel = Glib::IOChannel::create_from_fd(fd);
	m_io_channel->set_encoding();
	m_io_channel->set_buffered(0);
	Glib::signal_io().connect(
		sigc::mem_fun(*this,&AppMan::on_socket_changed),m_io_channel,Glib::IO_IN);
	*/

	Glib::signal_io().connect(
		sigc::mem_fun(*this,&AppMan::on_socket_changed),fd,Glib::IO_IN);		
}

bool AppMan::on_socket_changed(Glib::IOCondition io)
{
	got_here();

	m_handler->update();

	return true;
}

AppMan::AppMan(Handler* p_handler)
{
	m_handler = p_handler;

	m_window = new MainWindow(m_handler);
	m_prefswindow = new PrefsWindow(m_window);
	m_icon = new TrayIcon(m_window,m_prefswindow);

	//setupSocket();	
	m_prefswindow->loadSettings();

	m_window->show();
}
