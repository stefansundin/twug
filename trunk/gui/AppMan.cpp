#include "AppMan.h"


AppMan::AppMan()
{
	m_handler = new Handler();

	m_window = new MainWindow(m_handler);

	m_prefswindow = new PrefsWindow(m_window);
	m_icon = new TrayIcon(m_window,m_prefswindow);

	m_prefswindow->loadSettings();

	m_window->show();
}




