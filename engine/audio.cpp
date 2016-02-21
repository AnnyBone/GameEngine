/*	Copyright (C) 2011-2016 OldTimes Software

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "engine_base.h"

#include "audio.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

/*
	Audio System
	Unfinished
*/

#define AUDIO_NUM_BUFFERS	1
#define AUDIO_NUM_SOURCES	512

Audio_t g_audio;

ALCdevice	*audio_device;
ALCcontext	*audio_context;

bool audio_initialized = false;

void Audio_PlayCommand(void);

void Audio_Initialize(void)
{
	if (audio_initialized)
		return;

	Con_Printf("Initializing audio...\n");

	Cmd_AddCommand("audio_play", Audio_PlayCommand);

	audio_device = alcOpenDevice(NULL);
	if (!audio_device)
	{
		Con_Warning("Failed to open audio device!\n");
		return;
	}

	int attr[]=
	{
		ALC_FREQUENCY,	AUDIO_SAMPLE_RATE,
		0
	};
	
	audio_context = alcCreateContext(audio_device, attr);
	if (!audio_context || alcMakeContextCurrent(audio_context) == FALSE)
	{
		Audio_Shutdown();

		Con_Warning("Failed to create audio context!\n");
		return;
	}

	alGenBuffers(AUDIO_MAX_BUFFERS, g_audio.buffers);
	if (alGetError() != AL_NO_ERROR)
	{
		Audio_Shutdown();

		Con_Warning("Failed to create audio buffers!\n");
		return;
	}

	audio_initialized = true;
}

/*	Play a specified sound via the console.
*/
void Audio_PlayCommand(void)
{}

void Audio_PlaySound(AudioSound_t *sample)
{}

AudioSound_t *Audio_LoadSound(const char *path)
{
	return NULL;
}

/*	Called per-frame to update listener position and more!
*/
void Audio_Frame(void)
{
	MathVector3f_t	position, orientation, velocity;

	// TODO: Have nothing to assign this to yet.
	Math_VectorCopy(pl_origin3f, velocity);

	if(cls.signon == SIGNONS)
	{
		Math_VectorCopy(r_refdef.vieworg, position);
		Math_VectorCopy(r_refdef.viewangles, orientation);
	}
	else
	{
		Math_VectorCopy(pl_origin3f, position);
		Math_VectorCopy(pl_origin3f, orientation);
	}
	
	alListenerfv(AL_POSITION, position);
	alListener3f(AL_ORIENTATION, orientation[0], orientation[1], orientation[2]);
	alListenerfv(AL_VELOCITY, velocity);
}

void Audio_Shutdown(void)
{
	Con_Printf("Shutting down audio...\n");

	if (audio_device)
	{
		if (audio_context)
		{
			alDeleteBuffers(AUDIO_MAX_BUFFERS, g_audio.buffers);

			alcMakeContextCurrent(NULL);
			alcDestroyContext(audio_context);
		}

		alcCloseDevice(audio_device);
	}

	audio_initialized = false;
}
