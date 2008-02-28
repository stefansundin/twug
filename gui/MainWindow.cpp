#include "MainWindow.h"

MainWindow::MainWindow(Handler* p_handler)
{
	m_dontdoshit=false;
	m_handler = p_handler;

	set_title("Twug UI 0.3");
	//set_border_width(10);
	Gtk::VBox* vbox = new Gtk::VBox();
 	add(*vbox);

	//add popup menu
	vbox->add(m_popup);
	m_popup.signal_changed().connect(sigc::mem_fun(*this,
              &MainWindow::on_popup_changed) );

	//add button
	vbox->add(m_button);
	m_button.set_label("Talk button");
	m_button.signal_pressed().connect(sigc::mem_fun(*this,&MainWindow::on_button_pressed));
	m_button.signal_released().connect(sigc::mem_fun(*this,&MainWindow::on_button_released));

	//m_button.set_size_request(m_button.get_width(), m_button.get_height() );

	//add treeview
	m_columns = new mwColumns();
	m_treestore = Gtk::TreeStore::create(*m_columns);
	m_treeview = new Gtk::TreeView(m_treestore);
	vbox->add(*m_treeview);
	m_treeview->append_column("Name", m_columns->name);

	show_all();

	std::cout << m_button.get_width() << " " <<  m_button.get_height() << std::endl;
	m_button.set_size_request(m_button.get_width(), m_button.get_height() );
	m_treeview->set_size_request(180,400);
}


MainWindow::~MainWindow()
{
}

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

	for(int i=0;i<channels.size();i++)
	{
		if(channels.at(i) == "__lobby__")
		{
			std::vector<std::string> channelMembers = m_handler->getChannelMembers( channels.at(i) );
			if(!channelMembers.empty())
			{
				for(int a=0;a<channelMembers.size();a++)
				{
					iter = m_treestore->append();
					(*iter)[m_columns->name] = channelMembers.at(a);
				}
			}
		} else {
			iter = m_treestore->append();
			(*iter)[m_columns->name] = channels.at(i);

			std::vector<std::string> channelMembers = m_handler->getChannelMembers( channels.at(i) );
			if(!channelMembers.empty())
			{
				Gtk::TreeModel::iterator child_iter;
				for(int a=0;a<channelMembers.size();a++)
				{
				child_iter = m_treestore->append(iter->children());
				(*child_iter)[m_columns->name] = channelMembers.at(a);
				}
			}
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
