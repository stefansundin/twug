#ifndef message_types_h
#define message_types_h

#define CLIENT_LOGIN_REQUEST 0	//message is a string containing username and password concatenated. both should be filled out with 0 until they are 20 chars long (0 come at the end).
#define CLIENT_LOGOUT 1			//no message
#define CLIENT_AUDIO_DATA 2		//message is audio data
#define CLIENT_TEXT_DATA 3		//message is the username the text message is being sent to (filled out with 0 at the end until it is 20 chars long) concatenated with the text message.

#define CLIENT_CHANNEL_CHANGE 4	//message is the name of the channel the user wants to change to concatenated with the channel password both filled out with 0 until they are 20 chars long (filled at the end).

#define SERVER_LOGIN_OK 0		//no message
#define SERVER_LOGIN_BAD 1		//1 for bad username 2 for bad password
#define SERVER_DISCONNECT 2		//int why

#define SERVER_AUDIO_DATA 3		//audio data
#define SERVER_TEXT_DATA 4		//message is the username the text message comes from (filled out with 0 at the end until it is 20 chars long) concatenated with the text message.

#define SERVER_CHANNEL_CHANGE_RESPONSE 5	//int: 0 for ok, 1 for bad channel name, 2 for bad password, 3 for other
#define SERVER_ADD_CLIENT 6		//fill("client name", 20) + fill("channel name", 20)
#define SERVER_MOVE_CLIENT 7	//fill("client name", 20) + fill("new channel name", 20)
#define SERVER_REMOVE_CLIENT 8	//fill("client name", 20)


//this one is just to make deleting disconnected sockets fit in with the normal message system
#define SOCKET_DISCONNECTED 100

#define AUDIO_SAMPLE_RATE 44100
#endif //message_types_h

