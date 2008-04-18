#ifndef MESSAGEHANDLER_H
#define	MESSAGEHANDLER_H

#include <vector>
#include <string>
#include <iostream>

#include "MessageWindow.h"
#include "UIEvents.h"


class MessageHandler
{
private:
	std::string* m_nameptr;
	UIEvents* m_events;
	std::vector<MessageWindow*> m_windows;
public:
	MessageHandler(UIEvents* p_events, std::string* p_nameptr);
	void handleMessage(std::string p_sendername, std::string msg);
	void showWindow(std::string p_name);
};

#endif
