#ifndef MESSAGEHANDLER_H
#define	MESSAGEHANDLER_H

#include <vector>
#include <string>
#include <iostream>

#include "MessageWindow.h"
#include "HandlerLink.h"


class MessageHandler
{
private:
	Handler* m_handler;
	std::string* m_nameptr;
	std::vector<MessageWindow*> m_windows;
public:
	MessageHandler(Handler* p_handler, std::string* p_nameptr);
	void handleMessage(std::string p_sendername, std::string msg);
	void showWindow(std::string p_name);
};

#endif
