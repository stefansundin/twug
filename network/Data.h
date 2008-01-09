#ifndef DATA_H_
#define DATA_H_

#include <string>

class Data
{
public:
	Data();
	Data(int p_type, std::string p_data);
	~Data();

	int getType();
	std::string getData();

	int getLength();
private:
	int m_type;
	std::string m_data;
};

#endif /*DATA_H_*/
