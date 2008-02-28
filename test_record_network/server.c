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
#define NUM_SECONDS     (7)
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
	int play;
    int frameIndex;
    int maxFrameIndex;
    int receivedFrameIndex;
    SAMPLE *recordedSamples;
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
    unsigned int framesLeft = data->maxFrameIndex - data->receivedFrameIndex;

    (void) inputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

	if (!data->play) {
		// silence
		for( i=0; i<framesPerBuffer; i++ )
	    {
	        *wptr++ = 0;  /* left */
	        if( NUM_CHANNELS == 2 ) *wptr++ = 0;  /* right */
	    }
	    finished = paContinue;
	}
	else {
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
		    if (data->frameIndex == data->maxFrameIndex) {
			    finished = paComplete;
			}
			else {
				finished=paContinue;
			}
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
	}
    return finished;
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


	/* Audio */
	printf("Initializing portaudio... "); fflush(stdout);
	
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
    data.play=0;
	data.receivedFrameIndex=0;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    outputParameters.channelCount = NUM_CHANNELS;                     /* stereo output */
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

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
    
    err = Pa_StartStream( stream );
    if( err != paNoError ) goto done;

	printf("done!\n"); fflush(stdout);

	/* Create socket for incoming connections */
	int serversock;
	if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fprintf(stderr,"socket() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	
	/* Construct local address structure */
	struct sockaddr_in serveraddr; /* Local address */
	serveraddr.sin_family = AF_INET;                /* Internet address family */
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
	serveraddr.sin_port = htons(serverport);              /* Local port */

	/* Bind to the local address */
	if (bind(serversock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
		fprintf(stderr,"bind() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}

	printf("Waiting for connections... "); fflush(stdout);
	/* Mark the socket so it will listen for incoming connections */
	if (listen(serversock, MAXPENDING) < 0) {
		fprintf(stderr,"listen() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}

	/* Wait for connections */
	struct sockaddr_in clientaddr; /* Client address */

	/* Get the size of the in-out parameter */
	unsigned int addrlength = sizeof(clientaddr);
	
	/* Wait for a client to connect */
	int clientsock; /* Socket descriptor for client */
	if ((clientsock = accept(serversock, (struct sockaddr *) &clientaddr, &addrlength)) < 0) {
		fprintf(stderr,"accept() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	
	/* clientsock is connected to a client! */
	printf("%s connected\n", inet_ntoa(clientaddr.sin_addr)); fflush(stdout);
	
	/* Get the length of the message */
	int length;
	if (recv(clientsock, &length, sizeof(length), 0) < 0) {
		fprintf(stderr,"recv() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	printf("Length of message: %d bytes\n",length);
	
	/* Allocate size for message */
	if ((data.recordedSamples=(SAMPLE*)malloc(length)) == NULL) {
		fprintf(stderr,"malloc() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	
	/* Receive the message */
	int bytesreceived;
	printf("Getting message: 0"); fflush(stdout);
	while (data.receivedFrameIndex < length) {
		int bytestorecv=1000;
		if (data.receivedFrameIndex+bytestorecv > length) {
			bytestorecv=length-data.receivedFrameIndex;
		}
		bytesreceived=recv(clientsock, data.recordedSamples+data.receivedFrameIndex, bytestorecv, 0);
		data.receivedFrameIndex+=bytesreceived;
		printf("\rGetting message: %d (got %d)         ",data.receivedFrameIndex,bytesreceived); fflush(stdout);
		if (!data.play && data.receivedFrameIndex > 1000) {
			data.play=1;
		    printf("\rStarting playback.                 \n"); fflush(stdout);
		}
	}
	printf("\n");
    printf("Waiting for playback to finish... "); fflush(stdout);
	
    while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(100);
    if( err < 0 ) goto done;
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto done;
    printf(" done!\n");
    
	/* Verify that the length of the bytes expected and of the bytes received are the same */
	if (length != data.receivedFrameIndex) {
		fprintf(stderr,"Didn't get the expected amount of bytes (got %d bytes). file %s, line %d.\n",data.receivedFrameIndex,__FILE__,__LINE__);
		exit(1);
	}

	/* We got the whole message */
	printf("Received %d bytes!\n", length);
	close(clientsock);    /* Close client socket */
	
	/* write to file */
    /*FILE  *fid;
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
    }*/
/*
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
    }*/

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

