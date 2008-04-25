#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define DEBUG

int main(int argc, char *argv[])
{
	if (argc < 4) {
		fprintf(stderr,"%s: Too few arguments.\n", argv[0]);
		fprintf(stderr,"%s: Usage: %s <ip> <port> <message>\n", argv[0], argv[0]);
		exit(1);
	}

	/* Create a stream socket using TCP */
	int sock;
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fprintf(stderr,"%s: socket() failed. file %s, line %d.\n",argv[0],__FILE__,__LINE__);
		exit(1);
	}

	/* Construct the server address structure */
	struct sockaddr_in serveraddr;
	serveraddr.sin_family      = AF_INET;             /* Internet address family */
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);   /* Server IP address */
	serveraddr.sin_port        = htons(atoi(argv[2])); /* Server port */

	/* Establish the connection to the server */
	if (connect(sock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
		fprintf(stderr,"%s: connect() failed. file %s, line %d.\n",argv[0],__FILE__,__LINE__);
		exit(1);
	}

	/* Get length of string to send */
	unsigned int length;
	length = strlen(argv[3]);

	/* Send the length of the string */
	if (send(sock, &length, sizeof(length), 0) != sizeof(length)) {
		fprintf(stderr,"%s: send() sent a different number of bytes than expected. file %s, line %d.\n",argv[0],__FILE__,__LINE__);
		exit(1);
	}
	
	/* Send the string */
	if (send(sock, argv[3], length, 0) != length) {
		fprintf(stderr,"%s: send() sent a different number of bytes than expected. file %s, line %d.\n",argv[0],__FILE__,__LINE__);
		exit(1);
	}
	
	printf("Sent: %s\n", argv[3]);
	
	/* Get the length of the message */
	if (recv(sock, &length, sizeof(length), 0) < 0) {
		fprintf(stderr,"recv() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	
	/* Allocate size for message */
	char *message;
	if ((message=(char*)malloc(length+1)) == NULL) {
		fprintf(stderr,"malloc() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}

	/* Receive the message */
	int bytesreceived;
	if ((bytesreceived = recv(sock, message, length, 0)) < 0) {
		fprintf(stderr,"recv() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	
	/* Verify that the length of the bytes expected and of the bytes received are the same */
	if (length != bytesreceived) {
		fprintf(stderr,"Didn't get the expected amount of bytes. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}

	/* We got the whole message */
	message[bytesreceived]='\0';
	printf("Received: %s\n", message);

	close(sock);
	exit(0);
}
