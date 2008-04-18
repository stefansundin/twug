#include "ChannelList.h"

ChannelList::ChannelList(Glib::RefPtr<Gtk::TreeStore> p_store, Gtk::TreeModelColumn<Glib::ustring>* p_column, UIEvents* p_events, std::string* p_nameptr) : Gtk::TreeView(p_store)
{
	m_store = p_store;
	m_column = p_column;
	m_events = p_events;
	m_nameptr = p_nameptr;

	append_column("Name", *m_column);	
	set_size_request(180,400);
}

void ChannelList::notifyDisconnected()
{
	m_store->clear();
}

bool ChannelList::on_button_press_event(GdkEventButton* p_evb)
{
	bool val = TreeView::on_button_press_event(p_evb);

	Gtk::TreeModel::iterator iter = get_selection()->get_selected();
	Glib::ustring temp = (*iter)[*m_column];
	//print_me("got selection value " + temp);

	if (p_evb->type == GDK_BUTTON_PRESS)
	{	
		if( (p_evb->button == 3) )
		{
			createChannelListMenu(temp,p_evb, whatIsThis(temp));
		}
	}

	return val;
}


void ChannelList::on_channelmenu_join()
{
	if (m_lastclicked == m_mychannel)	
		m_events->to_network->pushEvent( UIEvent ( "JOINCHANNEL", "__lobby__" ) );
	else
		m_events->to_network->pushEvent( UIEvent ( "JOINCHANNEL", m_lastclicked ) );
}

void ChannelList::on_personmenu_message()
{
	m_events->to_ui->pushEvent ( UIEvent ( "SHOW_MSG_WINDOW" , m_lastclicked ) );	
}

int ChannelList::whatIsThis(std::string name)
{
	for(int i=0; i<m_lastchannellist.size(); i++)
	{
		if (m_lastchannellist.at(i) == name)
		{
			if (i==0)
				return 1;
			else if (m_lastchannellist.at(i-1) == "--END--")
				return 1;
			else
				return 0;
		}
	}
	std::cout << "ChannelList: <DEBUG> Clicked item not a channel or nickname" << std::endl;	
	return 2;
}


void ChannelList::on_channelmenu_removeChannel()
{
		m_events->to_network->pushEvent( UIEvent ( "REMOVECHANNEL", m_lastclicked ) );
}

void ChannelList::on_backgroundmenu_newChannel()
{
	
		m_events->to_network->pushEvent( UIEvent ( "NEWCHANNEL", "kakachannel" ));
}


void ChannelList::createChannelListMenu(std::string p_entry, GdkEventButton* p_evb, unsigned int p_type)
{
	got_here();

	m_lastclicked = p_entry;

	m_menu.items().clear();

	if (p_type == 0) // Person menu
	{
		m_menu.items().push_back(Gtk::Menu_Helpers::MenuElem("_Message "+p_entry,
			sigc::mem_fun(*this,&ChannelList::on_personmenu_message) ));
	} else if (p_type == 1) // Channel menu
	{
		m_menu.items().push_back(Gtk::Menu_Helpers::MenuElem("_Join channel"+p_entry,
			sigc::mem_fun(*this,&ChannelList::on_channelmenu_join) ));
		m_menu.items().push_back(Gtk::Menu_Helpers::MenuElem("_Remove channel "+p_entry,
			sigc::mem_fun(*this,&ChannelList::on_channelmenu_removeChannel) ));
	} else if (p_type == 2) // Background menu
	{
		m_menu.items().push_back(Gtk::Menu_Helpers::MenuElem("_New channel",
			sigc::mem_fun(*this,&ChannelList::on_backgroundmenu_newChannel) ));
	}

	m_menu.popup(p_evb->button, p_evb->time);	
}


void ChannelList::giveChannelList(std::vector<std::string> channels)
{
	print_me("Recieved new channel list");

	m_lastchannellist = channels;

	m_store->clear();

	Gtk::TreeModel::iterator iter;
	Gtk::TreeModel::iterator child_iter;
	
	int i=0;
	std::string channame;

	for(;i<channels.size();i++)
	{
		channame = channels.at(i);

		if(channame != "__lobby__")
		{
			iter = m_store->append();
			(*iter)[*m_column] = channels.at(i);
		}

		i++;

		for(;channels.at(i)!="--END--";i++)
		{
			if(channame == "__lobby__")
			{	
				iter = m_store->append();
				(*iter)[*m_column] = channels.at(i);	
			} else {
				child_iter = m_store->append(iter->children());
				(*child_iter)[*m_column] = channels.at(i);
			}
			if ((*m_nameptr) == channels.at(i))
				m_mychannel = channame;		
		}
	}
	expand_all();
}

