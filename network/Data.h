#ifndef DATA_H_
#define DATA_H_

#include <string>

class Data
{
public:
	Data();
	Data(int p_type, const void *data, unsigned int p_length);
	~Data();

	int getType();
	const void* getData();

	unsigned int getLength();
private:
	int m_type;
	const void *m_data;
	unsigned int m_length;
};

#endif /*DATA_H_*/
