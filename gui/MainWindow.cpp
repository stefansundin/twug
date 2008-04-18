#include "MainWindow.h"


MainWindow::MainWindow(UIEvents* p_events)
{
	m_events = p_events;

	m_nameptr = new std::string();

	set_title("Twug");
	set_default_icon_from_file("/usr/share/pixmaps/twug.png");
	//set_border_width(10);

	m_dont_do_shit = false;

	m_msghandler = new MessageHandler(m_events,m_nameptr);

	Gtk::VBox* vbox = new Gtk::VBox();


	Gtk::TreeModelColumn<Glib::ustring>* namecolumn = new Gtk::TreeModelColumn<Glib::ustring>();
	Gtk::TreeModelColumnRecord* columns = new Gtk::TreeModelColumnRecord();
	columns->add(*namecolumn);
        Glib::RefPtr<Gtk::TreeStore> treestore = Gtk::TreeStore::create(*columns);
        m_channellist = new ChannelList(treestore,namecolumn,m_events,m_nameptr);


	
	m_button.set_border_width(5);
	m_button.set_label("Talk button");

	vbox->add(m_popup);
	vbox->add(m_button);
	vbox->add(*m_channellist);
	add(*vbox);
	show_all();



	m_popup.signal_changed().connect(
		sigc::mem_fun(*this,&MainWindow::on_popup_changed) );
	m_button.signal_pressed().connect(
		sigc::mem_fun(*this,&MainWindow::on_button_pressed));
	m_button.signal_released().connect(
		sigc::mem_fun(*this,&MainWindow::on_button_released));
}


void MainWindow::event_newNewName (std::string p_name)
{
	m_newname = p_name;
}

MainWindow::~MainWindow()
{
}

void MainWindow::event_showMsgWindow (std::string p_name)
{
	m_msghandler->showWindow( p_name );
}

void MainWindow::event_newServerList(std::vector<std::string> p_servers)
{
	m_lastserverlist = p_servers;

	Glib::ustring temp;
	if(m_popup.get_active_text() != "")
		temp = m_popup.get_active_text();
	else
		temp = "Not Connected";

	m_dont_do_shit=true;
		m_popup.clear_items();
		m_popup.append_text("Not Connected");

		std::cout << "MainWindow: Received new server list: ";
		for (int i=0;i<m_lastserverlist.size();i+=2)
		{
			m_popup.append_text(m_lastserverlist.at(i));
			std::cout << m_lastserverlist.at(i+1) << " ";
		}
		std::cout << std::endl;

		m_popup.set_active_text(temp);
	m_dont_do_shit=false;
}



void MainWindow::event_textMessage( std::string sender, std::string message) 
{
	m_msghandler->handleMessage(sender, message);
}


void MainWindow::event_newChannelList(std::vector<std::string> p_channels)
{
	print_me("Recieved new channel list");

	m_channellist->giveChannelList(p_channels);
}

void MainWindow::event_connected(std::string p_ip, std::string p_name)
{
	print_me("MainWindow: Successfully connected to "+p_ip+" as "+p_name);

	*m_nameptr = p_name;
	//use p_name for menu?

	//m_events->to_network->pushEvent( UIEvent ( "JOINCHANNEL", "__lobby__" ) );
}

void MainWindow::event_errorConnecting(std::string str0)
{
	got_here();
		//if (p_err == "ERR_IS_PASSWORD")
		//{
		//	//triggerPassEntry();
		//}
		//else
		//{

	std::string kaka = str0;

	spawnErrorDialog("Error connecting", kaka);
	got_here();	
		//}
	m_dont_do_shit=true;
	m_popup.set_active_text("Not Connected");
	m_dont_do_shit=false;	
	m_channellist->notifyDisconnected();
}

void MainWindow::event_connectionLost(std::string p_address)
{
	spawnErrorDialog("Connection lost", p_address);
	m_dont_do_shit=true;
	m_popup.set_active_text("Not Connected");	
	m_dont_do_shit=false;
	m_channellist->notifyDisconnected();
}

void MainWindow::event_disconnected()
{
	m_channellist->notifyDisconnected();
}

void MainWindow::on_button_pressed()
{
	m_events->to_network->pushEvent( UIEvent ( "I_START_TALKING" ) );
}

void MainWindow::on_button_released()
{
	m_events->to_network->pushEvent( UIEvent ( "I_STOP_TALKING" ) );
}

void MainWindow::on_popup_changed()
{
	if(!m_dont_do_shit)
	{
		Glib::ustring text = m_popup.get_active_text();

		if (text != "Not Connected")
		{
			//m_events->to_network->pushEvent( UIEvent ( "DISCONNECT" ) );
			m_events->to_network->pushEvent( UIEvent ( "CONNECTTOSERVER", getServerIp(text), m_newname, "kaka" ) );
		} else {
			m_events->to_network->pushEvent( UIEvent ( "DISCONNECT" ) );
		}
	}
}

void MainWindow::spawnErrorDialog(std::string p_titlebar,std::string p_textbody)
{
	std::cout << "MainWindow: " << p_titlebar << " " << p_textbody << std::endl;

	Gtk::MessageDialog kaka(*this,p_titlebar,false,Gtk::MESSAGE_ERROR);
	kaka.set_secondary_text(p_textbody);
	kaka.run();
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



void MainWindow::toggleVisibility()
{
	if(!is_visible())
	  	show();
	else
		hide();
}



