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

static int recordCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    paTestData *data = (paTestData*)userData;
    const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
    SAMPLE *wptr = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
    long framesToCalc;
    long i;
    int finished;
    unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;

    (void) outputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

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

    PaStreamParameters  inputParameters;
    PaStream*           stream;
    PaError             err = paNoError;
    paTestData          data;
    int                 i;
    int                 totalFrames;
    int                 numSamples;
    int                 numBytes;
    SAMPLE              max, val;
    double              average;

    data.maxFrameIndex = totalFrames = NUM_SECONDS * SAMPLE_RATE; /* Record for a few seconds. */
    data.frameIndex = 0;
    numSamples = totalFrames * NUM_CHANNELS;
    numBytes = numSamples * sizeof(SAMPLE);
    data.recordedSamples = (SAMPLE*) malloc( numBytes ); /* From now on, recordedSamples is initialised. */
    if( data.recordedSamples == NULL )
    {
        printf("Could not allocate record array.\n");
        goto done;
    }
    for( i=0; i<numSamples; i++ ) data.recordedSamples[i] = 0;

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
        Pa_Sleep(1000);
        printf("index = %d\n", data.frameIndex ); fflush(stdout);
    }
    if( err < 0 ) goto done;

    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto done;

	/* write to file */
    FILE  *fid;
    fid = fopen("recorded-client.raw", "wb");
    if( fid == NULL )
    {
        printf("Could not open file.");
    }
    else
    {
        fwrite( data.recordedSamples, 1, numBytes, fid );
        fclose( fid );
        printf("Wrote data to 'recorded-client.raw'\n");
    }

done:
    Pa_Terminate();
//    if( data.recordedSamples )       /* Sure it is NULL or valid. */
//        free( data.recordedSamples );
    if( err != paNoError )
    {
        fprintf( stderr, "An error occured while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
	    return 1;
    }

	/* Send recordedSamples */
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

	int numBytesSent;
	/* Send the length of recordedSamples */
	printf("recordedSamples length: %d.\n",numBytes);
	if ((numBytesSent=send(sock, &numBytes, sizeof(numBytes), 0)) != sizeof(numBytes)) {
		fprintf(stderr,"send() sent a different number of bytes than expected (sent %d bytes). file %s, line %d.\n",numBytesSent,__FILE__,__LINE__);
		exit(1);
	}

	/* Send recordedSamples */
	int messagepos=0;
	printf("Sending message: 0");
	while (messagepos < numBytes) {
		int bytestosend=1000;
		if (messagepos+bytestosend > numBytes) {
			bytestosend=numBytes-messagepos;
		}
		if ((numBytesSent=send(sock, data.recordedSamples+messagepos, bytestosend, 0)) != bytestosend) {
			fprintf(stderr,"send() sent a different number of bytes than expected (sent %d bytes, expected %d). file %s, line %d.\n",numBytesSent,bytestosend,__FILE__,__LINE__);
			exit(1);
		}
		messagepos+=bytestosend;
		printf("\rSending message: %d (%d)             ",messagepos,bytestosend);
	}
	printf("\n");

	/* close socket */
	close(sock);
}

