#ifndef fill_h
#define fill_h

#define FILL_CHAR '0'

//fills out the passed string until it is p_length long then returns true.
//if p_str is already longer than p_length false is returned.
bool fill(std::string &p_str, unsigned int p_length);

#endif //fill_h
