#ifndef HEADER_H_
#define HEADER_H_

struct header
{
	char	id[5]; // "TWUG"
	short	version;
	short	message_type;
	int		message_length;
};

#endif /*HEADER_H_*/

