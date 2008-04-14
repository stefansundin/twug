#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include "DataKeeper.h"

class AudioManager
{
public:
	AudioManager(DataKeeper* p_data);

private:
	DataKeeper* m_data;
};

#endif
