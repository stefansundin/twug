#ifndef Channel_h
#define Channel_h

#include <string>

#include "../debug.h"

class Channel
{
public:
	Channel();
	Channel(std::string p_name, std::string p_password);
	~Channel();

	std::string getName() const;
	std::string getPassword() const;

	void setPassword(std::string p_password);

	bool operator<(const Channel &p_rhs) const;

private:
	std::string m_name;
	std::string m_password;
};

#endif //Channel_h

