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

	setup_channelList();

	m_button.set_border_width(5);
	m_button.set_label("Talk button");


	Gtk::Button* addbutton = new Gtk::Button(Gtk::Stock::ADD);
	addbutton->signal_clicked().connect(
		sigc::mem_fun(*this,&MainWindow::on_addbutton_clicked));

	Gtk::Button* broadcastbutton = new Gtk::Button(Gtk::Stock::COPY);
	broadcastbutton->set_label("Broadcast");
	broadcastbutton->signal_clicked().connect(
		sigc::mem_fun(*this,&MainWindow::on_broadcastbutton_clicked));

	Gtk::HBox* hbox = new Gtk::HBox();
	hbox->add(*addbutton);
	hbox->add(*broadcastbutton);

	vbox->add(m_popup);
	vbox->add(m_button);
	vbox->add(*m_channellist);
	vbox->add(*hbox);
	add(*vbox);
	show_all();


	m_popup.signal_changed().connect(
		sigc::mem_fun(*this,&MainWindow::on_popup_changed) );
	m_button.signal_pressed().connect(
		sigc::mem_fun(*this,&MainWindow::on_button_pressed));
	m_button.signal_released().connect(
		sigc::mem_fun(*this,&MainWindow::on_button_released));

	//setup add channel dialog
	m_kaka = new Gtk::MessageDialog(*this,"Add new channel",false,Gtk::MESSAGE_QUESTION,Gtk::BUTTONS_OK_CANCEL);
	m_kaka->set_secondary_text("What do you want to call the new channel?");
	m_entry = new Gtk::Entry(); 
	m_kaka->get_vbox()->add(*m_entry);
	m_entry->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::hack));

	//setup broadcast dialog
	m_kaka2 = new Gtk::MessageDialog(*this,"Broadcast message",false,Gtk::MESSAGE_QUESTION,Gtk::BUTTONS_OK_CANCEL);
	m_kaka2->set_secondary_text("Please type your message below.");

	m_entry2 = new Gtk::Entry(); 
	m_kaka2->get_vbox()->add(*m_entry2);
	m_entry2->signal_activate().connect(sigc::mem_fun(*this,&MainWindow::hack2));
}

void MainWindow::hack()
{
	m_kaka->response(Gtk::RESPONSE_OK);
	m_kaka->hide();
}

void MainWindow::hack2()
{
	m_kaka2->response(Gtk::RESPONSE_OK);
	m_kaka2->hide();
}


void MainWindow::on_addbutton_clicked()
{
	m_entry->set_text("");
	m_kaka->set_focus(*m_entry);
	m_kaka->show_all();

	int answer = m_kaka->run();

	if ( m_entry->get_text() != "" && answer == Gtk::RESPONSE_OK )
	{
		m_events->to_network->pushEvent( UIEvent ( "NEWCHANNEL", m_entry->get_text()  ) );
	}

	m_kaka->hide();
}

void MainWindow::on_broadcastbutton_clicked()
{
	m_entry2->set_text("");
	m_kaka2->set_focus(*m_entry2);
	m_kaka2->show_all();

	int answer = m_kaka2->run();

	if ( m_entry2->get_text() != "" && answer == Gtk::RESPONSE_OK )
	{
		m_events->to_network->pushEvent( UIEvent ( "BROADCAST_TEXT", m_entry2->get_text()  ) );
	}

	m_kaka2->hide();
}

void MainWindow::setup_channelList()
{
	Gtk::TreeModelColumn<Glib::ustring>* namecolumn = new Gtk::TreeModelColumn<Glib::ustring>();
	Gtk::TreeModelColumnRecord* columns = new Gtk::TreeModelColumnRecord();
	columns->add(*namecolumn);
        Glib::RefPtr<Gtk::TreeStore> treestore = Gtk::TreeStore::create(*columns);
        m_channellist = new ChannelList(treestore,namecolumn,m_events,m_nameptr);
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

void MainWindow::event_connecting(std::string p_server)
{
	//m_channellist->putMessage("Connecting to " + p_server + "...");
	m_channellist->putMessage("Connecting...");
}

void MainWindow::event_loggingIn(std::string p_server)
{
	//m_channellist->putMessage("Connecting to " + p_server + "...");
	m_channellist->putMessage("Logging in...");
}


void MainWindow::event_connectionLost(std::string p_address)
{
	if (m_popup.get_active_text() != "Not Connected")
	{
		spawnErrorDialog("Connection lost", p_address);
		m_dont_do_shit=true;
		m_popup.set_active_text("Not Connected");	
		m_dont_do_shit=false;
		m_channellist->notifyDisconnected();
	}
}

void MainWindow::event_disconnected()
{
	m_channellist->notifyDisconnected();
	m_dont_do_shit=true;
	m_popup.set_active_text("Not Connected");	
	m_dont_do_shit=false;
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



