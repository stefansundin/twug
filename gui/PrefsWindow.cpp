#include "PrefsWindow.h"

PrefsWindow::PrefsWindow(UIEvents* p_events)
{
	m_events = p_events;

	set_title("Twug Preferences");
	set_border_width(10);

	Gtk::VBox *vbox = new Gtk::VBox;
	Gtk::Label *label = new Gtk::Label("Server list:");
	m_columns = new Columns();
	m_serverstore = Gtk::ListStore::create(*m_columns);
	m_serverlist = new Gtk::TreeView(m_serverstore);
	

	m_serverlist->append_column_editable("Name", m_columns->name);
	m_serverlist->append_column_editable("Address", m_columns->ip);
	m_serverlist->set_size_request(300,230);

	Gtk::HBox* buttonbox = new Gtk::HBox();

	m_buttonAdd.set_label("Add");
	m_buttonRemove.set_label("Remove");
	
	buttonbox->add(m_buttonAdd);
	buttonbox->add(m_buttonRemove);
	

	Gtk::HBox* namebox = new Gtk::HBox();
	Gtk::Label* label2 = new Gtk::Label("Nickname: ");
	m_nameEntry = new Gtk::Entry();
	
	namebox->add(*label2);
	namebox->add(*m_nameEntry);

	vbox->add(*namebox);
	vbox->add(*label); 
	vbox->add(*m_serverlist);
	vbox->add(*buttonbox);
	
	vbox->show_all();
	add(*vbox);

	Gtk::CellRendererText* temp;

	temp = dynamic_cast<Gtk::CellRendererText*>(m_serverlist->get_column_cell_renderer(0));
	temp->signal_edited().connect(
		sigc::mem_fun(*this,&PrefsWindow::on_tree_changed));

	temp = dynamic_cast<Gtk::CellRendererText*>(m_serverlist->get_column_cell_renderer(1));
	temp->signal_edited().connect(
		sigc::mem_fun(*this,&PrefsWindow::on_tree_changed));


	m_buttonAdd.signal_clicked().connect(
		sigc::mem_fun(*this,&PrefsWindow::on_buttonAdd_clicked));
	m_buttonRemove.signal_clicked().connect(
		sigc::mem_fun(*this,&PrefsWindow::on_buttonRemove_clicked));	

	m_nameEntry->signal_changed().connect(
		sigc::mem_fun(*this,&PrefsWindow::on_name_changed));	




//new stuff
	m_nameEntry->set_text( m_settings.getNick() );
	
	std::vector<std::string> servers = m_settings.getServerList();

	Gtk::TreeModel::iterator iter;

	for (int i=0;i<servers.size();i+=2)
	{
		iter = m_serverstore->append();
		(*iter)[m_columns->name] = servers.at(i);
		(*iter)[m_columns->ip] = servers.at(i+1);		
	}

	saveServerList();
//end new stuff
}


void PrefsWindow::on_name_changed()
{
	std::string name = m_nameEntry->get_text();

	m_settings.setNick ( m_nameEntry->get_text() );
	m_events->to_ui->pushEvent( UIEvent("NEW_NEWNAME", name) );
}

void PrefsWindow::on_tree_changed(const Glib::ustring&, const Glib::ustring&)
{
	std::cout << "PrefsWindow: Tree changed\n";
	saveServerList();
}

void PrefsWindow::saveServerList()
{
	std::vector<std::string> temp;

	Gtk::TreeModel::Children::iterator iter;
	for(iter = m_serverstore->children().begin();
		iter != m_serverstore->children().end();
			iter++)
	{
		Glib::ustring name = (*iter)[m_columns->name];
		Glib::ustring ip = (*iter)[m_columns->ip];	
		
   		temp.push_back(name);
    		temp.push_back(ip);
	}


	m_settings.setServerList(temp);	
	m_events->to_ui->pushEvent( UIEvent("NEW_SERVER_LIST", temp) );
}

void PrefsWindow::on_buttonAdd_clicked()
{
	Gtk::TreeModel::iterator iter;
	
	iter = m_serverstore->append();
	(*iter)[m_columns->name] = "Server";
	(*iter)[m_columns->ip] = "0.0.0.0";

	saveServerList();
}

void PrefsWindow::on_buttonRemove_clicked()
{
	Gtk::TreeModel::iterator iter = m_serverlist->get_selection()->get_selected();
	
	m_serverstore->erase(iter);

	saveServerList();
}




void PrefsWindow::toggleVisibility()
{
	if(!is_visible())
	  	show();
	else
		hide();
}
