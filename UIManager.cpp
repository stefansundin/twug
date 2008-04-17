#include "UIManager.h"

UIManager::UIManager(UIEvents* p_events)
{
	m_events = p_events;
	m_restore_prefs_window=0;

	//m_events->to_network->pushEvent( UIEvent ("HACK" ));
	m_iochannel = Glib::IOChannel::create_from_file(m_events->to_ui->getFilePath() , "r" );
 	Glib::signal_io().connect(sigc::mem_fun(*this,&UIManager::on_fd_readable), m_iochannel, Glib::IO_IN);
}

void UIManager::trigger()
{
	m_window = new MainWindow(m_events);
	m_prefs_window = new PrefsWindow(m_events);
	m_icon = new TrayIcon(m_events);

	m_prefs_window->loadSettings();
	m_window->show();
}

bool UIManager::on_fd_readable(Glib::IOCondition condition)
{
	if ((condition & Glib::IO_IN) == 0)
	{
		print_me("Error: got other than Glib::IO_IN");
	} else {
		print_me("UIManager: reading from fd");
		Glib::ustring buf;
		m_iochannel->read_line(buf);
		print_me("UIManager: read this line from fd: "+buf);
		processEvents();
	}

	return true;
}

void UIManager::processEvents()
{
	bool process = true;
	while(process)
	{
		UIEvent event = m_events->to_ui->popEvent();
		std::cout << "event type: " << event.getType() << std::endl;

		if (event.getType() == "EMPTY") {
			process = false;
		} else if (event.getType() == "NEW_CONNECTION_STATUS") {
			std::string s = event.pop_first();
			if (s == "CONNECTION_LOST") {
				m_window->event_connectionLost(event.pop_first());
			} else if (s == "CONNECTED") {
				std::string ip = event.pop_first();
				std::string name = event.pop_first();
				m_window->event_connected(ip, name);
			} else if (s == "CONNECTING") {
				//m_window->event_connecting(event.pop(), event.pop() );
				std::cout << "event_connecting" << std::endl;
			} else if (s == "ERROR_CONNECTING") {
				m_window->event_errorConnecting(event.pop_first() );
			} else if (s == "DISCONNECTED") {
				print_me(event.getFirst());
				m_window->event_disconnected();
			}
		} else if (event.getType() == "TEXT_MESSAGE") {
				m_window->event_textMessage(event.pop(), event.pop());
		} else if (event.getType() == "NEW_CHANNEL_LIST") {
				m_window->event_newChannelList(event.getData());
		} else if (event.getType() == "NEW_SERVER_LIST") {
				m_window->event_newServerList(event.getData());
		} else if (event.getType() == "NEW_NEWNAME") {
				m_window->event_newNewName(event.pop());
		} else if (event.getType() == "TOGGLE_TWUG_VISIBILITY") {
				event_toggleTwugVisibility();
		} else if (event.getType() == "SHOW_PREFSWINDOW") {
				m_prefs_window->show();
		} else {
				print_me("UIManager: Got invalid event");
		}
	}
}


void UIManager::event_toggleTwugVisibility()
{
	if (m_window->is_visible())
	{
		if(m_prefs_window->is_visible())
			{
				m_prefs_window->toggleVisibility();
				m_restore_prefs_window = true;
			}
	} else {
		if(m_restore_prefs_window)
		{
			m_prefs_window->show();
			m_restore_prefs_window=false;
		}
	}

	m_window->toggleVisibility();
}

