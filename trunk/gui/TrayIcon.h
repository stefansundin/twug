#ifndef TRAYICON_H
#define TRAYICON_H

#include <gtkmm/stock.h>
#include <gtkmm/statusicon.h>
#include <gtkmm.h>
#include <gtkmm/aboutdialog.h>

#include "MainWindow.h"
#include "PrefsWindow.h"


class AboutTwug : public Gtk::AboutDialog
{
private:
	std::vector<Glib::ustring> m_credits;
	void addCredits(Glib::ustring line)
	{
		m_credits.push_back(line);
	}
public:
	AboutTwug()
	{
		addCredits("made by certain people...");

		set_authors(m_credits);
		set_name("Twug");
		set_comments("The Voice Communications Tool");
		set_copyright("©2007-2008 The Twug Team");
		set_website("http://code.google.com/p/twug");
		set_version("0.1");
	}
};

class TrayIcon : public Gtk::StatusIcon
{
public:
  TrayIcon(MainWindow* p_window, PrefsWindow* p_prefswindow);
 ~TrayIcon();
protected:
	AboutTwug* m_about;
	MainWindow* m_window;
	PrefsWindow* m_prefswindow;
	virtual void on_clicked();
	virtual void on_popup(const unsigned int&, const unsigned int&);
	void on_action_quit();
	void on_action_prefs();
	void on_action_about();
	Gtk::Menu* m_menu;
	bool m_restoreprefs;
};

#endif
