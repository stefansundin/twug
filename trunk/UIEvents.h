#ifndef UIEvents_h
#define UIEvents_h

#include <vector>
#include <string>
#include <queue>
#include <iostream>
#include <fcntl.h>

class UIEvent {
private:
	std::string m_type;
	std::vector<std::string> m_vector;
public:
	UIEvent(std::string p_type)
	{
		m_type = p_type;
	}
	UIEvent(std::string p_type, std::vector<std::string> p_vector)
	{
		m_type = p_type;
		m_vector = p_vector;
	}
	UIEvent(std::string p_type, std::string p_str)
	{
		m_type = p_type;
		m_vector.push_back(p_str);
	}
	UIEvent(std::string p_type, std::string p_str0, std::string p_str1)
	{
		m_type = p_type;
		m_vector.push_back(p_str0);
		m_vector.push_back(p_str1);
	}
	UIEvent(std::string p_type, std::string p_str0, std::string p_str1, std::string p_str2)
	{
		m_type = p_type;
		m_vector.push_back(p_str0);
		m_vector.push_back(p_str1);
		m_vector.push_back(p_str2);
	}
	std::string getType()
	{
		return m_type;
	}
	std::string getFirst()
	{
		return m_vector.at(0);
	}
	std::string pop_first()
	{
		std::string first = getFirst();
		m_vector.erase( m_vector.begin() );
		return first;
	}
	std::string pop()
	{
		std::vector<std::string>::iterator iter = m_vector.end();
		iter--;
		std::string temp = (*iter);
		m_vector.erase(iter);
		return temp;				
	}
	std::vector<std::string> getData()
	{
		return m_vector;
	}
	std::string top()
	{
		return m_vector.at( m_vector.size()-1 );
	}
};

class UIEventQueue {
private:
	std::queue<UIEvent> m_queue;
	std::string m_filepath;
	void lock()
	{
		std::cout << "Locking " << m_filepath << std::endl;
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
	void unlock()
	{
		std::cout << "Unlocking " << m_filepath << std::endl;
		m_lock = false;
	}
	bool m_lock;
public:
	UIEventQueue(std::string p_path)
	{
		m_filepath = p_path;
		m_lock = 0;

	 	if (access(m_filepath.c_str(), F_OK) == -1)
		{
    			if (mkfifo(m_filepath.c_str(), 0666) != 0)
			{
     				std::cout << "error creating fifo" << std::endl;
    			}
		}
	}
	std::string getFilePath() const
	{
		return m_filepath;
	}
	UIEvent popEvent()
	{
		UIEvent event = UIEvent("EMPTY");
		lock();
		if(!m_queue.empty())
		{
			event = m_queue.front();
			m_queue.pop();
		}			
		unlock();
		return event;
	}
	void pushEvent(UIEvent p_event)
	{
		lock();

		m_queue.push(p_event);

		// notify destination thread by writing to a pipe(hack)
		std::cout << "notifying destination thread by writing to a pipe(hack)\n";

		FILE* f = fopen(m_filepath.c_str(), "w");
		fputc('\n', f);
		fclose(f);

		unlock();
	}
};

class UIEvents {
public:
	UIEvents()
	{
		to_ui = new UIEventQueue("to_ui.temp");
		to_network = new UIEventQueue("to_network.temp");
  	}
	UIEventQueue* to_ui; 
	UIEventQueue* to_network;
};

#endif
