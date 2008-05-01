#ifndef TRAYICON_H
#define TRAYICON_H

#include <gtkmm/stock.h>
#include <gtkmm/statusicon.h>
#include <gtkmm.h>


#include "UIEvents.h"


class TrayIcon : public Gtk::StatusIcon
{
public:
  TrayIcon(UIEvents* p_events);
 ~TrayIcon();
protected:
	UIEvents* m_events;
	virtual void on_clicked();
	virtual void on_popup(const unsigned int&, const unsigned int&);
	void on_action_quit();
	void on_action_prefs();
	void on_action_about();
	Gtk::Menu* m_menu;
	bool m_restoreprefs;
};

#endif
