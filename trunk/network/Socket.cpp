#include "Socket.h"

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

int Socket::connect( const std::string address, int port )
{
	inet_pton( addr.sin_family, address.c_str(), &addr.sin_addr );
	addr.sin_port = htons( port );
	return ::connect( sock, (sockaddr *) &addr, sizeof(addr) );
}
int Socket::bind( std::string address, int port )
{
	if( address == "" )
	{
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		inet_pton( addr.sin_family, address.c_str(), &addr.sin_addr );
	}
	addr.sin_port = htons( port );
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


int Socket::recv( std::string &data, int max_recive )
{
	data = "";
	char buffer[max_recive+1];
	memset( buffer, 0, max_recive+1 );
	int returned = ::recv( sock, &buffer, max_recive, 0 );
	data = buffer;
	return returned;
}
int Socket::send( std::string data )
{
	const char *buffer = data.c_str();
	return ::send( sock, buffer, data.size() ,0 );
}
int Socket::shutdown( int how )
{
	return ::shutdown( sock, how );
}
int Socket::close()
{
	return ::close( sock );
}
