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
#include <gtkmm/messagedialog.h>

#include "HandlerLink.h"
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
	void spawnErrorDialog(std::string p_titlebar,std::string p_textbody);

	std::string getServerIp(std::string text);
	Glib::ustring getSelectionValue();
	bool isChannel(std::string name);
	void on_treeview_clicked(GdkEventButton* evb);
	void on_personmenu_message();
	void on_channelmenu_join();

	bool m_dontdoshit;
	std::vector<Glib::ustring> m_lastserverlist;
	std::string* m_nameptr;

	
	Handler* m_handler;

	Gtk::Button m_button;
	Gtk::ComboBoxText m_popup;
	Gtk::Menu* m_personmenu;
	Gtk::Menu* m_channelmenu;
	mwColumns *m_columns;
	Gtk::TreeView *m_treeview;
	Glib::RefPtr<Gtk::TreeStore> m_treestore;
public:
MessageHandler* m_msghandler;
	MainWindow(Handler* p_handler);
	virtual ~MainWindow();
	void toggleVisibility();
	void giveServers(std::vector<Glib::ustring> p_servers);

	bool m_autoopen;
	std::string m_newname;

	//callbacks
	void connectedAs(std::string p_ip, std::string p_name); 
	void connectionError(bool p_type, std::string p_err);
	void reloadChannels();
};

#endif

