#include <string>

#include "fill.h"

bool fill(std::string &p_str, unsigned int p_length)
{
	if(p_str.length() > p_length)
		return false;
	while(p_str.length() < p_length)
	{
		p_str += FILL_CHAR;
	}
	return true;
}
