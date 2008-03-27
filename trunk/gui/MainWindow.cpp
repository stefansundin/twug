#include "MainWindow.h"

MainWindow::MainWindow(Handler* p_handler)
{
	m_nameptr = new std::string();

	set_title("Twug");
	set_default_icon_from_file("/usr/share/pixmaps/twug.png");
	//set_border_width(10);

	m_handler = p_handler;

	m_dontdoshit = false;
	m_autoopen = false;

	m_msghandler = new MessageHandler(m_handler,m_nameptr);

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


MainWindow::~MainWindow()
{
}

void MainWindow::giveServers(std::vector<Glib::ustring> p_servers)
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

void MainWindow::on_button_pressed()
{
	m_handler->iStartTalking();
}

void MainWindow::on_button_released()
{
	m_handler->iStopTalking();
}

void MainWindow::on_popup_changed()
{
	if(!m_dontdoshit)
	{
		Glib::ustring text = m_popup.get_active_text();

		if (text != "Not Connected")
		{
			m_handler->disconnect();
			m_handler->connectToServer( getServerIp(text), m_newname, "kaka" );
			
		} else {
			m_handler->disconnect();
			m_treestore->clear();
		}
	}
}

void MainWindow::connectedAs(std::string p_ip, std::string p_name)
{
	*m_nameptr = p_name;

	m_handler->joinChannel("__lobby__");
}

void MainWindow::connectionError(bool p_type, std::string p_err)
{
	if (!p_type) // if error during connection process
	{
		//if (p_err == "ERR_IS_PASSWORD")
		//{
		//	//triggerPassEntry();
		//}
		//else
		//{
			spawnErrorDialog("Error connecting", p_err);			
		//}
	} else { // if unexpected network connection lost
		spawnErrorDialog("Connection lost", p_err);
		
	}
	m_popup.set_active_text("Not Connected");	
}

void MainWindow::spawnErrorDialog(std::string p_titlebar,std::string p_textbody)
{
	//temp
	std::cout << "MainWindow: " << p_titlebar << " " << p_textbody << std::endl;
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
	m_treestore->clear();

	std::vector<std::string> channels = m_handler->getChannels();

	Gtk::TreeModel::iterator iter;
	Gtk::TreeModel::iterator child_iter;

	std::vector<std::string> channelMembers;
	int a;

	for(int i=0;i<channels.size();i++)
	{
		if(channels.at(i) == "__lobby__")
		{
			channelMembers = m_handler->getChannelMembers( channels.at(i) );
			for(a=0;a<channelMembers.size();a++)
			{
				iter = m_treestore->append();
				(*iter)[m_columns->name] = channelMembers.at(a);
				if (m_autoopen) 
					m_msghandler->showWindow(channelMembers.at(a));
			}
		} else {
			iter = m_treestore->append();
			(*iter)[m_columns->name] = channels.at(i);

			channelMembers = m_handler->getChannelMembers( channels.at(i) );

			for(a=0;a<channelMembers.size();a++)
			{
				child_iter = m_treestore->append(iter->children());
				(*child_iter)[m_columns->name] = channelMembers.at(a);
				if (m_autoopen) 
					m_msghandler->showWindow(channelMembers.at(a));
			}
		}
	}
	m_treeview->expand_all();
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
	std::vector<std::string> kaka = m_handler->getChannelMembers( getSelectionValue());
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
		m_handler->joinChannel ( "__lobby__");
	else
		m_handler->joinChannel( getSelectionValue() );
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
	std::vector<std::string> channels = m_handler->getChannels();
	
	for(int i=0; i<channels.size(); i++)
	{
		if (channels.at(i) == name)
		{
			return true;
		}
	}	
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
