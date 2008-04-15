#include "MessageWindow.h"

MessageWindow::MessageWindow(std::string p_name, UIEvents* p_events, std::string* p_nameptr)
{
	m_nameptr = p_nameptr;

	m_events = p_events;
	m_name = p_name;

	set_border_width(10);
	set_title("Text conversation with " + m_name);

	Gtk::VBox* vbox = new Gtk::VBox();
	Gtk::HBox* hbox = new Gtk::HBox();
	m_scrolled = new Gtk::ScrolledWindow();	
	m_textview = new Gtk::TextView(); 
	m_entry = new Gtk::Entry(); 
	m_button = new Gtk::Button("Send");
	m_buffer = m_textview->get_buffer();

	m_textview->set_editable(false);
	m_textview->set_size_request(280,220);
	m_textview->set_wrap_mode(Gtk::WRAP_CHAR);
	m_scrolled->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);

	m_scrolled->add(*m_textview);
	vbox->add(*m_scrolled);

	hbox->add(*m_entry);	
	hbox->add(*m_button);
	vbox->add(*hbox);
	
	add(*vbox);

	show_all();

	m_button->signal_clicked().connect(sigc::mem_fun(*this,&MessageWindow::sendEntry));
	m_entry->signal_activate().connect(sigc::mem_fun(*this,&MessageWindow::sendEntry));

	//m_button->set_size_request(m_button->get_width(), m_button->get_height() );
	
	set_focus(*m_entry);

	std::cout << "MessageWindow(" << m_name << "): constructed\n";
}

void MessageWindow::giveMessage(std::string p_msg)
{
	std::cout << "MessageWindow(" << m_name << "): Recieved new message: " << p_msg << std::endl;

	m_buffer->insert(m_buffer->end(), m_name+": "+p_msg+"\n");
	scrollDown();
}

std::string MessageWindow::getName()
{
	return m_name;
}

void MessageWindow::sendEntry()
{
	std::string msg = m_entry->get_text();
	if (msg != "")
	{
		m_entry->set_text("");

		m_buffer->insert(m_buffer->end(), (*m_nameptr)+": "+msg+"\n");
		scrollDown();

		m_events->to_network->pushEvent( UIEvent ( "SEND_TEXT", m_name, msg ) );
	}
}

void MessageWindow::scrollDown()
{
	Gtk::Adjustment* adj = m_scrolled->get_vadjustment();
	
	adj->set_value( adj->get_upper() );	
}
