#include "TrayIcon.h"


TrayIcon::TrayIcon(MainWindow* p_window, PrefsWindow* p_prefswindow)
{
	m_restoreprefs = false;
	m_window = p_window;
	m_prefswindow = p_prefswindow;

	set(Gtk::Stock::NEW);
	signal_activate().connect(sigc::mem_fun(*this,&TrayIcon::on_clicked));
	signal_popup_menu().connect(sigc::mem_fun(*this,&TrayIcon::on_popup));

	// -- start popup initialization
		m_menu = new Gtk::Menu();

		m_menu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Preferences",
		 	sigc::mem_fun(*this,&TrayIcon::on_action_prefs) ));
		m_menu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Quit",
		 	sigc::mem_fun(*this,&TrayIcon::on_action_quit) ));
	// -- end popup initialization
}



TrayIcon::~TrayIcon()
{
}


void TrayIcon::on_clicked()
{
	std::cout << "clicked tray icon" << std::endl;
	
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

