#ifndef PREFSWINDOW_H
#define PREFSWINDOW_H

#include <iostream>
#include <vector>
#include <string>

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
#include <gconfmm-2.6/gconfmm.h>

#include "../UIEvents.h"

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
	PrefsWindow(UIEvents* p_events);
	virtual ~PrefsWindow();
	Gtk::CheckButton *m_chkbtn_blinking;
	virtual void loadSettings();
	std::vector<Glib::ustring> getServerList();
	void toggleVisibility();
protected:
	UIEvents* m_events;

	void reloadServers();
	void saveServerList();

	Columns *m_columns;
	Gtk::TreeView *m_serverlist;
	Glib::RefPtr<Gtk::ListStore> m_serverstore;
	Glib::RefPtr<Gnome::Conf::Client> m_gconf;

	Gtk::Button m_buttonAdd;
	Gtk::Button m_buttonRemove;

	Gtk::Entry *m_nameEntry;

	virtual void on_button_clicked();
	void on_buttonAdd_clicked();
	void on_buttonRemove_clicked();
	virtual void on_tree_changed(const Glib::ustring&, const Glib::ustring&);
	virtual void onGConfChanged(const Glib::ustring&, const Gnome::Conf::Value&);
	void on_name_changed();
};

#endif

