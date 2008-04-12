#include "UIManager.h"

UIManager::UIManager(UIEvents* p_events)	
{
	got_here();

	std::cout << "UIManager: constructing" << std::endl;

	m_events = p_events;
	m_restoreprefswindow=0;

	/*int readfd = open(m_events->to_ui->getFilePath().c_str(), O_RDONLY);
	got_here();
 	Glib::signal_io().connect(sigc::mem_fun(*this,&UIManager::on_fd_readable), readfd, Glib::IO_IN);
	got_here();
	m_iochannel = Glib::IOChannel::create_from_fd(readfd);
	got_here();*/

	m_events->to_network->pushEvent( UIEvent ("HACK" ));
got_here();
	m_iochannel = Glib::IOChannel::create_from_file(m_events->to_ui->getFilePath() , "r" );
	got_here();
 	Glib::signal_io().connect(sigc::mem_fun(*this,&UIManager::on_fd_readable), m_iochannel, Glib::IO_IN);


	m_window = new MainWindow(m_events);
	m_prefswindow = new PrefsWindow(m_events);
	m_icon = new TrayIcon(m_events);
	m_prefswindow->loadSettings();
	m_window->show();
	std::cout << "UIManager: constructed" << std::endl;
}

bool UIManager::on_fd_readable(Glib::IOCondition condition)
{
	if ((condition & Glib::IO_IN) == 0)
	{
		std::cout << "Error" << std::endl;
	} else {
		Glib::ustring buf;
		m_iochannel->read_line(buf);
		//std::cout << "buf: " << buf << std::endl;
		processEvents();
  	}
	
	return true;
}

void UIManager::processEvents()
{
	std::cout << "process events to_ui" << std::endl;

	bool kaka=true;
	while(kaka)
	{
		UIEvent event = m_events->to_ui->popEvent();
		std::cout << "event type: " << event.getType() << std::endl;
		if (event.getType() == "EMPTY") {
			kaka=false;
		} else if (event.getType() == "NEW_CONNECTION_STATUS") {
			std::string s = event.pop_first();
			if (s == "CONNECTION_LOST")
				m_window->event_connectionLost(event.pop_first() );
			else if (s == "CONNECTED")
				m_window->event_connected(event.pop_first(), event.pop_first() );
			else if (s == "CONNECTING")

				//m_window->event_connecting(event.pop(), event.pop() );
				std::cout << "event_connecting" << std::endl;
			else if (s == "ERROR_CONNECTING")

				m_window->event_errorConnecting(event.pop_first(), event.pop(), event.pop_first() );

			else if (s == "DISCONNECTED")
				std::cout << event.getFirst() << std::endl;
				m_window->event_disconnected( );
		} else if (event.getType() == "TEXT_MESSAGE") {
				m_window->event_textMessage( event.pop(), event.pop() );
		} else if (event.getType() == "NEW_CHANNEL_LIST") {
				m_window->event_newChannelList(event.getData());
		} else if (event.getType() == "NEW_SERVER_LIST") {
				m_window->event_newServerList(event.getData());
		} else if (event.getType() == "NEW_NEWNAME") {
				m_window->event_newNewName(event.pop());
		} else if (event.getType() == "TOGGLE_TWUG_VISIBILITY") {
				event_toggleTwugVisibility();
		} else if (event.getType() == "SHOW_PREFSWINDOW") {
				m_prefswindow->show();
		} else {
				std::cout << "UIManager: Got invalid event" << std::endl;
		}
		
	}
}


void UIManager::event_toggleTwugVisibility()
{
	if (m_window->is_visible())
	{
		if(m_prefswindow->is_visible())
			{
				m_prefswindow->toggleVisibility();
				m_restoreprefswindow = true;
			}
	} else {
		if(m_restoreprefswindow)
		{
			m_prefswindow->show();
			m_restoreprefswindow=false;
		}
	}

	m_window->toggleVisibility();
}
