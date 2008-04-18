#include "Buffer.h"

#include <cstring>

Buffer::Buffer()
{
	m_length = 0;
	m_buffer = 0;
}
Buffer::~Buffer()
{
	setBuffer(0, 0);
}


char* Buffer::getBuffer()
{
	return m_buffer;
}
unsigned int Buffer::getLength()
{
	return m_length;
}


void Buffer::setBuffer(char *p_buffer, unsigned int p_length)
{
	if(m_buffer != 0)
	{
		delete [] m_buffer;
	}

	m_length = p_length;
	m_buffer = new char[m_length];

	memcpy(m_buffer, p_buffer, m_length);
}

