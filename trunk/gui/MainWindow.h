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
#include <gtkmm/aboutdialog.h>

#include "../debug.h"
#include "UIEvents.h"
#include "MessageHandler.h"
#include "ChannelList.h"

class AboutTwug : public Gtk::AboutDialog
{
private:
	std::vector<Glib::ustring> m_credits;
	void addCredits(Glib::ustring line)
	{
		m_credits.push_back(line);
	}
public:
	AboutTwug()
	{
		addCredits("made by certain people...");

		set_authors(m_credits);
		set_name("Twug");
		set_comments("The Voice Communications Tool");
		set_copyright("©2007-2008 The Twug Team");
		set_website("http://code.google.com/p/twug");
		set_version("0.2");
	}
};

class EntryDialog : public Gtk::MessageDialog
{
public:
	EntryDialog(std::string p_first,std::string p_secondary, UIEvents* p_events) : Gtk::MessageDialog(*this,p_first,false,Gtk::MESSAGE_QUESTION,Gtk::BUTTONS_OK_CANCEL)
	{
		m_events = p_events;
		set_secondary_text(p_secondary);
		m_entry.signal_activate().connect(sigc::mem_fun(*this,&EntryDialog::hack));
		get_vbox()->add(m_entry);
	}
	void trigger()
	{
		m_entry.set_text("");
		set_focus(m_entry);
		show_all();

		int answer = run();

		if ( m_entry.get_text() != "" && answer == Gtk::RESPONSE_OK )
		{
			doAction();
		}
		hide();
	}
protected:
	UIEvents* m_events;
	Gtk::Entry m_entry;
	void hack()
	{
		response(Gtk::RESPONSE_OK);
		hide();
	}
	virtual void doAction()
	{
		print_me("ERROR");
	}
};

class BroadcastDialog : public EntryDialog
{
public:
	BroadcastDialog(UIEvents* p_events) : EntryDialog("Broadcast message","Please type your message below.", p_events)
	{
	}
protected:
	virtual void doAction()
	{
		m_events->to_network->pushEvent( UIEvent ( "BROADCAST_TEXT", m_entry.get_text()  ) );
	}
};

class AddChannelDialog : public EntryDialog
{
public:
	AddChannelDialog(UIEvents* p_events) : EntryDialog("Add new channel","What do you want to call the new channel?", p_events)
	{
	}
protected:
	virtual void doAction()
	{
		m_events->to_network->pushEvent( UIEvent ( "NEWCHANNEL", m_entry.get_text()  ) );
	}
};


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
	void event_ErrorMessage(std::string p_header, std::string p_message);
	void event_Notification(std::string p_header, std::string p_message);
	void event_runAbout();
protected:
	virtual void on_aboutbutton_clicked();
	virtual void on_prefsbutton_clicked();
	virtual void on_quitbutton_clicked();

	virtual void on_button_released();
	virtual void on_addbutton_clicked();
	virtual void on_broadcastbutton_clicked();
	virtual void on_button_pressed();
	virtual void on_popup_changed();
	void on_button_clicked();
	void spawnErrorDialog(std::string p_titlebar,std::string p_textbody);
	void spawnInfoDialog(std::string p_titlebar,std::string p_textbody);

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

	AddChannelDialog* m_add_dialog;
	BroadcastDialog* m_broadcast_dialog;
	AboutTwug* m_about;
};

#endif //MainWindow_h

