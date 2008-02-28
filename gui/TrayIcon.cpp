#include "TrayIcon.h"


TrayIcon::TrayIcon(MainWindow* p_window) : m_menu(0)
{
	m_window = p_window;
	set(Gtk::Stock::NEW);
	signal_activate().connect(sigc::mem_fun(*this,&TrayIcon::on_clicked));
	signal_popup_menu().connect(sigc::mem_fun(*this,&TrayIcon::on_popup));

	// -- start popup initialization
		m_actiongroup = Gtk::ActionGroup::create();
		m_actiongroup->add(Gtk::Action::create("Popup", "Popup"));
		m_actiongroup->add(Gtk::Action::create("Quit", Gtk::Stock::QUIT), sigc::mem_fun(*this, &TrayIcon::on_menu_quit) );

		Glib::ustring ui = "<ui>"
		"<popup name='Popup'>"
		"	<menuitem action='Quit' />"
		"</popup>"
		"</ui>";

		Glib::RefPtr<Gtk::UIManager> UIManager = Gtk::UIManager::create();
		UIManager->insert_action_group(m_actiongroup);
		UIManager->add_ui_from_string(ui);

		m_menu = dynamic_cast<Gtk::Menu*>(UIManager->get_widget("/Popup"));
	// -- end popup initialization
}
/*
	right-click menu:
		Hide/show channel list(BOLD,SAME AS LEFT CLICK)
		-------
		About Twug...
		Quit
*/

TrayIcon::~TrayIcon()
{
}

void TrayIcon::on_menu_quit()
{
	std::cout << "qui\n";
}

void TrayIcon::on_clicked()
{
	std::cout << "clicked tray icon" << std::endl;
	
	m_window->toggleVisibility();
}

void TrayIcon::on_popup(const unsigned int& btn, const unsigned int& time)
{
	std::cout << "open menu\n";
	//popup_menu_at_position (Menu& menu, guint  	button, guint32 activate_time)	
	popup_menu_at_position (*m_menu, btn, time);	
}
