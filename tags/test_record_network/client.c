/*
	Record audio and send
*/

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "portaudio.h"
#include "shared.h"

static int recordCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
	paData *data = (paData*)userData;
	
	unsigned int i=0;
	
	paBuffer *buffer = &data->record_buffer;
	if (buffer->go && inputBuffer != NULL) {
	    const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
		SAMPLE *wptr = &buffer->samples[buffer->frameIndex * NUM_CHANNELS];
		unsigned int framesLeft = buffer->maxFrameIndex-buffer->frameIndex;
		unsigned int framesToRecord = (framesLeft < framesPerBuffer?framesLeft:framesPerBuffer);
		//We got some audio to record
		for(i=0; i < framesToRecord; i++ )
		{
		    *wptr++ = *rptr++;  // left
		    if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  // right
		}
	    buffer->frameIndex += framesToRecord;
	}

	if (buffer->frameIndex == buffer->maxFrameIndex) {
		return paComplete;
	}
	else {
		return paContinue;
	}
}

/*******************************************************************/
int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr,"%s: Too few arguments.\n", argv[0]);
		fprintf(stderr,"%s: Usage: %s <ip> <port>\n", argv[0], argv[0]);
		exit(1);
	}
	
	printf("Connecting to %s:%s... ",argv[1],argv[2]);

	/* Establish connection */
	int sock;
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fprintf(stderr,"failed.\nsocket() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}

	/* Construct the server address structure */
	struct sockaddr_in serveraddr;
	serveraddr.sin_family      = AF_INET;             /* Internet address family */
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);   /* Server IP address */
	serveraddr.sin_port        = htons(atoi(argv[2])); /* Server port */

	/* Establish the connection to the server */
	if (connect(sock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
		fprintf(stderr,"failed.\nconnect() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
	}
	
	printf("success!\n");
	
	/* AUDIO */
    PaStreamParameters  inputParameters;
    PaStream*           stream;
    PaError             err = paNoError;
    paData          data;
    int i;
    
    data.record_buffer.frameIndex=0;
    data.record_buffer.maxFrameIndex=SAMPLE_RATE*3;
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
	
    err = Pa_Initialize();
    if( err != paNoError ) goto done;

    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    inputParameters.channelCount = NUM_CHANNELS;                    /* stereo input */
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    
    //Open file
    FILE *f;
    f=fopen("recording-client.raw","wb");

    /* Record some audio. -------------------------------------------- */
    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL,                  // no output
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      // we won't output out of range samples so don't bother clipping them
              recordCallback,
              &data );
    if( err != paNoError ) goto done;

	paBuffer *buffer=&data.record_buffer;	
	buffer->go=1;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto done;
    printf("Now recording!\n"); fflush(stdout);

	//Send samples
	int bytessent;
	printf("Sending samples: ...\n"); fflush(stdout);
    while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
    {
        Pa_Sleep(10);
		int bytestosend=sizeof(SAMPLE)*NUM_CHANNELS*buffer->frameIndex-buffer->tx_pos;
		if (bytestosend > 0) {
			//Send the data
			bytessent=send(sock, buffer->samples+buffer->tx_pos, bytestosend, 0);
			fwrite(buffer->samples+buffer->tx_pos,1,bytessent,f); fflush(f);
			printf("Sending samples: %d-%d (size %d, sent %d)\n",buffer->tx_pos,buffer->tx_pos+bytessent,buffer->size,bytessent); fflush(stdout);
			buffer->tx_pos+=bytessent;
			//buffer->maxFrameIndex=buffer->tx_pos/sizeof(SAMPLE);
			//Check if we have to increase buffer
			if (sizeof(SAMPLE)*NUM_CHANNELS*buffer->frameIndex+sizeof(SAMPLE)*NUM_CHANNELS*SAMPLE_RATE > buffer->size) {
				//We have to realloc, add size for 1 second
				buffer->maxFrameIndex+=SAMPLE_RATE;
				buffer->size=sizeof(SAMPLE)*NUM_CHANNELS*buffer->maxFrameIndex;
				printf("Realloc buffer, new size: %d bytes.\n",buffer->size);
				if ((buffer->samples=(SAMPLE*)realloc(buffer->samples,buffer->size)) == NULL) {
					fprintf(stderr,"realloc() failed. file %s, line %d.\n",__FILE__,__LINE__);
					exit(1);
				}
			}
		}
    }
    printf("\n");
    if( err < 0 ) goto done;
    
    printf("Closing stream... "); fflush(stdout);
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto done;
    printf("done\n"); fflush(stdout);
	
	printf("Closing connection... "); fflush(stdout);
	close(sock);
	printf("done\n"); fflush(stdout);
	
	fclose(f);
	
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
	    return 1;
    }
}

