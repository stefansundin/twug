#ifndef MESSAGE_TYPES_H_
#define MESSAGE_TYPES_H_

//these are only outgoing messages
int CONNECT = 0;
/*
message is a string containing username and password concatenated.
both should be filled out with 0 until they are 20 chars long (0 come at the end).
*/
int DISCONNECT = 1;
/* no message */
int AUDIO_DATA = 2;
/* message is audio data*/
int TEXT_DATA = 3;
/*
message is the username the text message is being sent to (filled out with 0 at the end
until it is 20 chars long) concatenated with the text message.
*/
int CHANNEL_CHANGE = 4;
/*
message is the name of the channel the user wants to change to concatenated with the channel
password both filled out with 0 until they are 20 chars long (filled at the end).
*/

#endif /*MESSAGE_TYPES_H_*/