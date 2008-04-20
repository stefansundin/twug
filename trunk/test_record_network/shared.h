

/* AUDIO */
#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (1024)
#define NUM_CHANNELS    (1)

#if 0
	#define PA_SAMPLE_TYPE  paFloat32
	typedef float SAMPLE;
	#define SAMPLE_SILENCE  (0.0f)
#elif 0
	#define PA_SAMPLE_TYPE  paInt16
	typedef short SAMPLE;
	#define SAMPLE_SILENCE  (0)
#elif 0
	#define PA_SAMPLE_TYPE  paInt8
	typedef char SAMPLE;
	#define SAMPLE_SILENCE  (0)
#else
	#define PA_SAMPLE_TYPE  paUInt8
	typedef unsigned char SAMPLE;
	#define SAMPLE_SILENCE  (128)
#endif

typedef struct {
    SAMPLE *samples;
    int frameIndex;
    int maxFrameIndex;
    int tx_pos;
    int size;
    int go;
} paBuffer;

typedef struct
{
	paBuffer record_buffer;
    paBuffer *play_buffers;
    int num_play_buffers;
} paData;

