#ifndef ChannelList_h
#define ChannelList_h

#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <string>
#include "UIEvents.h"

class ChannelList : public Gtk::TreeView
{
protected:
	Glib::RefPtr<Gtk::TreeStore> m_store;
	Gtk::TreeModelColumnRecord* m_columns;
	UIEvents* m_events;

	Gtk::TreeModelColumn<Glib::ustring>* m_column;

	std::vector<std::string> m_lastchannellist;
	Gtk::Menu m_menu;
	std::string m_lastclicked;
	std::string m_mychannel;
	std::string* m_nameptr;


	bool isChannel(std::string name);
	void on_personmenu_message();
	void on_channelmenu_join();
	void on_channelmenu_removeChannel();
	void on_backgroundmenu_newChannel();
	void createChannelListMenu(std::string p_entry, GdkEventButton* p_evb, unsigned int p_type);

	virtual bool on_button_press_event(GdkEventButton* p_evb);
public:
	ChannelList(Glib::RefPtr<Gtk::TreeStore> p_store, Gtk::TreeModelColumn<Glib::ustring>* p_column, UIEvents* p_events,std::string* p_nameptr);
	void giveChannelList(std::vector<std::string> p_channels);
	void notifyDisconnected();
};



#endif
