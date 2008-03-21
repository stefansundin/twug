#include "TrayIcon.h"


TrayIcon::TrayIcon(MainWindow* p_window, PrefsWindow* p_prefswindow)
{
	set_from_file("/usr/share/pixmaps/twug.png");

	m_restoreprefs = false;

	m_window = p_window;
	m_prefswindow = p_prefswindow;

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
	if (m_window->is_visible())
	{
		if(m_prefswindow->is_visible())
			{
				m_prefswindow->toggleVisibility();
				m_restoreprefs = true;
			}
	} else {
		if(m_restoreprefs)
		{
			m_prefswindow->show();
			m_restoreprefs=false;
		}
	}


	m_window->toggleVisibility();
}

void TrayIcon::on_popup(const unsigned int& btn, const unsigned int& time)
{	
	popup_menu_at_position (*m_menu, btn, time);		
}


void TrayIcon::on_action_prefs()
{
	m_prefswindow->show();
}

void TrayIcon::on_action_quit()
{
	Gtk::Main::quit();
}

void TrayIcon::on_action_about()
{
	m_about->show();	
}
