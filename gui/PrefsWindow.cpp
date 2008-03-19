#include "PrefsWindow.h"

PrefsWindow::PrefsWindow(MainWindow* p_window)
{
	//m_icon = p_icon;
	m_window = p_window;

	set_title("Twug Preferences");
	set_border_width(10);

	Gnome::Conf::init();
	m_gconf = Gnome::Conf::Client::get_default_client();
	m_gconf->add_dir("/apps/twug", Gnome::Conf::CLIENT_PRELOAD_RECURSIVE);
 	m_gconf->signal_value_changed().connect(sigc::mem_fun(*this,&PrefsWindow::onGConfChanged));


	Gtk::VBox *vbox = new Gtk::VBox;
	add(*vbox);

	Gtk::Label *label = new Gtk::Label("Server list:");
	vbox->add(*label); 

	m_columns = new Columns();
	m_serverstore = Gtk::ListStore::create(*m_columns);
	m_serverlist = new Gtk::TreeView(m_serverstore);
	vbox->add(*m_serverlist);
	m_serverlist->append_column_editable("Name", m_columns->name);
	m_serverlist->append_column_editable("Address", m_columns->ip);

	//m_serverlist->get_selection()->signal_changed().connect(sigc::mem_fun(*this,&PrefsWindow::on_tree_changed));
	//m_serverlist->signal_columns_changed().connect(sigc::mem_fun(*this,&PrefsWindow::on_tree_changed));

	Gtk::CellRendererText* kaka = dynamic_cast<Gtk::CellRendererText*>(m_serverlist->get_column_cell_renderer(1));
	//kaka->signal_edited().connect(sigc::mem_fun(*this,&PrefsWindow::on_tree_changed));




	m_chkbtn_blinking = new Gtk::CheckButton("Use blinking tray icon");
	vbox->add(*m_chkbtn_blinking);
	m_chkbtn_blinking->signal_clicked().connect(sigc::mem_fun(*this,&PrefsWindow::on_button_clicked));

	show_all();
}

void PrefsWindow::on_tree_changed()
{
	std::cout << "PrefsWindow: Tree changed\n";
}

std::vector<Glib::ustring> PrefsWindow::getServerList()
{
	std::cout << "PrefsWindow: Fetching server list from GConf" << std::endl;

	std::vector<Glib::ustring> servers;

	std::vector<Gnome::Conf::Entry> entries = m_gconf->all_entries("/apps/twug/servers");

	for(int i=0;i<entries.size();i++)
	{
		servers.push_back(entries.at(i).get_key().substr(19));
		servers.push_back(entries.at(i).get_value().get_string());
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

void PrefsWindow::loadSettings()
{
	std::cout << "PrefsWindow: Loading settings...\n";
	reloadServers();

	m_chkbtn_blinking->set_active( m_gconf->get_bool("/apps/twug/use_blinking_trayicon") );
	
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

  //m_icon->set_blinking( active );
  m_gconf->set("/apps/twug/use_blinking_trayicon",active);
}

void PrefsWindow::toggleVisibility()
{
	std::cout << "toggling" << std::endl;
	if(!is_visible())
	  	show();
	else
		hide();
}
