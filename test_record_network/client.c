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

#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (1024)
#define NUM_SECONDS     (60)
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
    SAMPLE *samples;
    int frameIndex;
    int maxFrameIndex;
    int tx_pos;
}
paTestData;

static int recordCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    paTestData *data = (paTestData*)userData;
    const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
    SAMPLE *wptr = &data->samples[data->frameIndex * NUM_CHANNELS];
    long framesToCalc;
    long i;
    int finished;
    unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;

    if( framesLeft < framesPerBuffer )
    {
        framesToCalc = framesLeft;
        finished = paComplete;
    }
    else
    {
        framesToCalc = framesPerBuffer;
        finished = paContinue;
    }

    if( inputBuffer == NULL )
    {
        for( i=0; i<framesToCalc; i++ )
        {
            *wptr++ = SAMPLE_SILENCE;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = SAMPLE_SILENCE;  /* right */
        }
    }
    else
    {
        for( i=0; i<framesToCalc; i++ )
        {
            *wptr++ = *rptr++;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
        }
    }
    data->frameIndex += framesToCalc;
    return finished;
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
    paTestData          data;
    SAMPLE              max, val;
    double              average;

    data.maxFrameIndex = NUM_SECONDS * SAMPLE_RATE;
    data.frameIndex = 0;
    data.tx_pos = 0;
    if ((data.samples = (SAMPLE*)malloc(sizeof(SAMPLE)*NUM_CHANNELS*data.maxFrameIndex)) == NULL) {
		fprintf(stderr,"malloc() failed. file %s, line %d.\n",__FILE__,__LINE__);
		exit(1);
    }
	//memset(data.samples,0,sizeof(SAMPLE)*NUM_CHANNELS*data.maxFrameIndex);

	/* Send info about the data */
	int numBytesSent;
	printf("maxFrameIndex: %d frames.\n",data.maxFrameIndex);
	if ((numBytesSent=send(sock, &data.maxFrameIndex, sizeof(data.maxFrameIndex), 0)) != sizeof(data.maxFrameIndex)) {
		fprintf(stderr,"send() sent a different number of bytes than expected (sent %d bytes). file %s, line %d.\n",numBytesSent,__FILE__,__LINE__);
		exit(1);
	}

    err = Pa_Initialize();
    if( err != paNoError ) goto done;

    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    inputParameters.channelCount = NUM_CHANNELS;                    /* stereo input */
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    /* Record some audio. -------------------------------------------- */
    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL,                  /* no output */
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              recordCallback,
              &data );
    if( err != paNoError ) goto done;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto done;
    printf("Now recording!!\n"); fflush(stdout);
	
    while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
    {
        Pa_Sleep(10);
		/* Send unsent frames */
		if (data.tx_pos < sizeof(SAMPLE)*NUM_CHANNELS*data.frameIndex) {
			if ((numBytesSent=send(sock, data.samples+data.tx_pos, sizeof(SAMPLE)*NUM_CHANNELS*data.frameIndex-data.tx_pos, 0)) != sizeof(SAMPLE)*NUM_CHANNELS*data.frameIndex-data.tx_pos) {
				fprintf(stderr,"send() sent a different number of bytes than expected (sent %d bytes, expected %d). file %s, line %d.\n",numBytesSent,sizeof(SAMPLE)*NUM_CHANNELS*data.frameIndex-data.tx_pos,__FILE__,__LINE__);
				exit(1);
			}
			printf("\rSending byte %d of %d (sent %d bytes)          ",data.tx_pos,data.maxFrameIndex,numBytesSent); fflush(stdout);
			data.tx_pos+=numBytesSent;
		}
    }
    printf("\n");
    if( err < 0 ) goto done;

    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto done;

done:
    Pa_Terminate();
    if( data.samples )       /* Sure it is NULL or valid. */
        free( data.samples );
    if( err != paNoError )
    {
        fprintf( stderr, "An error occured while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
	    return 1;
    }

	/* close socket */
	close(sock);
}

