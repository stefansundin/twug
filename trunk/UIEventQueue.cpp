#include "UIEventQueue.h"


void UIEventQueue::lock()
{
	m_lock.lock();
}
void UIEventQueue::unlock()
{
	m_lock.unlock();
}

#ifndef _WIN32 
UIEventQueue::UIEventQueue(std::string p_name, int p_readfd, int p_writefd)
{
	m_readfd = p_readfd;
	m_writefd = p_writefd;
#else
UIEventQueue::UIEventQueue(std::string p_name)
{	
#endif
	m_name = p_name;
}

int UIEventQueue::getReadFd() const 
{
	return m_readfd;
}
	
int UIEventQueue::getWriteFd() const 
{
	return m_writefd;
}

std::string UIEventQueue::getName() const
{
	return m_name;
}	
UIEvent UIEventQueue::popEvent()
{
	//print_me("UIEventQueue("+m_name+"): Popping event");
	UIEvent event = UIEvent("EMPTY");
	lock();
	if(!m_queue.empty())
	{
		event = m_queue.front();
		m_queue.pop();
	}			
	unlock();
	//print_me("UIEventQueue("+m_name+"): Popped event");
	return event;
}
void UIEventQueue::pushEvent(UIEvent p_event)
{
	//print_me("UIEventQueue("+m_name+"): Pushing event");
	lock();

	m_queue.push(p_event);

	unlock();
	//print_me("UIEventQueue("+m_name+"): Pushed event");
}

