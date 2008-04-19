#include "Data.h"

Data::Data()
{
	m_type = 0;
	m_data = 0;
	m_length = 0;
}
Data::Data(int p_type, std::string p_str_data)
{
	m_type = p_type;
	m_length = p_str_data.size();

	m_data = new char[m_length];
	memcpy(m_data, p_str_data.c_str(), m_length);
}
Data::Data(int p_type, const void *p_data, unsigned int p_length)
{
	m_type = p_type;
	m_length = p_length;

	m_data = new char[m_length];

	memcpy(m_data, p_data, m_length);

}
Data::~Data()
{
	if(m_data != 0)
	{
//		delete [] m_data;		//why does this line cause a crash?
	}
}

const void* Data::getData() const
{
	return m_data;
}
int Data::getType() const
{
	return m_type;
}

unsigned int Data::getLength() const
{
	return m_length;
}

