#include "TrayIcon.h"


TrayIcon::TrayIcon(UIEvents* p_events)
{
	set_from_file("/usr/share/pixmaps/twug.png");

	m_restoreprefs = false;

	m_events = p_events;

	m_about = new AboutTwug();
	m_menu = new Gtk::Menu();
	
	signal_activate().connect(sigc::mem_fun(*this,&TrayIcon::on_clicked));
	signal_popup_menu().connect(sigc::mem_fun(*this,&TrayIcon::on_popup));

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
	popup_menu_at_position (*m_menu, btn, time);		
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
	//m_about->show();
	//m_about->present();
	m_about->run();
	m_about->hide();
}
