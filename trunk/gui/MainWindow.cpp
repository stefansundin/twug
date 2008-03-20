#include "MainWindow.h"

MainWindow::MainWindow(Handler* p_handler)
{
	m_autoopen = false;

	m_msghandler = new MessageHandler(m_handler);


	m_dontdoshit=false;
	m_handler = p_handler;

	set_title("Twug");
	//set_border_width(10);
	Gtk::VBox* vbox = new Gtk::VBox();
 	add(*vbox);

	//add popup menu
	vbox->add(m_popup);
	m_popup.signal_changed().connect(sigc::mem_fun(*this,
              &MainWindow::on_popup_changed) );

	//add button
	vbox->add(m_button);
	m_button.set_label("Temp test button");
	m_button.signal_pressed().connect(sigc::mem_fun(*this,&MainWindow::on_button_pressed));
	m_button.signal_released().connect(sigc::mem_fun(*this,&MainWindow::on_button_released));

	//add treeview
	m_menu = new Gtk::Menu();

	m_menu->items().push_back(Gtk::Menu_Helpers::MenuElem("_Message", sigc::mem_fun(
		*this,&MainWindow::on_menuitem_clicked) ));

	m_columns = new mwColumns();
	m_treestore = Gtk::TreeStore::create(*m_columns);
	m_treeview = new Gtk::TreeView(m_treestore);
	vbox->add(*m_treeview);
	m_treeview->append_column("Name", m_columns->name);

	show_all();


	m_treeview->set_size_request(180,400);

	m_button.set_border_width(5);

	m_treeview->signal_button_press_event().connect_notify(sigc::mem_fun(*this,&MainWindow::on_person_clicked));
}


MainWindow::~MainWindow()
{
}

void MainWindow::giveServers(std::vector<Glib::ustring> p_servers)
{
//todo: reconnect if ip for current server changed(?)
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
	//std::cout << "button pressed" << std::endl;
	m_handler->iStartTalking();
}

void MainWindow::on_button_released()
{
	//std::cout << "button released" << std::endl;
	m_handler->iStopTalking();
	std::string kaka = "Basse";
	std::string kaka2 = "Hejsan";
	m_msghandler->handleMessage(kaka,kaka2); 
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
			m_treestore->clear();
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
			if(!channelMembers.empty())
			{
				for(a=0;a<channelMembers.size();a++)
				{
					iter = m_treestore->append();
					(*iter)[m_columns->name] = channelMembers.at(a);
					if (m_autoopen) 
						m_msghandler->showWindow(channelMembers.at(a));
				}
			}
		} else {
			iter = m_treestore->append();
			(*iter)[m_columns->name] = channels.at(i);

			channelMembers = m_handler->getChannelMembers( channels.at(i) );
			if(!channelMembers.empty())
			{
				for(a=0;a<channelMembers.size();a++)
				{
				child_iter = m_treestore->append(iter->children());
				(*child_iter)[m_columns->name] = channelMembers.at(a);
				if (m_autoopen) 
					m_msghandler->showWindow(channelMembers.at(a));
				}
			}
		}
	}
}

void MainWindow::toggleVisibility()
{
	//std::cout << "toggling" << std::endl;
	if(!is_visible())
	  	show();
	else
		hide();
}


void MainWindow::on_menuitem_clicked()
{
	m_msghandler->showWindow( getSelectionValue() );	
}

Glib::ustring MainWindow::getSelectionValue()
{
	Gtk::TreeModel::iterator iter = m_treeview->get_selection()->get_selected();
	Glib::ustring temp = (*iter)[m_columns->name];
	return temp; 
}


void MainWindow::on_person_clicked(GdkEventButton* evb)
{
	if( (evb->type == GDK_BUTTON_PRESS) && (evb->button == 3) )
	{
		m_menu->popup(evb->button,evb->time);
	}	
}


void MainWindow::set_autoopen(bool active)
{
	std::cout << "Setting m_autoopen to " << active << std::endl;
	m_autoopen = active;	
}
