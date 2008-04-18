#ifndef UIEvent_h
#define UIEvent_h

#include <iostream>
#include <string>
#include <vector>


class UIEvent {
private:
	std::string m_type;
	std::vector<std::string> m_vector;
public:
	UIEvent(std::string p_type);
	UIEvent(std::string p_type, std::vector<std::string> p_vector);
	UIEvent(std::string p_type, std::string p_str);
	UIEvent(std::string p_type, std::string p_str0, std::string p_str1);
	UIEvent(std::string p_type, std::string p_str0, std::string p_str1, std::string p_str2);
	std::string getType();
	std::string getFirst();
	std::string pop_first();
	std::string pop();
	std::vector<std::string> getData();
	std::string top();
};

#endif
