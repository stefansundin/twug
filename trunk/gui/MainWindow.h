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

#include "../debug.h"
#include "../UIEvents.h"
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
	void on_button_clicked();
	void spawnErrorDialog(std::string p_titlebar,std::string p_textbody);

	std::string getServerIp(std::string text);
	Glib::ustring getSelectionValue();
	bool isChannel(std::string name);
	void on_treeview_clicked(GdkEventButton* evb);
	void on_personmenu_message();
	void on_channelmenu_join();
	void on_channelmenu_removeChannel();
	void on_backgroundmenu_newChannel();

	bool m_dontdoshit;
	std::vector<std::string> m_lastserverlist;
	std::vector<std::string> m_lastchannellist;
	std::string* m_nameptr;
	std::string m_newname;
	std::string m_mychannel;

	
	UIEvents* m_events;

	Gtk::Button m_button;
	Gtk::ComboBoxText m_popup;
	Gtk::Menu* m_personmenu;
	Gtk::Menu* m_channelmenu;
	Gtk::Menu* m_backgroundmenu;
	mwColumns *m_columns;
	Gtk::TreeView *m_treeview;
	Glib::RefPtr<Gtk::TreeStore> m_treestore;
public:
	MessageHandler* m_msghandler;
	MainWindow(UIEvents* p_events);
	virtual ~MainWindow();
	void toggleVisibility();

	bool m_autoopen;

	void event_newServerList(std::vector<std::string> p_servers);
	void event_textMessage( std::string sender, std::string message);
	void event_newChannelList(std::vector<std::string> channels);
	void event_connected(std::string p_ip, std::string p_name);
	void event_errorConnecting(std::string p_err, std::string, std::string);
	void event_connectionLost(std::string p_address);
	void event_newNewName (std::string p_name);
	void event_disconnected ();


};

#endif

