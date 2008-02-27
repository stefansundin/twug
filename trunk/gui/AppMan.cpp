#include "AppMan.h"


AppMan::AppMan()
{
	//initialize the Handler
	m_handler = new Handler();

	m_window = new MainWindow(m_handler);
	m_icon = new TrayIcon(m_window);
	m_prefswindow = new PrefsWindow(m_icon,m_window);

	m_prefswindow->loadSettings();

	m_window->show();
	m_prefswindow->show();
	
/*
networkmanager integration	
*/
}




