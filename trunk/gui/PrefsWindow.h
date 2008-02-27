#ifndef PREFSWINDOW_H
#define PREFSWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/box.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/label.h>
#include <gtkmm/treeselection.h>
#include <gtkmm/cellrenderer.h>
#include <gtkmm/cellrenderertext.h>
#include <glibmm/slisthandle.h>
#include <iostream>
#include <gconfmm-2.6/gconfmm.h>
#include "TrayIcon.h"
#include <vector>
#include "MainWindow.h"

class Columns : public Gtk::TreeModelColumnRecord
{
public:
	Gtk::TreeModelColumn<Glib::ustring> name;
	Gtk::TreeModelColumn<Glib::ustring> ip;
	Columns()
	{
		add(name);
		add(ip);
	}
};

class PrefsWindow : public Gtk::Window
{
public:
	PrefsWindow(TrayIcon* p_icon,MainWindow* p_window);
	virtual ~PrefsWindow();
	Gtk::CheckButton *m_chkbtn_blinking;
	virtual void loadSettings();
	std::vector<Glib::ustring> getServerList();
	virtual void on_tree_changed();
protected:
	MainWindow* m_window;
	void reloadServers();
	Columns *m_columns;
	//Gtk::ListStore *m_serverstore;
	Gtk::TreeView *m_serverlist;
	Glib::RefPtr<Gtk::ListStore> m_serverstore;
	Glib::RefPtr<Gnome::Conf::Client> m_gconf;
	Gtk::StatusIcon* m_icon;
	virtual void on_button_clicked();

	virtual void onGConfChanged(const Glib::ustring&, const Gnome::Conf::Value&);

};

#endif

