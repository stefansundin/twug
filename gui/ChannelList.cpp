#include "ChannelList.h"

ChannelList::ChannelList(Glib::RefPtr<Gtk::TreeStore> p_store, Gtk::TreeModelColumn<Glib::ustring>* p_column, UIEvents* p_events, std::string* p_nameptr) : Gtk::TreeView(p_store)
{
	m_store = p_store;
	m_column = p_column;
	m_events = p_events;
	m_nameptr = p_nameptr;

	append_column("Name", *m_column);	
	set_size_request(180,400);
	set_headers_visible(false);
}

void ChannelList::notifyDisconnected()
{
	m_store->clear();
}

void ChannelList::putMessage(std::string p_msg)
{
	m_store->clear();
	Gtk::TreeModel::iterator iter;

	iter = m_store->append();
	(*iter)[*m_column] = p_msg;
}

bool ChannelList::on_button_press_event(GdkEventButton* p_evb)
{
	bool val = TreeView::on_button_press_event(p_evb);

	Gtk::TreeModel::iterator iter = get_selection()->get_selected();
	Glib::ustring temp = (*iter)[*m_column];
	//print_me("got selection value " + temp);
	m_lastclicked = temp;
	if (p_evb->type == GDK_BUTTON_PRESS && p_evb->button == 3)
	{
		createChannelListMenu(temp);
		m_menu.popup(p_evb->button, p_evb->time);	
	} else if (p_evb->type == GDK_2BUTTON_PRESS)
	{
		int type = whatIsThis(temp);
		if (type == 0) //person
		{
			on_personmenu_message();
		} else if (type == 1 || type == 4) //channel
		{
			on_channelmenu_join();
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
	if (name == "<empty>")
		return 3;

	for(int i=0; i<m_lastchannellist.size(); i++)
	{
		if (m_lastchannellist.at(i) == name)
		{
			if (i==0 || m_lastchannellist.at(i-1) == "--END--")
			{
				if (m_mychannel == m_lastchannellist.at(i))
					return 4;
				else
					return 1;
			}
			else
			{
				return 0;
			}
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


void ChannelList::createChannelListMenu(std::string p_entry)
{
	got_here();

	m_menu.items().clear();

	int p_type = whatIsThis(p_entry);

	if (p_type == 0) // Person menu
	{
		m_menu.items().push_back(Gtk::Menu_Helpers::MenuElem("_Message "+p_entry,
			sigc::mem_fun(*this,&ChannelList::on_personmenu_message) ));
	} else if (p_type == 1) // Channel menu
	{
		m_menu.items().push_back(Gtk::Menu_Helpers::MenuElem("_Join channel "+p_entry,
			sigc::mem_fun(*this,&ChannelList::on_channelmenu_join) ));
		m_menu.items().push_back(Gtk::Menu_Helpers::MenuElem("_Remove channel "+p_entry,
			sigc::mem_fun(*this,&ChannelList::on_channelmenu_removeChannel) ));
	} else if (p_type == 4) // My channel menu
	{
		m_menu.items().push_back(Gtk::Menu_Helpers::MenuElem("_Leave channel "+p_entry,
			sigc::mem_fun(*this,&ChannelList::on_channelmenu_join) ));
		m_menu.items().push_back(Gtk::Menu_Helpers::MenuElem("_Remove channel "+p_entry,
			sigc::mem_fun(*this,&ChannelList::on_channelmenu_removeChannel) ));
	}


}


void ChannelList::giveChannelList(std::vector<std::string> channels)
{
	print_me("Received new channel list");

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
		
		if (channels.at(i) == "--END--" && channels.at(i-1) != "__lobby__")
		{
			child_iter = m_store->append(iter->children());
			(*child_iter)[*m_column] = "<empty>";
		} else {
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
	}
	expand_all();
}

