#ifndef MESSAGEWINDOW_H
#define	MESSAGEWINDOW_H

#include <string>
#include <iostream>

#include <gtkmm/window.h>
#include <gtkmm/textview.h>
#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/scrolledwindow.h>

#include "../debug.h"
#include "UIEvents.h"

class MessageWindow : public Gtk::Window
{
private:
	void sendEntry();
	void scrollDown();
	
	UIEvents* m_events;
	std::string* m_nameptr;

	std::string m_name;
	std::string m_myname;
	
	Gtk::TextView* m_textview;
	Gtk::Button* m_button;
	Gtk::Entry* m_entry;
	Gtk::ScrolledWindow* m_scrolled;
	Glib::RefPtr<Gtk::TextBuffer> m_buffer;
public:
	MessageWindow(std::string p_name, UIEvents* p_events, std::string* p_nameptr);
	void giveMessage(std::string p_msg);
	std::string getName();	
};

#endif
