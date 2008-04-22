#ifndef MainWindow_h
#define MainWindow_h

#include <iostream>
#include <vector>
#include <string>

#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/main.h>
#include <gtkmm/box.h>
#include <gtkmm/menu.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/stock.h>

#include "../debug.h"
#include "UIEvents.h"
#include "MessageHandler.h"
#include "ChannelList.h"

class MainWindow : public Gtk::Window
{
public:
	MessageHandler* m_msghandler;
	MainWindow(UIEvents* p_events);
	virtual ~MainWindow();
	void toggleVisibility();

	void event_newServerList(std::vector<std::string> p_servers);
	void event_textMessage( std::string sender, std::string message);
	void event_newChannelList(std::vector<std::string> channels);
	void event_connected(std::string p_ip, std::string p_name);
	void event_errorConnecting(std::string p_err);
	void event_connectionLost(std::string p_address);
	void event_newNewName (std::string p_name);
	void event_showMsgWindow (std::string p_name);
	void event_disconnected ();
	void event_connecting(std::string p_server);
	void event_loggingIn(std::string p_server);
protected:
	virtual void on_button_released();
	virtual void on_addbutton_clicked();
	virtual void on_button_pressed();
	virtual void on_popup_changed();
	void on_button_clicked();
	void spawnErrorDialog(std::string p_titlebar,std::string p_textbody);

	std::string getServerIp(std::string text);

	bool m_dont_do_shit;
	std::vector<std::string> m_lastserverlist;
	std::string* m_nameptr;
	std::string m_newname;

	UIEvents* m_events;

	Gtk::Button m_button;
	Gtk::ComboBoxText m_popup;

	ChannelList *m_channellist;
	void setup_channelList(); 

	void hack();
	Gtk::MessageDialog* m_kaka;
	Gtk::Entry* m_entry;
};

#endif //MainWindow_h

