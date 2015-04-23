/*	Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef __SERVERGAMEMODE__
#define __SERVERGAMEMODE__

typedef struct
{
	const char *ccName;			// Name of the mode.
	const char *ccDescription;	// The description of the gamemode.

	void(*StartFrame)(void);	// Called at the start of the frame (pre).
	void(*EndFrame)(void);		// Called at the end of the frame (post).

	void(*PlayerSpawn)(void);	// Called just before spawning a player.
} ServerGameMode_t;

#endif