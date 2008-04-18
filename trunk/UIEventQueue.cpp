#include "UIEventQueue.h"


void UIEventQueue::lock()
{
	/*bool completed=false;
	while(!completed)
	{
		if(!m_lock)
		{
			m_lock = true;
			completed = true;
		}
	}*/
	m_lock.lock();
}
void UIEventQueue::unlock()
{
	//m_lock = false;
	m_lock.unlock();
}

UIEventQueue::UIEventQueue(std::string p_name, int p_readfd, int p_writefd)
{
	m_name = p_name;
	m_readfd = p_readfd;
	m_writefd = p_writefd;
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

