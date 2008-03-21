#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <vector>
#include <string>

#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/main.h>
#include <gtkmm/box.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/menu.h>
#include <gtkmm/comboboxtext.h>

#include "../handler/Handler.h"
#include "MessageHandler.h"


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
protected:
	virtual void on_button_released();
	virtual void on_button_pressed();
	virtual void on_popup_changed();
	void reloadChannels();

	std::string getServerIp(std::string text);
	Glib::ustring getSelectionValue();
	void on_person_clicked(GdkEventButton* evb);
	void on_menuitem_clicked();

	bool m_dontdoshit;
	std::vector<Glib::ustring> m_lastserverlist;

	MessageHandler* m_msghandler;
	Handler* m_handler;

	Gtk::Button m_button;
	Gtk::ComboBoxText m_popup;
	Gtk::Menu* m_menu;
	mwColumns *m_columns;
	Gtk::TreeView *m_treeview;
	Glib::RefPtr<Gtk::TreeStore> m_treestore;
public:
	MainWindow(Handler* p_handler);
	virtual ~MainWindow();
	void toggleVisibility();
	void giveServers(std::vector<Glib::ustring> p_servers);

	bool m_autoopen;
};

#endif

