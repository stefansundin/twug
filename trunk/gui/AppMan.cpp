#include "AppMan.h"

void AppMan::setupSocket()
{
//	int fd = m_handler->getSocket();

	int fd = open("kaka.txt", O_RDONLY);

	Glib::signal_io().connect(
		sigc::mem_fun(*this,&AppMan::on_socket_changed),fd,Glib::IO_IN);
	Glib::IOChannel::create_from_fd(fd);
}

bool AppMan::on_socket_changed(Glib::IOCondition io)
{
	if((io & Glib::IO_IN) == 0)
	{
		printf("no good\n");
	}
	else
	{
//		printf("good\n");
		m_handler->update();
	}

	return true;
}

AppMan::AppMan(Handler* p_handler)
{
	m_handler = p_handler;

	m_window = new MainWindow(m_handler);
	m_prefswindow = new PrefsWindow(m_window);
	m_icon = new TrayIcon(m_window,m_prefswindow);

	setupSocket();	
	m_prefswindow->loadSettings();

	m_window->show();
}
