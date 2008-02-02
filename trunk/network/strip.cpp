#include <string>

bool strip(std::string &p_str)
{
	while(p_str.at(p_str.size()-1) == '0')
	{
		p_str.erase(p_str.size()-1);
	}
}
