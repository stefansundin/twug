#ifndef HEADER_H_
#define HEADER_H_

struct header
{
	char			id[5];				// "TWUG\0"
	short			version;
	short			message_type;
	unsigned int	message_length;		//in bytes
}__attribute__((packed));		//apparently this prevents GCC from messing up the layout of the struct

#endif /*HEADER_H_*/

