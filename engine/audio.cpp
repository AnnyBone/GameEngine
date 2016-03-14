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
#include <AL/efx-presets.h>

/*
	Audio System
*/

ConsoleVariable_t cv_audio_volume			= { "audio_volume", "1", true };
ConsoleVariable_t cv_audio_volume_music		= { "audio_volume_music", "1", true };

Audio_t g_audio;

ALCdevice	*audio_device;
ALCcontext	*audio_context;

bool audio_initialized = false;

void Audio_PlayCommand(void);

LPALGENEFFECTS alGenEffects;
LPALDELETEEFFECTS alDeleteEffects;
LPALISEFFECT alIsEffect;
LPALEFFECTI alEffecti;
LPALEFFECTIV alEffectiv;
LPALEFFECTF alEffectf;
LPALEFFECTFV alEffectfv;
LPALGETEFFECTI alGetEffecti;
LPALGETEFFECTIV alGetEffectiv;
LPALGETEFFECTF alGetEffectf;
LPALGETEFFECTFV alGetEffectfv;

LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

void Audio_Initialize(void)
{
	if (audio_initialized || COM_CheckParm("-nosound") || COM_CheckParm("-noaudio"))
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
		ALC_FREQUENCY, AUDIO_SAMPLE_SPEED,
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

		Con_Warning("Failed to create audio buffers! (%i)\n", AUDIO_MAX_BUFFERS);
		return;
	}

	alGenSources(AUDIO_MAX_SOURCES, g_audio.sources);
	if (alGetError() != AL_NO_ERROR)
	{
		Audio_Shutdown();
		
		Con_Warning("Failed to create audio sources! (%i)\n", AUDIO_MAX_SOURCES);
		return;
	}

	// Check for extensions...

	g_audio.extensions.efx					=
	g_audio.extensions.soft_buffer_samples	= false;

	if (alcIsExtensionPresent(alcGetContextsDevice(audio_context), "ALC_EXT_EFX"))
	{
		Con_Printf(" EFX support detected!\n");

		alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");

		g_audio.extensions.efx = true;
	}
	if (alIsExtensionPresent("AL_SOFT_buffer_samples"))
		g_audio.extensions.soft_buffer_samples = true;

	// We're done, initialized!
	audio_initialized = true;
}

/*	Play a specified sound via the console.
*/
void Audio_PlayCommand(void)
{}

void Audio_PlaySound(const AudioSound_t *sample)
{
	if (!sample)
		return;

	alSourcefv(sample->source, AL_POSITION, sample->position);
	alSourcefv(sample->source, AL_VELOCITY, sample->velocity);

	// Play the source.
	alSourcePlay(g_audio.sources[sample->source]);
}

void Audio_StopSound(const AudioSound_t *sample)
{
	if (!sample)
		return;

	// Stop the source.
	alSourceStop(sample->source);
}

AudioSound_t *Audio_LoadSound(const char *path)
{
	return NULL;
}

void Audio_DeleteSound(AudioSound_t *sample)
{
	
}

/*	Called per-frame to update listener position and more!
*/
void Audio_Frame(void)
{
	MathVector3f_t	position, orientation, velocity;

	// TODO: Have nothing to assign this to yet.
	plVectorCopy3fv(pl_origin3f, velocity);

	if(cls.signon == SIGNONS)
	{
		plVectorCopy3fv(r_refdef.vieworg, position);
		plVectorCopy3fv(r_refdef.viewangles, orientation);
	}
	else
	{
		plVectorCopy3fv(pl_origin3f, position);
		plVectorCopy3fv(pl_origin3f, orientation);
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
			alDeleteSources(AUDIO_MAX_SOURCES, g_audio.sources);

			alcMakeContextCurrent(NULL);
			alcDestroyContext(audio_context);
		}

		alcCloseDevice(audio_device);
	}

	audio_initialized = false;
}
