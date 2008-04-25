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
#include "shared.h"


#define MAXPENDING 5    /* Maximum outstanding connection requests */

static int playCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData )
{
    paData *data = (paData*)userData;
	SAMPLE *wptr = (SAMPLE*)outputBuffer;
	
	unsigned int i=0;
    
	paBuffer *buffer = &data->play_buffers[0];
	if (buffer->go) {
		SAMPLE *rptr = &buffer->samples[buffer->frameIndex * NUM_CHANNELS];
		unsigned int framesLeft = buffer->maxFrameIndex-buffer->frameIndex;
		unsigned int framesToPlay = (framesLeft < framesPerBuffer?framesLeft:framesPerBuffer);
		//We got some audio to play
		for(i=0; i < framesToPlay; i++ )
		{
		    *wptr++ = *rptr++;  // left
		    if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  // right
		}
	    buffer->frameIndex += framesToPlay;
	    //Fill the rest of the buffer with silence
	    for(; i<framesPerBuffer; i++ )
	    {
	        *wptr++ = 0;  // left
	        if( NUM_CHANNELS == 2 ) *wptr++ = 0;  // right
	    }
	}
	else {
		//Silence
		for( i=0; i<framesPerBuffer; i++ )
	    {
	        *wptr++ = 0;  // left
	        if( NUM_CHANNELS == 2 ) *wptr++ = 0;  // right
	    }
	}
	
	if (buffer->go && buffer->frameIndex == buffer->maxFrameIndex) {
		return paComplete;
	}
	else {
		return paContinue;
	}
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
    paData          data;
    int                 i;

    err = Pa_Initialize();
    if( err != paNoError ) goto done;
	
    data.record_buffer.frameIndex=0;
    data.record_buffer.maxFrameIndex=SAMPLE_RATE;
	data.record_buffer.tx_pos=0;
	data.record_buffer.size=sizeof(SAMPLE)*NUM_CHANNELS*data.record_buffer.maxFrameIndex;
	data.record_buffer.go=0;
	// Allocate size for 1 second
	if ((data.record_buffer.samples=(SAMPLE*)malloc(data.record_buffer.size)) == NULL) {
		fprintf(stderr,"failed.\nmalloc() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	//fprintf(stderr,"Line: %d\n",__LINE__);
    data.num_play_buffers=2;
	if ((data.play_buffers=(paBuffer*)malloc(data.num_play_buffers)) == NULL) {
		fprintf(stderr,"failed.\nmalloc() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	for (i=0; i < data.num_play_buffers; i++) {
		data.play_buffers[i].frameIndex=0;
		data.play_buffers[i].maxFrameIndex=0;
		data.play_buffers[i].tx_pos=0;
		data.play_buffers[i].size=sizeof(SAMPLE)*NUM_CHANNELS*SAMPLE_RATE;
		data.play_buffers[i].go=0;
		// Allocate size for 1 second
		if ((data.play_buffers[i].samples=(SAMPLE*)malloc(data.play_buffers[i].size)) == NULL) {
			fprintf(stderr,"failed.\nmalloc() failed. file %s, line %d.\n",__FILE__,__LINE__);
			exit(1);
		}
	}
	
	//Config portaudio
    outputParameters.device = Pa_GetDefaultOutputDevice();
    outputParameters.channelCount = NUM_CHANNELS;
    outputParameters.sampleFormat = PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    //Open file
    FILE *f;
    f=fopen("recording-server.raw","wb");

    err = Pa_OpenStream(
              &stream,
              NULL, // no input
              &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      // we won't output out of range samples so don't bother clipping them
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
	int sock; /* Socket descriptor for client */
	if ((sock = accept(serversock, (struct sockaddr *) &clientaddr, &addrlength)) < 0) {
		fprintf(stderr,"failed.\naccept() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	
	/* sock is connected to a client! */
	printf("%s connected\n", inet_ntoa(clientaddr.sin_addr)); fflush(stdout);
	
	// Receive data
	int bytesreceived;
	paBuffer *buffer=&data.play_buffers[0];
	
	printf("Getting samples: ...\n"); fflush(stdout);
	do {
		int bytestorecv=sizeof(SAMPLE)*NUM_CHANNELS*FRAMES_PER_BUFFER;
		//Check if we have to increase the buffer
		if (buffer->tx_pos+bytestorecv > buffer->size) {
			//We have to realloc, add size for 1 second
			buffer->maxFrameIndex+=SAMPLE_RATE;
			buffer->size=sizeof(SAMPLE)*NUM_CHANNELS*buffer->maxFrameIndex;
			printf("Realloc buffer, new size: %d bytes.\n",buffer->size);
			if ((buffer->samples=(SAMPLE*)realloc(buffer->samples,buffer->size)) == NULL) {
				fprintf(stderr,"realloc() failed. file %s, line %d.\n",__FILE__,__LINE__);
				exit(1);
			}
		}
		//Receive the data
		bytesreceived=recv(sock, buffer->samples+buffer->tx_pos, bytestorecv, 0);
		fwrite(buffer->samples+buffer->tx_pos,1,bytesreceived,f); fflush(f);
		printf("Getting samples: %d-%d (max %d, got %d)\n",buffer->tx_pos,buffer->tx_pos+bytesreceived,buffer->size,bytesreceived); fflush(stdout);
		buffer->tx_pos+=bytesreceived;
		buffer->maxFrameIndex=buffer->tx_pos/(sizeof(SAMPLE)*NUM_CHANNELS);
		//Start playback if we got some audio
		if (!buffer->go && buffer->maxFrameIndex > FRAMES_PER_BUFFER*5) {
			buffer->go=1;
		    printf("Starting playback.\n"); fflush(stdout);
		}
	} while(bytesreceived != 0);
	
	//If playback haven't started, start it now
	if (!buffer->go) {
		buffer->go=1;
	}

	printf("Closing connection... "); fflush(stdout);
	close(sock);
	printf("done\n"); fflush(stdout);
	
	fclose(f);
	
    printf("Waiting for playback to finish... "); fflush(stdout);
    while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(100);
    if( err < 0 ) goto done;
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto done;
    printf("done!\n");

done:
    Pa_Terminate();
    if( data.record_buffer.samples ) free( data.record_buffer.samples );
    for (i=0; i < data.num_play_buffers; i++) {
		if( data.play_buffers[i].samples ) free( data.play_buffers[i].samples );
    }
    free( data.play_buffers );

    if( err != paNoError )
    {
        fprintf( stderr, "An error occured while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        err = 1;          /* Always return 0 or 1, but no other return codes. */
    }
    return;

}

