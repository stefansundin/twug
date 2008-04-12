#include "UIEventQueue.h"


void UIEventQueue::lock()
{
	//std::cout << "Locking " << m_filepath << std::endl;
	bool completed=0;
	while(!completed)
	{
		if(!m_lock)
		{
			m_lock = true;
			completed = true;
		}
	}
}
void UIEventQueue::unlock()
{
	//std::cout << "Unlocking " << m_filepath << std::endl;
	m_lock = false;
}

UIEventQueue::UIEventQueue(std::string p_name)
{
	m_name = p_name;
	m_filepath = p_name + ".temp";
	m_lock = 0;

 	if (access(m_filepath.c_str(), F_OK) == -1)
	{
		if (mkfifo(m_filepath.c_str(), 0666) != 0)
		{
			std::cout << "error creating fifo" << std::endl;
		}
	}
}
std::string UIEventQueue::getFilePath() const
{
	return m_filepath;
}
std::string UIEventQueue::getName() const
{
	return m_name;
}	
UIEvent UIEventQueue::popEvent()
{
	//std::cout << "UIEvents: Popping event from queue " << m_name << std::endl;
	UIEvent event = UIEvent("EMPTY");
	lock();
	if(!m_queue.empty())
	{
		event = m_queue.front();
		m_queue.pop();
	}			
	unlock();
	std::cout << "UIEvents: Popped event from queue " << m_name << std::endl;
	return event;
}
void UIEventQueue::pushEvent(UIEvent p_event)
{
	//std::cout << "UIEvents: Pushing event to queue " << m_name << std::endl;
	lock();

	m_queue.push(p_event);

	unlock();
	std::cout << "UIEvents: Pushed event to queue " << m_name << std::endl;
}
