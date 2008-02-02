#ifndef MESSAGE_TYPES_H_
#define MESSAGE_TYPES_H_

#define OUT_LOGIN_REQUEST 0	//message is a string containing username and password concatenated. both should be filled out with 0 until they are 20 chars long (0 come at the end).
#define OUT_LOGOUT 1			//no message
#define OUT_AUDIO_DATA 2		//message is audio data
#define OUT_TEXT_DATA 3		//message is the username the text message is being sent to (filled out with 0 at the end until it is 20 chars long) concatenated with the text message.

#define OUT_CHANNEL_CHANGE 4	//message is the name of the channel the user wants to change to concatenated with the channel password both filled out with 0 until they are 20 chars long (filled at the end).

#define INC_LOGIN_OK 0		//no message
#define INC_LOGIN_BAD 1		//1 for bad username 2 for bad password
#define INC_DISCONNECT 2		//int why

#define INC_AUDIO_DATA 3		//audio data
#define INC_TEXT_DATA 4		//message is the username the text message comes from (filled out with 0 at the end until it is 20 chars long) concatenated with the text message.

#define INC_BUDDY_LIST_UPDATE 5	//
#endif /*MESSAGE_TYPES_H_*/


