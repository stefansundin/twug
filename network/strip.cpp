#include "strip.h"

bool strip(std::string &p_str)
{
	while(p_str.at(p_str.size()-1) == FILL_CHAR)
	{
		p_str.erase(p_str.size()-1);
	}

	return true;
}

