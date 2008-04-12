#include "MainWindow.h"

MainWindow::MainWindow(UIEvents* p_events)
{
	m_events = p_events;

	m_nameptr = new std::string();

	set_title("Twug");
	set_default_icon_from_file("/usr/share/pixmaps/twug.png");
	//set_border_width(10);


	m_dontdoshit = false;
	m_autoopen = false;

	m_msghandler = new MessageHandler(m_events,m_nameptr);

	Gtk::VBox* vbox = new Gtk::VBox();
	m_personmenu = new Gtk::Menu();
	m_channelmenu = new Gtk::Menu();
	m_columns = new mwColumns();
	m_treestore = Gtk::TreeStore::create(*m_columns);
	m_treeview = new Gtk::TreeView(m_treestore);

	m_treeview->append_column("Name", m_columns->name);	
	m_treeview->set_size_request(180,400);
	m_button.set_border_width(5);
	m_button.set_label("Talk button");

	vbox->add(m_popup);
	vbox->add(m_button);
	vbox->add(*m_treeview);
	add(*vbox);
	show_all();

	m_personmenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Message",
		sigc::mem_fun(*this,&MainWindow::on_personmenu_message) ));

	m_channelmenu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Join Channel",
		sigc::mem_fun(*this,&MainWindow::on_channelmenu_join) ));

	m_popup.signal_changed().connect(
		sigc::mem_fun(*this,&MainWindow::on_popup_changed) );
	m_button.signal_pressed().connect(
		sigc::mem_fun(*this,&MainWindow::on_button_pressed));
	m_button.signal_released().connect(
		sigc::mem_fun(*this,&MainWindow::on_button_released));

	m_treeview->signal_button_press_event().connect_notify(
		sigc::mem_fun(*this,&MainWindow::on_treeview_clicked));
}


void MainWindow::event_newNewName (std::string p_name)
{
	m_newname = p_name;
}

MainWindow::~MainWindow()
{
}

void MainWindow::event_newServerList(std::vector<std::string> p_servers)
{
	m_lastserverlist = p_servers;

	Glib::ustring temp;
	if(m_popup.get_active_text() != "")
		temp = m_popup.get_active_text();
	else
		temp = "Not Connected";

	m_dontdoshit=true;
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
	m_dontdoshit=false;
}



void MainWindow::event_textMessage( std::string sender, std::string message) 
{
	m_msghandler->handleMessage(sender, message);
}


void MainWindow::event_newChannelList(std::vector<std::string> channels)
{
	std::cout << "MainWindow: recieved new channel list" << std::endl;

	m_lastchannellist = channels;

	m_treestore->clear();

	Gtk::TreeModel::iterator iter;
	Gtk::TreeModel::iterator child_iter;
	
	int i=0;
	std::string channame;

	for(;i<channels.size();i++)
	{
		channame = channels.at(i);

		if(channame != "__lobby__")
		{
			iter = m_treestore->append();
			(*iter)[m_columns->name] = channels.at(i);
		}

		i++;

		for(;channels.at(i)!="--END--";i++)
		{
			if(channame == "__lobby__")
			{	
				iter = m_treestore->append();
				(*iter)[m_columns->name] = channels.at(i);	
			} else {
				child_iter = m_treestore->append(iter->children());
				(*child_iter)[m_columns->name] = channels.at(i);
			}
			if (m_autoopen) 
				m_msghandler->showWindow(channels.at(i));
		}
	}
	m_treeview->expand_all();
}

void MainWindow::event_connected(std::string p_ip, std::string p_name)
{
	print_me("MainWindow: Successfully connected to "+p_ip+" as "+p_name);

	*m_nameptr = p_name;
	//use p_name for menu?

	//m_events->to_network->pushEvent( UIEvent ( "JOINCHANNEL", "__lobby__" ) );
}

void MainWindow::event_errorConnecting(std::string str0, std::string str1, std::string str2)
{
		//if (p_err == "ERR_IS_PASSWORD")
		//{
		//	//triggerPassEntry();
		//}
		//else
		//{

	std::string kaka = str0 + str1 + str2;

			spawnErrorDialog("Error connecting", kaka);			
		//}
	m_popup.set_active_text("Not Connected");	
}

void MainWindow::event_connectionLost(std::string p_address)
{
	spawnErrorDialog("Connection lost", p_address);
	m_popup.set_active_text("Not Connected");	
}

void MainWindow::event_disconnected()
{
	m_treestore->clear();
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
	if(!m_dontdoshit)
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


void MainWindow::on_channelmenu_join()
{
	/*std::vector<std::string> kaka = ?????->getChannelMembers( getSelectionValue());
	bool found=0;
	for (int i=0;i<kaka.size();i++)
	{
		if (kaka.at(i) == *m_nameptr)
		{
			found=1;
			break;
		}
	}

	if (found)	
		m_events->to_network->pushEvent( UIEvent ( "JOINCHANNEL", "__lobby__" ) );

	else*/
		m_events->to_network->pushEvent( UIEvent ( "JOINCHANNEL", getSelectionValue() ) );
}

void MainWindow::on_personmenu_message()
{
	m_msghandler->showWindow( getSelectionValue() );	
}

Glib::ustring MainWindow::getSelectionValue()
{
	Gtk::TreeModel::iterator iter = m_treeview->get_selection()->get_selected();
	Glib::ustring temp = (*iter)[m_columns->name];
	return temp; 
}

bool MainWindow::isChannel(std::string name)
{
	for(int i=0; i<m_lastchannellist.size(); i++)
	{
		if (m_lastchannellist.at(i) == name)
		{
			if (i==0)
				return true;
			else if (m_lastchannellist.at(i-1) == "--END--")
				return true;
			else
				return false;
		}
	}
	std::cout << "ERROR: Not a channel or nickname" << std::endl;	
	return false;
}

void MainWindow::on_treeview_clicked(GdkEventButton* evb)
{
	if (evb->type == GDK_BUTTON_PRESS)	
		if( (evb->button == 3) )
		{
			if ( isChannel( getSelectionValue() ) )
			{
				m_channelmenu->popup(evb->button,evb->time);
			} else {
				m_personmenu->popup(evb->button,evb->time);
			}
		}	
}
