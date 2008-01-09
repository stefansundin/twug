#include "Data.h"

Data::Data()
{
}
Data::Data(int p_type, std::string p_data)
{
	m_type = p_type;
	m_data = p_data;
}
~Data()
{
}

std::string Data::getData()
{
	return m_data;
}
int Data::getType()
{
	return m_type;
}

int Data::getLength()
{
	return m_data.length();
}