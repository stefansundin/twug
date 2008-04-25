#include "MessageHandler.h"

MessageHandler::MessageHandler(UIEvents* p_events, std::string* p_nameptr)
{
	m_nameptr = p_nameptr;
	m_events = p_events; 

	print_me("Constructed");
}

void MessageHandler::handleMessage(std::string p_sendername, std::string p_msg)
{
	print_me("Handling message from "+p_sendername);
	showWindow(p_sendername);
	//std::cout << "MessageHandler: <DEBUG> Window is shown\n";	

	std::vector<MessageWindow*>::iterator iter;
	for(iter=m_windows.begin();iter!=m_windows.end();iter++)
	{
		if ((*iter)->getName() == p_sendername)
		{
			(*iter)->giveMessage(p_msg);
			break;
		}
	}
}

void MessageHandler::showWindow(std::string p_name)
{
	bool found=false;

	std::vector<MessageWindow*>::iterator iter;

	for(iter=m_windows.begin(); iter!=m_windows.end(); iter++)
	{
		if ((*iter)->getName() == p_name)
		{
			found=true;
			break;
		}
	}

	if (found)
	{
		if (!(*iter)->is_visible())
			(*iter)->show();
		else
			(*iter)->present();	
	} else {
		m_windows.push_back( new MessageWindow(p_name,m_events,m_nameptr) );
	}
}

