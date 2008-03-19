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
#include "../handler/Handler.h"

class MessageWindow : public Gtk::Window
{
private:
	Handler* m_handler;
	std::string m_name;
	std::string m_myname;
	Glib::RefPtr<Gtk::TextBuffer> m_buffer;
	//widgets:
	Gtk::TextView* m_textview;
	Gtk::Button* m_button;
	Gtk::Entry* m_entry;
	Gtk::ScrolledWindow* m_scrolled;

	void scrollDown();
public:
	MessageWindow(std::string p_name, Handler* p_handler);
	void giveMessage(std::string p_msg);
	std::string getName();
	void sendEntry();
};

#endif
