#ifndef UIManager_h
#define	UIManager_h

#include <vector>
#include <string>
#include <iostream>

#include <cstdio>
#include <fcntl.h>

#include <gtkmm/main.h>

#include "UIEvents.h"
#include "debug.h"
#include "gui/TrayIcon.h"
#include "gui/MainWindow.h"
#include "gui/PrefsWindow.h"

class UIManager
{
public:
	UIManager(UIEvents* p_events);	

	bool on_fd_readable(Glib::IOCondition condition);
	void processEvents();
private:
	Glib::RefPtr<Glib::IOChannel> m_iochannel;
	UIEvents* m_events;
	TrayIcon* m_icon;
	MainWindow* m_window;
	PrefsWindow* m_prefswindow;
	void event_toggleTwugVisibility();


	bool m_restoreprefswindow;
};

#endif

