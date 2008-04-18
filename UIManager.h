#ifndef UIManager_h
#define	UIManager_h

#include <vector>
#include <string>
#include <iostream>

#include <cstdio>
#include <fcntl.h>

#include <gtkmm/main.h>

#include "gui/UIEvents.h"
#include "debug.h"
#include "gui/TrayIcon.h"
#include "gui/MainWindow.h"
#include "gui/PrefsWindow.h"

class UIManager
{
public:
	UIManager(UIEventQueue* p_to_ui, UIEventQueue* p_to_network, void (*p_funptr)());

	bool on_fd_readable(Glib::IOCondition condition);
	void processEvents();
	void trigger();

private:
	void event_toggleTwugVisibility();

	Glib::RefPtr<Glib::IOChannel> m_iochannel;
	UIEvents* m_events;
	TrayIcon* m_icon;
	MainWindow* m_window;
	PrefsWindow* m_prefs_window;
	bool m_restore_prefs_window;
};

#endif //UIManager_h

