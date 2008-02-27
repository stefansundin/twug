#ifndef APPMAN_H
#define	APPMAN_H

#include "TrayIcon.h"
#include "MainWindow.h"
#include "../handler/Handler.h"
#include "PrefsWindow.h"

class AppMan
{
protected:
	MainWindow *m_window;
	PrefsWindow *m_prefswindow;
	TrayIcon *m_icon;
	Handler *m_handler;
public:
	AppMan();
	//~AppMan();
};

#endif
