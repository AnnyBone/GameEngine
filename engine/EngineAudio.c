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

#include "EngineAudio.h"
#include "SDL.h"

/*
	Audio System
	Unfinished
	OpenAL or SDL? :(
*/

#define AUDIO_NUM_BUFFERS	1
#define AUDIO_NUM_SOURCES	512

bool bAudioInitialized = false;

void Audio_PlayCommand(void);

void Audio_Initialize(void)
{
	if(bAudioInitialized)
		return;

	Cmd_AddCommand("audio_play",Audio_PlayCommand);

	if(SDL_AudioInit(NULL) < 0)
		Sys_Error("Failed to initialize audio!\n%s\n",SDL_GetError());

	bAudioInitialized = true;
}

/*	Play a specified sound via the console.
*/
void Audio_PlayCommand(void)
{
}

void Audio_PlaySound(AudioSound_t *asSample)
{
	sfxcache_t	*sSoundCache;

	if(!asSample)
		return;

	sSoundCache = S_LoadSound(asSample->sSample);
	if(!sSoundCache)
	{
		Con_Warning("Failed to load sound data (%s)!\n",asSample->sSample->name);
		return;
	}

	Audio.iAudioSource++;
}

AudioSound_t *Audio_LoadSound(sfx_t *sSoundEffect)
{
	AudioSound_t *asNewSound = NULL;
	uint8_t	
		*bData,
		bStackBuffer[1*1024];
//	sfxcache_t		*sSoundCache;
	char cNameBuffer[256];

//	sSoundCache = (sfxcache_t*)Cache_Check(&sSoundEffect->cache);
//	if(sSoundCache)
//		return sSoundCache;

	sprintf(cNameBuffer,"%s",Global.cSoundPath);
	strncat(cNameBuffer,sSoundEffect->name,sizeof(sSoundEffect->name));

	bData = COM_LoadStackFile(cNameBuffer,bStackBuffer,sizeof(bStackBuffer));
	if(!bData)
	{
		Con_Warning("Failed to load %s!\n",cNameBuffer);
		return NULL;
	}

	//------

	return asNewSound;
}

/*	Called per-frame to update listener position and more!
*/
void Audio_Frame(void)
{
#if 0
	vec3_t	vPosition,
			vOrientation;

	if(cls.signon == SIGNONS)
	{
		Math_VectorCopy(r_origin,vPosition);
		Math_VectorCopy(r_refdef.viewangles,vOrientation);
	}
	else
	{
		Math_VectorCopy(g_mvOrigin3f,vPosition);
		Math_VectorCopy(g_mvOrigin3f,vOrientation);
	}

	// TODO: Update listener position.
#endif
}

void Audio_Shutdown(void)
{
	if(!bAudioInitialized)
		return;

	SDL_CloseAudio();

	bAudioInitialized = false;
}
