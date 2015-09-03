/*	Copyright (C) 2011-2015 OldTimes Software
*/

#include "server_main.h"

class CServerEntityAnimated : public CServerEntity
{
public:
private:
	int
		CurrentAnimationFrame,	// Current frame of the current sequence.
		EndAnimationFrame;		// Last frame of the current sequence.

	double AnimationTime;	// Speed/time of the animation.
};