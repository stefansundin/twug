#include "MainWindow.h"

MainWindow::MainWindow(Handler* p_handler)
{
	m_dontdoshit=false;
	set_title("Twug UI 0.3");
	m_handler = p_handler;

	//set_border_width(10);

	Gtk::VBox* vbox = new Gtk::VBox();
 	add(*vbox);

//initialize popup menu
	vbox->add(m_popup);
	
	//m_popup.append_text("Not Connected"); 
	//m_popup.set_active_text("Not Connected");
	m_popup.signal_changed().connect(sigc::mem_fun(*this,
              &MainWindow::on_popup_changed) );


//add buttons
	vbox->add(m_button);
	m_button.set_label("Talk button");
	m_button.signal_pressed().connect(sigc::mem_fun(*this,&MainWindow::on_button_pressed));
	m_button.signal_released().connect(sigc::mem_fun(*this,&MainWindow::on_button_released));

	show_all();
}

/*
	VBox with 2 rows:
		Popup menu:
			Servers <GREYED TEXT>
			[ ] Not connected
			[X] Saved server 1
			[ ] Saved server 2
			(...)
			---------------
			Preferences...
			About...
			---------------
			Quit this Application
		TreeView:
			<GREENICON> Channel 1
				Random guy 1
				Random guy 2
				<yourself>
				(...)
			<REDICON> Channel 1
				Random guy 3
				(...)
			(...)
*/


MainWindow::~MainWindow()
{
}

//void MainWindow::reloadServers()
void MainWindow::giveServers(std::vector<Glib::ustring> p_servers)
{
//todo: reconnect if ip for current server changed
	m_lastserverlist = p_servers;

	Glib::ustring temp;
	if(m_popup.get_active_text() != "")
		temp = m_popup.get_active_text();
	else
		temp = "Not Connected";

	m_dontdoshit=true;
		m_popup.clear_items();
		m_popup.append_text("Not Connected");

		std::cout << "MainWindow: Recieved new server list: ";
		for (int i=0;i<m_lastserverlist.size();i+=2)
		{
			m_popup.append_text(m_lastserverlist.at(i));
			std::cout << m_lastserverlist.at(i+1) << " ";
		}
		std::cout << std::endl;

		m_popup.set_active_text(temp);
	m_dontdoshit=false;
}

void MainWindow::on_button_pressed()
{
  //std::cout << "button pressed" << std::endl;
  m_handler->iStartTalking();
}

void MainWindow::on_button_released()
{
  //std::cout << "button released" << std::endl;
  m_handler->iStopTalking();
}

void MainWindow::on_popup_changed()
{
	if(!m_dontdoshit)
	{
		Glib::ustring text = m_popup.get_active_text();

		if (text != "Not Connected" && text!= "")
		{
			if (!m_handler->connectToServer( getServerIp(text) ))
			{
				m_popup.set_active_text("Not Connected");			
			} else {
				reloadChannels();
			}
		} else {
			m_handler->connectToServer("0");
		}
	}
}

std::string MainWindow::getServerIp(std::string text)
{
	for(int i=0;i<m_lastserverlist.size();i+=2)
	{
		if(m_lastserverlist.at(i)==text)
		{
			return m_lastserverlist.at(i+1);
		} 	
	}
	return text;
}


void MainWindow::reloadChannels()
{
	std::cout << "new channel list: \n";
	std::vector<std::string> channels = m_handler->getChannels();
	for(int i=0;i<channels.size();i++)
	{
		std::cout << channels.at(i) << std::endl;
		std::vector<std::string> channelMembers = m_handler->getChannelMembers( channels.at(i) );
		if(!channelMembers.empty())
			for(int a=0;a<channelMembers.size();a++)
			{
				std::cout << "   " << channelMembers.at(a) << std::endl;
			}
	} 
}


void MainWindow::toggleVisibility()
{
	std::cout << "toggling" << std::endl;
	if(!is_visible())
	  	show();
	else
		hide();
}
