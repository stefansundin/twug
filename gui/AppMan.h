#ifndef APPMAN_H
#define	APPMAN_H

#include "TrayIcon.h"
#include "MainWindow.h"
#include "PrefsWindow.h"
#include "../handler/Handler.h"

class AppMan
{
protected:
	MainWindow *m_window;
	PrefsWindow *m_prefswindow;
	TrayIcon *m_icon;
	Handler *m_handler;

	void setupSocket();
	bool on_socket_changed(Glib::IOCondition io);
public:
	AppMan();
	//~AppMan();
};

#endif