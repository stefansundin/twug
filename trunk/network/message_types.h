#ifndef message_types_h
#define message_types_h

#define CLIENT_LOGIN_REQUEST 0			//fill(username,20) + fill(password,20)
#define CLIENT_LOGOUT 1					//no message
#define CLIENT_AUDIO_DATA 2				//message is audio data
#define CLIENT_TEXT_DATA 3				//fill(to_user_name,20) + message_text

#define CLIENT_CHANNEL_CHANGE 4			//fill(channel_name,20) + fill(password,20)

#define CLIENT_ADMIN_CREATE_CHANNEL 5		//fill(channel_name,20)
#define CLIENT_ADMIN_REMOVE_CHANNEL 6		//fill(channel_name,20)
#define CLIENT_ADMIN_SET_PASSWORD 7		//fill(new_server_password,20)


#define SERVER_LOGIN_OK 0		//no message
#define SERVER_LOGIN_BAD 1		//1 for bad username 2 for bad password
#define SERVER_DISCONNECT 2		//int why

#define SERVER_AUDIO_DATA 3		//audio data
#define SERVER_TEXT_DATA 4		////fill(from_user_name,20) + message_text

#define SERVER_CHANNEL_CHANGE_RESPONSE 5	//int: 0 for ok, 1 for bad channel name, 2 for bad password, 3 for other
#define SERVER_ADD_CLIENT 6		//fill("client name", 20) + fill("channel name", 20)
#define SERVER_MOVE_CLIENT 7	//fill("client name", 20) + fill("new channel name", 20)
#define SERVER_REMOVE_CLIENT 8	//fill("client name", 20)


//this one is just to make deleting disconnected sockets fit in with the normal message system
#define SOCKET_DISCONNECTED 100

#define AUDIO_SAMPLE_RATE 44100
#endif //message_types_h

