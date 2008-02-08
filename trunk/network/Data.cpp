#include "Data.h"

Data::Data()
{
}
Data::Data(int p_type, const void *p_data, unsigned int p_length)
{
	m_type = p_type;
	m_data = p_data;
	m_length = p_length;
}
Data::~Data()
{
}

const void* Data::getData()
{
	return m_data;
}
int Data::getType()
{
	return m_type;
}

unsigned int Data::getLength()
{
	return m_length;
}

