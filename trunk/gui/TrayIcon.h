#ifndef TRAYICON_H
#define TRAYICON_H

#include <gtkmm/stock.h>
#include <gtkmm/statusicon.h>
#include <gtkmm.h>
#include "MainWindow.h"

class TrayIcon : public Gtk::StatusIcon
{
public:
  TrayIcon(MainWindow* p_window);
 ~TrayIcon();
protected:
	MainWindow* m_window;
	virtual void on_clicked();
	virtual void on_popup(const unsigned int&, const unsigned int&);
	void on_menu_quit();
	Glib::RefPtr<Gtk::ActionGroup> m_actiongroup;
	Gtk::Menu* m_menu;
};

#endif