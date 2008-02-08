#include "Socket.h"

Socket::Socket()
{
}
Socket::Socket( int domain, int type )
{
	if( domain != AF_INET || (type != SOCK_STREAM && type != SOCK_DGRAM) )
	{
		throw "Only domain AF_INET and types SOCK_STREAM and SOCK_DGRAM accepted.";
	}
	addr.sin_family = domain;
	sock = ::socket(domain, type, 0);
}
Socket::Socket( int file_descriptor )
{
	sock = file_descriptor;
}
Socket::~Socket()
{}

int Socket::connect( const std::string p_address, unsigned int p_port )
{
	inet_pton( addr.sin_family, p_address.c_str(), &addr.sin_addr );
	addr.sin_port = htons( p_port );
	return ::connect( sock, (sockaddr *) &addr, sizeof(addr) );
}
int Socket::bind( std::string p_address, unsigned int p_port )
{
	if( p_address == "" )
	{
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		inet_pton( addr.sin_family, p_address.c_str(), &addr.sin_addr );
	}
	addr.sin_port = htons( p_port );
	return ::bind( sock, (struct sockaddr *) &addr, sizeof(addr) );
}
int Socket::listen( int backlog )
{
	return ::listen( sock, backlog );
}
Socket Socket::accept()
{
	unsigned int length = sizeof(addr);
	int file_descriptor = ::accept( sock, (struct sockaddr *) &addr, &length );
	return Socket( file_descriptor );
}


int Socket::recv( void *buffer, unsigned int p_max_recive )
{
	int returned = ::recv( sock, buffer, p_max_recive, 0 );
	return returned;
}
int Socket::send( const void *buffer, unsigned int p_length )
{
	return ::send( sock, buffer, sizeof(buffer) ,0 );
}
int Socket::shutdown( int how )
{
	return ::shutdown( sock, how );
}
int Socket::close()
{
	return ::close( sock );
}

