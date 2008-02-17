#ifndef HEADER_H_
#define HEADER_H_

struct header
{
	char	id[5];			// "TWUG"
	short	version;
	short	message_type;
	int		message_length;
}__atribute__((packed));		//apparently this prevents GCC from messing up the layout of the struct

#endif /*HEADER_H_*/

