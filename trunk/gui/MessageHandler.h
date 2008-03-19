#ifndef MESSAGEHANDLER_H
#define	MESSAGEHANDLER_H

#include "MessageWindow.h"
#include "../handler/Handler.h"
#include <vector>
#include <string>
#include <iostream>

class MessageHandler
{
private:
	Handler* m_handler;
	std::vector<MessageWindow*> m_windows;
public:
	MessageHandler(Handler* p_handler);
	void handleMessage(std::string p_sendername, std::string msg);
	void showWindow(std::string p_name);
};

#endif
