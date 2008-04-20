#include "strip.h"

#include "../debug.h"

bool strip(std::string &p_str)
{
	print_me("stripping ("+p_str+")");

	if(p_str.size() < 1)
	{
		return false;
	}

	while(p_str.at(p_str.size()-1) == FILL_CHAR)
	{
		p_str.erase(p_str.size()-1);

		if(p_str.size() < 1)
		{
			return false;
		}
	}

	return true;
}

