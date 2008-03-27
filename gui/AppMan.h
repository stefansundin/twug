#ifndef APPMAN_H
#define	APPMAN_H

#include "TrayIcon.h"
#include "MainWindow.h"
#include "PrefsWindow.h"
#include "HandlerLink.h"

class AppMan
{
protected:
	void setupSocket();
	bool on_socket_changed(Glib::IOCondition io);
public:
	MainWindow *m_window;
	PrefsWindow *m_prefswindow;
	TrayIcon *m_icon;
	Handler *m_handler;

	AppMan(Handler* p_handler);
	//~AppMan();
};

#endif
