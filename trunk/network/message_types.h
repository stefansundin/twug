#define CLIENT_LOGIN_REQUEST 0			//fill(username) + fill(password)
#define CLIENT_LOGOUT 1					//no message
#define CLIENT_AUDIO_DATA 2				//message is audio data
#define CLIENT_TEXT_DATA 3				//fill(to_user_name) + message_text

#define CLIENT_CHANNEL_CHANGE 4			//fill(channel_name) + fill(password)

#define CLIENT_ADMIN_CREATE_CHANNEL 5	//fill(channel_name)
#define CLIENT_ADMIN_REMOVE_CHANNEL 6	//fill(channel_name)
#define CLIENT_ADMIN_SET_PASSWORD 7		//fill(new_server_password)


#define SERVER_LOGIN_OK 0				//no message
#define SERVER_LOGIN_BAD 1				//1 for bad username 2 for bad password
#define SERVER_DISCONNECT 2				//int why

#define SERVER_AUDIO_DATA 3				//audio data
#define SERVER_TEXT_DATA 4				//fill(from_user_name) + message_text

#define SERVER_CHANNEL_CHANGE_RESPONSE 5	//string: "0" for ok, "1" for bad channel name, "2" for bad password, "3" for other
#define SERVER_ADD_CLIENT 6	 			//fill("client name") + fill("channel name")
#define SERVER_MOVE_CLIENT 7			//fill("client name") + fill("new channel name")
#define SERVER_REMOVE_CLIENT 8			//fill("client name")

#define SERVER_ADD_CHANNEL 9			//fill("channel name")
#define SERVER_REMOVE_CHANNEL 10		//fill("channel name")


//this one is just to make deleting disconnected sockets fit in with the normal message system
#define SOCKET_DISCONNECTED 100

#define MESSAGE_FILL 20

#define AUDIO_SAMPLE_RATE 44100

