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

typedef struct {
    SAMPLE *samples;
    int frameIndex;
    int maxFrameIndex;
    int tx_pos;
    int size;
} paBuffer;

typedef struct
{
	int play;
    int play_buffer;
    int new_buffer;
    int last_buffer;
    paBuffer buffers[2];
} paTestData;

static int playCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData )
{
    paTestData *data = (paTestData*)userData;
	SAMPLE *wptr = (SAMPLE*)outputBuffer;
	
	int finished;
	unsigned int i=0;

	if (!data->play) {
		//Silence
		for( i=0; i<framesPerBuffer; i++ )
	    {
	        *wptr++ = 0;  // left
	        if( NUM_CHANNELS == 2 ) *wptr++ = 0;  // right
	    }
	    finished = paContinue;
	}
	else {
		paBuffer *buffer = &data->buffers[data->play_buffer];
		SAMPLE *rptr;
		//Check if we have to change buffer
		if (buffer->frameIndex+framesPerBuffer > buffer->maxFrameIndex) {
			//Yes, we have to, but first, check if we have some frames left in the old buffer
			if (buffer->frameIndex < buffer->maxFrameIndex) {
				//Play the remaining frames
				rptr = &buffer->samples[buffer->frameIndex * NUM_CHANNELS];
				for(; i < buffer->maxFrameIndex-buffer->frameIndex; i++ )
				{
					*wptr++ = *rptr++;  // left
					if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  // right
				}
				buffer->frameIndex=buffer->maxFrameIndex;
			}
			//Switch buffers
			data->play_buffer = (data->play_buffer+1)%2;
			data->new_buffer = 1;
			buffer = &data->buffers[data->play_buffer];
		}
		//Do we have more audio to play?
		if (!data->last_buffer || !data->new_buffer) {
			rptr = &buffer->samples[buffer->frameIndex * NUM_CHANNELS];
			unsigned int framesLeft = buffer->maxFrameIndex-buffer->frameIndex;
			unsigned int framesToPlay = (framesLeft < framesPerBuffer?framesLeft:framesPerBuffer);
			//We got some audio to play
			for(; i < framesToPlay; i++ )
			{
			    *wptr++ = *rptr++;  // left
			    if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  // right
			}
		    buffer->frameIndex += framesToPlay;
			finished = paContinue;
		}
		else {
			finished = paComplete;
		}
	    //Fill the rest of the buffer with silence
	    for(; i<framesPerBuffer; i++ )
	    {
	        *wptr++ = 0;  // left
	        if( NUM_CHANNELS == 2 ) *wptr++ = 0;  // right
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
    int                 numBytes;
    SAMPLE              max, val;
    double              average;

    err = Pa_Initialize();
    if( err != paNoError ) goto done;
    
    data.play=0;
	data.play_buffer=0;
	data.new_buffer=0;
	data.last_buffer=0;
    data.buffers[0].frameIndex=0;
    data.buffers[0].maxFrameIndex=0;
	data.buffers[0].tx_pos=0;
	data.buffers[0].size=sizeof(SAMPLE)*NUM_CHANNELS*FRAMES_PER_BUFFER*100;
    data.buffers[1].frameIndex=0;
    data.buffers[1].maxFrameIndex=0;
	data.buffers[1].tx_pos=0;
	data.buffers[1].size=sizeof(SAMPLE)*NUM_CHANNELS*FRAMES_PER_BUFFER*100;
	
	// Allocate size for 100 frames in each buffer
	if ((data.buffers[0].samples=(SAMPLE*)malloc(data.buffers[0].size)) == NULL) {
		fprintf(stderr,"failed.\nmalloc() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	if ((data.buffers[1].samples=(SAMPLE*)malloc(data.buffers[1].size)) == NULL) {
		fprintf(stderr,"failed.\nmalloc() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	
	//Config portaudio
    outputParameters.device = Pa_GetDefaultOutputDevice();
    outputParameters.channelCount = NUM_CHANNELS;
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

	printf("Waiting for connection... "); fflush(stdout);
	/* Mark the socket so it will listen for incoming connections */
	if (listen(serversock, MAXPENDING) < 0) {
		fprintf(stderr,"failed.\nlisten() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}

	/* Wait for connections */
	struct sockaddr_in clientaddr; /* Client address */

	/* Get the size of the in-out parameter */
	unsigned int addrlength = sizeof(clientaddr);
	
	/* Wait for a client to connect */
	int clientsock; /* Socket descriptor for client */
	if ((clientsock = accept(serversock, (struct sockaddr *) &clientaddr, &addrlength)) < 0) {
		fprintf(stderr,"failed.\naccept() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	
	/* clientsock is connected to a client! */
	printf("%s connected\n", inet_ntoa(clientaddr.sin_addr)); fflush(stdout);
	
	/* Get the number of samples */
	int numFrames;
	if (recv(clientsock, &numFrames, sizeof(numFrames), 0) < 0) {
		fprintf(stderr,"recv() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	printf("numFrames: %d frames.\n",numFrames);
	
	/* Receive the message */
	int bytesreceived;
	int byte=0;
	printf("Getting samples: 0"); fflush(stdout);
	do {
		paBuffer *put_buffer=&data.buffers[(data.play_buffer+1)%2];
		int bytestorecv=sizeof(SAMPLE)*FRAMES_PER_BUFFER*NUM_CHANNELS;
		//Check if we have a new buffer
		if (data.new_buffer) {
			data.new_buffer=0;
			//Reset properties
			put_buffer->frameIndex=0;
			put_buffer->maxFrameIndex=0;
			put_buffer->tx_pos=0;
			//Realloc buffer?
		}
		//Check if we have to increase the buffer
		if (put_buffer->tx_pos+bytestorecv > put_buffer->size) {
			//We have to realloc, add size for 100 frames
			put_buffer->size+=sizeof(SAMPLE)*NUM_CHANNELS*FRAMES_PER_BUFFER*100;
			put_buffer->maxFrameIndex+=100;
			if (realloc(put_buffer->samples,put_buffer->size) == NULL) {
				fprintf(stderr,"realloc() failed. file %s, line %d.\n",__FILE__,__LINE__);
				exit(1);
			}
		}
		//Receive the data
		bytesreceived=recv(clientsock, put_buffer->samples+put_buffer->tx_pos, bytestorecv, 0);
		put_buffer->tx_pos+=bytesreceived;
		put_buffer->maxFrameIndex=put_buffer->tx_pos/sizeof(SAMPLE);
		printf("\rPutting samples in buffer %d at %d (max %d, got %d)            ",(data.play_buffer+1)%2,put_buffer->tx_pos,put_buffer->size,bytesreceived); fflush(stdout);
		//Start playback if we got some audio
		if (!data.play && put_buffer->maxFrameIndex > FRAMES_PER_BUFFER*5) {
			data.play_buffer=(data.play_buffer+1)%2; //Switch buffers
			data.new_buffer=1; 
			data.play=1;
		    printf("\rStarting playback.                                                    \n"); fflush(stdout);
		}
	} while(bytesreceived != 0);
	data.last_buffer=1;
	
	printf("\n");
    printf("Waiting for playback to finish... "); fflush(stdout);
	
    while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(100);
    if( err < 0 ) goto done;
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto done;
    printf("done!\n");

	close(clientsock);    /* Close client socket */

done:
    Pa_Terminate();
    if( data.buffers[0].samples ) free( data.buffers[0].samples );
    if( data.buffers[1].samples ) free( data.buffers[1].samples );
    if( err != paNoError )
    {
        fprintf( stderr, "An error occured while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        err = 1;          /* Always return 0 or 1, but no other return codes. */
    }
    return;

}

