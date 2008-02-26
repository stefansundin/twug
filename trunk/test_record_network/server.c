/*
	Receive audio and play
*/

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <errno.h>
#include "portaudio.h"

#define MAXPENDING 5    /* Maximum outstanding connection requests */

/* AUDIO */
#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (1024)
#define NUM_SECONDS     (1)
#define NUM_CHANNELS    (1)

#if 0
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 0
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif

typedef struct
{
    int          frameIndex;  /* Index into sample array. */
    int          maxFrameIndex;
    SAMPLE      *recordedSamples;
}
paTestData;

static int playCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData )
{
    paTestData *data = (paTestData*)userData;
    SAMPLE *rptr = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
    SAMPLE *wptr = (SAMPLE*)outputBuffer;
    unsigned int i;
    int finished;
    unsigned int framesLeft = data->maxFrameIndex - data->frameIndex;

    (void) inputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

    if( framesLeft < framesPerBuffer )
    {
        /* final buffer... */
        for( i=0; i<framesLeft; i++ )
        {
            *wptr++ = *rptr++;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
        }
        for( ; i<framesPerBuffer; i++ )
        {
            *wptr++ = 0;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = 0;  /* right */
        }
        data->frameIndex += framesLeft;
        finished = paComplete;
    }
    else
    {
        for( i=0; i<framesPerBuffer; i++ )
        {
            *wptr++ = *rptr++;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
        }
        data->frameIndex += framesPerBuffer;
        finished = paContinue;
    }
    return finished;
}

/* TCP */

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
	unsigned int length = sizeof(clientaddr);
	
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

/* main */

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr,"%s: Too few arguments.\n", argv[0]);
		fprintf(stderr,"%s: Usage: %s <port>\n", argv[0], argv[0]);
		exit(1);
	}
	
	/* <port> */
	unsigned short serverport = atoi(argv[1]);
	
	/* Create TCP Socket */
	int serversock = CreateTCPServerSocket(serverport);

	/* Wait for connections */
	int clientsock = AcceptTCPConnection(serversock);
	
	/* Get the length of the message */
	int length;
	if (recv(clientsock, &length, sizeof(length), 0) < 0) {
		fprintf(stderr,"recv() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	printf("Length of message: %d bytes\n",length);
	
	/* Allocate size for message */
	SAMPLE *message;
	if ((message=(SAMPLE*)malloc(length)) == NULL) {
		fprintf(stderr,"malloc() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}

	/* Receive the message */
	int bytesreceived;
	int messagepos=0;
	printf("Getting message: 0");
	while (messagepos < length) {
		int bytestorecv=1000;
		if (messagepos+bytestorecv > length) {
			bytestorecv=length-messagepos;
		}
		if ((bytesreceived=recv(clientsock, message+messagepos, bytestorecv, 0)) != bytestorecv) {
			fprintf(stderr,"recv() received a different number of bytes than expected (received %d bytes, expected %d, errno: %d). file %s, line %d.\n",bytesreceived,bytestorecv,errno,__FILE__,__LINE__);
			exit(1);
		}
		messagepos+=bytestorecv;
		printf("\rGetting message: %d (%d)              ",messagepos,bytestorecv);
	}
	printf("\n");
	
	/* Verify that the length of the bytes expected and of the bytes received are the same */
	if (length != messagepos) {
		fprintf(stderr,"Didn't get the expected amount of bytes (got %d bytes). file %s, line %d.\n",messagepos,__FILE__,__LINE__);
		exit(1);
	}

	/* We got the whole message */
	printf("Received %d bytes\n", length);
	close(clientsock);    /* Close client socket */
	
	/* write to file */
    FILE  *fid;
    fid = fopen("recorded-server.raw", "wb");
    if( fid == NULL )
    {
        printf("Could not open file.");
    }
    else
    {
        fwrite( message, 1, length, fid );
        fclose( fid );
        printf("Wrote data to 'recorded-server.raw'\n");
    }

	/* Play message */
    PaStreamParameters  outputParameters;
    PaStream*           stream;
    PaError             err = paNoError;
    paTestData          data;
    int                 i;
    int                 totalFrames;
    int                 numSamples;
    int                 numBytes;
    SAMPLE              max, val;
    double              average;

    err = Pa_Initialize();
    if( err != paNoError ) goto done;

    data.frameIndex = 0;
    data.maxFrameIndex = totalFrames = NUM_SECONDS * SAMPLE_RATE;
	data.recordedSamples=message;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    outputParameters.channelCount = NUM_CHANNELS;                     /* stereo output */
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    printf("Begin playback.\n"); fflush(stdout);
    err = Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              playCallback,
              &data );
    if( err != paNoError ) goto done;

    if( stream )
    {
        err = Pa_StartStream( stream );
        if( err != paNoError ) goto done;
        
        printf("Waiting for playback to finish.\n"); fflush(stdout);

        while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(100);
        if( err < 0 ) goto done;
        
        err = Pa_CloseStream( stream );
        if( err != paNoError ) goto done;
        
        printf("Done.\n"); fflush(stdout);
    }

done:
    Pa_Terminate();
    if( data.recordedSamples )       /* Sure it is NULL or valid. */
        free( data.recordedSamples );
    if( err != paNoError )
    {
        fprintf( stderr, "An error occured while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        err = 1;          /* Always return 0 or 1, but no other return codes. */
    }
    return;

}

