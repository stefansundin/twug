#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/main.h>
#include <gtkmm/box.h>
#include <iostream>
#include <vector>
#include <string>
#include <gtkmm/comboboxtext.h>
#include "../handler/Handler.h"
#include "MessageHandler.h"

#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/menu.h>
//#include <gtkmm.h>

class mwColumns : public Gtk::TreeModelColumnRecord
{
public:
	Gtk::TreeModelColumn<Glib::ustring> name;
	mwColumns()
	{
		add(name);
	}
};

class MainWindow : public Gtk::Window
{
public:
	MainWindow(Handler* p_handler);
	virtual ~MainWindow();
	void toggleVisibility();
	void giveServers(std::vector<Glib::ustring> p_servers);
protected:
	//virtual void on_button_clicked();
	virtual void on_button_released();
	virtual void on_button_pressed();
	virtual void on_popup_changed();
	void reloadChannels();
	std::vector<Glib::ustring> m_lastserverlist;
	Gtk::Button m_button;
	Gtk::ComboBoxText m_popup;
	Handler* m_handler;
	std::string getServerIp(std::string text);
	bool m_dontdoshit;

	//treeview stuff:

	Glib::ustring getSelectionValue();
	mwColumns *m_columns;
	Gtk::TreeView *m_treeview;
	Glib::RefPtr<Gtk::TreeStore> m_treestore;

	void on_person_clicked(GdkEventButton* evb);
	void on_menuitem_clicked();

	//MessageHandler* m_msgs;
	MessageHandler* m_msghandler;
	Gtk::Menu* m_menu;
};

#endif

