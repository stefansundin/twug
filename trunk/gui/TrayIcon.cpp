#include "TrayIcon.h"


TrayIcon::TrayIcon(UIEvents* p_events)
{
#ifndef _WIN32
	set_from_file("/usr/share/pixmaps/twug.png");
#endif
	m_events = p_events;


	m_menu = new Gtk::Menu();

#ifndef _WIN32
	signal_activate().connect(sigc::mem_fun(*this,&TrayIcon::on_clicked));
	signal_popup_menu().connect(sigc::mem_fun(*this,&TrayIcon::on_popup));
#endif

	m_menu->items().push_back(Gtk::Menu_Helpers::MenuElem("_About",
	 	sigc::mem_fun(*this,&TrayIcon::on_action_about) ));
	m_menu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Preferences",
	 	sigc::mem_fun(*this,&TrayIcon::on_action_prefs) ));
	m_menu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Quit",
	 	sigc::mem_fun(*this,&TrayIcon::on_action_quit) ));
}



TrayIcon::~TrayIcon()
{
}


void TrayIcon::on_clicked()
{
	m_events->to_ui->pushEvent( UIEvent ( "TOGGLE_TWUG_VISIBILITY" ) );
}

void TrayIcon::on_popup(const unsigned int& btn, const unsigned int& time)
{	
#ifndef _WIN32
	popup_menu_at_position (*m_menu, btn, time);
#endif
}


void TrayIcon::on_action_prefs()
{	
	m_events->to_ui->pushEvent( UIEvent ( "SHOW_PREFSWINDOW" ) );
}

void TrayIcon::on_action_quit()
{
	Gtk::Main::quit();
}

void TrayIcon::on_action_about()
{
	m_events->to_ui->pushEvent( UIEvent ( "SHOW_ABOUT" ) );
}
