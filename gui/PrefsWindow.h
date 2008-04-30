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

#include "UIEvents.h"
#include "ClientSettings.h"


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
	void toggleVisibility();
protected:
	ClientSettings m_settings;

	UIEvents* m_events;

	void saveServerList();

	Columns *m_columns;
	Gtk::TreeView *m_serverlist;
	Glib::RefPtr<Gtk::ListStore> m_serverstore;

	Gtk::Button m_buttonAdd;
	Gtk::Button m_buttonRemove;

	Gtk::Entry *m_nameEntry;

	void on_buttonAdd_clicked();
	void on_buttonRemove_clicked();
	virtual void on_tree_changed(const Glib::ustring&, const Glib::ustring&);
	void on_name_changed();
};

#endif

