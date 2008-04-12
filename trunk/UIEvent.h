#ifndef UIEvent_h
#define UIEvent_h


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

#endif
