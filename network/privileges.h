//Privilege defines
#define PRIV_NONE				0
#define PRIV_TALK				1
#define PRIV_TEXT				2
#define PRIV_CREATE_CHANNEL		4
#define PRIV_REMOVE_CHANNEL		8
#define PRIV_TEXT_BROADCAST		16
#define PRIV_KICK				32
#define PRIV_ALL				PRIV_TALK | PRIV_TEXT | PRIV_CREATE_CHANNEL \
								| PRIV_REMOVE_CHANNEL | PRIV_TEXT_BROADCAST | PRIV_KICK

