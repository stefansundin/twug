#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <pthread.h>    /* for POSIX threads */

#define MAXPENDING 5    /* Maximum outstanding connection requests */

void str_reverse(char s[]) {
	int c, i, j;
	for (i=0, j=strlen(s)-1; i < j; i++,j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

int CreateTCPServerSocket(unsigned short port) {
	/* Create socket for incoming connections */
	int sock;
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fprintf(stderr,"socket() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	
	/* Construct local address structure */
	struct sockaddr_in serveraddr; /* Local address */
	serveraddr.sin_family = AF_INET;                /* Internet address family */
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
	serveraddr.sin_port = htons(port);              /* Local port */
	
	/* Bind to the local address */
	if (bind(sock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
		fprintf(stderr,"bind() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	
	/* Mark the socket so it will listen for incoming connections */
	if (listen(sock, MAXPENDING) < 0) {
		fprintf(stderr,"listen() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}

	return sock;
}


int AcceptTCPConnection(int serversock) {
	struct sockaddr_in clientaddr; /* Client address */

	/* Get the size of the in-out parameter */
	unsigned int length;
	length = sizeof(clientaddr);
	
	/* Wait for a client to connect */
	int clientsock; /* Socket descriptor for client */
	if ((clientsock = accept(serversock, (struct sockaddr *) &clientaddr, &length)) < 0) {
		fprintf(stderr,"accept() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	
	/* clientsock is connected to a client! */
	printf("Handling client %s\n", inet_ntoa(clientaddr.sin_addr));
	
	return clientsock;
}


void HandleTCPClient(int clientsock) {
	/* Get the length of the message */
	int length;
	if (recv(clientsock, &length, sizeof(length), 0) < 0) {
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
	if ((bytesreceived = recv(clientsock, message, length, 0)) < 0) {
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
	
	/* Reverse message */
	str_reverse(message);
	
	/* Send the length of the message */
	if (send(clientsock, &length, sizeof(length), 0) != sizeof(length)) {
		fprintf(stderr,"send() sent a different number of bytes than expected. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	
	/* Send the reversed message back to client */
	if (send(clientsock, message, length, 0) != length) {
		fprintf(stderr,"send() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	
	printf("Sent: %s\n", message);
	
	close(clientsock);    /* Close client socket */
}


/* Structure of arguments to pass to client thread */
struct ThreadArgs {
	int clientsock;                      /* Socket descriptor for client */
};

void *ThreadMain(void *threadArgs)
{
	int clientsock;                   /* Socket descriptor for client connection */
	
	/* Ensure that thread resources are freed upon return */
	pthread_detach(pthread_self()); 
	
	/* Extract socket file descriptor from argument */
	clientsock = ((struct ThreadArgs *) threadArgs) -> clientsock;
	free(threadArgs);              /* Deallocate memory for argument */
	
	HandleTCPClient(clientsock);
	
	return NULL;
}


int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr,"%s: Too few arguments.\n", argv[0]);
		fprintf(stderr,"%s: Usage: %s <port>\n", argv[0], argv[0]);
		exit(1);
	}
	
	/* <port> */
	unsigned short serverport;
	serverport = atoi(argv[1]);
	
	/* Create TCP Socket */
	int serversock;
	serversock = CreateTCPServerSocket(serverport);

	/* Wait for connections */
	int clientsock;
	for (;;) {
		clientsock = AcceptTCPConnection(serversock);
		
		/* Create separate memory for client argument */
		struct ThreadArgs *threadArgs;
		if ((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) == NULL) {
			fprintf(stderr,"%s: malloc() failed. file %s, line %d.\n",argv[0],__FILE__,__LINE__);
			exit(1);
		}
		threadArgs -> clientsock = clientsock;
		
		/* Create client thread */
		pthread_t threadID;              /* Thread ID from pthread_create() */
		if (pthread_create(&threadID, NULL, ThreadMain, (void *) threadArgs) != 0) {
			fprintf(stderr,"%s: pthread_create() failed. file %s, line %d.\n",argv[0],__FILE__,__LINE__);
			exit(1);
		}
		printf("with thread %ld\n", (long int) threadID);
	}
}

