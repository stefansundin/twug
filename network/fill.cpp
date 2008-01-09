bool fill(std::string &p_str)
{
	if(p_str.length() > p_length)
		return false;
	while(p_str.length() < p_length)
	{
		p_str += "0";
	}
	return true;
}