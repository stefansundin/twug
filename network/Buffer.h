#ifndef Buffer_h
#define Buffer_h

class Buffer
{
public:
	Buffer();
	~Buffer();

	char* getBuffer();
	unsigned int getLength();

	void setBuffer(char *p_buffer, unsigned int p_length);

private:
	char *m_buffer;
	unsigned int m_length;
};

#endif //Buffer_h

