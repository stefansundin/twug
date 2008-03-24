#include "PrefsWindow.h"

PrefsWindow::PrefsWindow(MainWindow* p_window)
{
	m_window = p_window;

	set_title("Twug Preferences");
	set_border_width(10);

	Gnome::Conf::init();
	m_gconf = Gnome::Conf::Client::get_default_client();
	m_gconf->add_dir("/apps/twug", Gnome::Conf::CLIENT_PRELOAD_RECURSIVE);
 	


	Gtk::VBox *vbox = new Gtk::VBox;
	Gtk::Label *label = new Gtk::Label("Server list:");
	m_chkbtn_blinking = new Gtk::CheckButton(
		"Open chat windows with everyone upon connected :)");
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

	vbox->add(*label); 
	vbox->add(*m_serverlist);
	vbox->add(*buttonbox);
	vbox->add(*m_chkbtn_blinking);
	vbox->add(*namebox);	
	vbox->show_all();
	add(*vbox);

	Gtk::CellRendererText* temp;

	temp = dynamic_cast<Gtk::CellRendererText*>(m_serverlist->get_column_cell_renderer(0));
	temp->signal_edited().connect(
		sigc::mem_fun(*this,&PrefsWindow::on_tree_changed));

	temp = dynamic_cast<Gtk::CellRendererText*>(m_serverlist->get_column_cell_renderer(1));
	temp->signal_edited().connect(
		sigc::mem_fun(*this,&PrefsWindow::on_tree_changed));


	m_gconf->signal_value_changed().connect(
		sigc::mem_fun(*this,&PrefsWindow::onGConfChanged));

	m_buttonAdd.signal_clicked().connect(
		sigc::mem_fun(*this,&PrefsWindow::on_buttonAdd_clicked));
	m_buttonRemove.signal_clicked().connect(
		sigc::mem_fun(*this,&PrefsWindow::on_buttonRemove_clicked));

	m_chkbtn_blinking->signal_clicked().connect(
		sigc::mem_fun(*this,&PrefsWindow::on_button_clicked));	

	m_nameEntry->signal_changed().connect(
		sigc::mem_fun(*this,&PrefsWindow::on_name_changed));	
}


void PrefsWindow::on_name_changed()
{
	std::string name = m_nameEntry->get_text();

	m_gconf->set("/apps/twug/nickname",name);	
	m_window->m_newname=name;	
}

void PrefsWindow::on_tree_changed(const Glib::ustring&, const Glib::ustring&)
{
	std::cout << "PrefsWindow: Tree changed\n";
	saveServerList();
}


std::vector<Glib::ustring> PrefsWindow::getServerList()
{
	std::cout << "PrefsWindow: Fetching server list from GConf" << std::endl;

	std::vector<Glib::ustring> servers;

	std::vector<Gnome::Conf::Entry> entries = m_gconf->all_entries("/apps/twug/servers");

	for(int i=0;i<entries.size();i++)
	{
		if (entries.at(i).get_value().get_type() != Gnome::Conf::VALUE_INVALID)
		{
			servers.push_back(entries.at(i).get_key().substr(19));
			servers.push_back(entries.at(i).get_value().get_string());
		}
	}
	return servers;
}

void PrefsWindow::reloadServers()
{
	m_serverstore->clear();

	std::vector<Glib::ustring> servers = getServerList();

	Gtk::TreeModel::iterator iter;
	
	for(int i=0;i<servers.size();i+=2)
	{
		iter = m_serverstore->append();
		(*iter)[m_columns->name] = servers.at(i);
		(*iter)[m_columns->ip] = servers.at(i+1);
	}

	m_window->giveServers(servers);
}

void PrefsWindow::saveServerList()
{
	std::vector<Gnome::Conf::Entry> entries = m_gconf->all_entries("/apps/twug/servers");
	
	for(int i=0;i<entries.size();i++)
	{
		m_gconf->unset ( entries.at(i).get_key() );
	}

	
	Gtk::TreeModel::Children::iterator iter;
	for(iter = m_serverstore->children().begin();
		iter != m_serverstore->children().end();
			iter++)
	{
		m_gconf->set( "/apps/twug/servers/" + (*iter)[m_columns->name],
	(*iter)[m_columns->ip] );	
	}
	
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

void PrefsWindow::loadSettings()
{
	std::cout << "PrefsWindow: Loading settings...\n";
	reloadServers();

	m_chkbtn_blinking->set_active( m_gconf->get_bool("/apps/twug/autoopen_enabled") );

	m_nameEntry->set_text( m_gconf->get_string("/apps/twug/nickname") ) ;
	
	std::cout << "PrefsWindow: Loaded settings\n";
}


void PrefsWindow::onGConfChanged(const Glib::ustring&, const Gnome::Conf::Value&)
{
	loadSettings();
}


PrefsWindow::~PrefsWindow()
{
}

void PrefsWindow::on_button_clicked()
{
	bool active = m_chkbtn_blinking->get_active();

	m_gconf->set("/apps/twug/autoopen_enabled",active);	
	m_window->m_autoopen=active;	
}

void PrefsWindow::toggleVisibility()
{
	if(!is_visible())
	  	show();
	else
		hide();
}
