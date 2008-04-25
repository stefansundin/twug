#ifndef Data_h
#define Data_h

#include <string>
#include <cstring>

class Data
{
public:
	Data();
	Data(int p_type, std::string p_str_data);
	Data(int p_type, const void *data, unsigned int p_length);
	~Data();

	const void* getData() const;
	unsigned int getLength() const;
	int getType() const;

private:
	int m_type;
	char *m_data;
	unsigned int m_length;
};

#endif //Data_h
