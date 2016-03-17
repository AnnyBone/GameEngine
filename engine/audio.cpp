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

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

/*
	Audio System
*/

ConsoleVariable_t cv_audio_volume			= { "audio_volume", "1", true };
ConsoleVariable_t cv_audio_volume_music		= { "audio_volume_music", "1", true };

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

using namespace Core;

void Audio_PlaySoundCommand(void);
void Audio_ListSoundsCommand(void);

ALCdevice	*device;
ALCcontext	*context;

AudioManager::AudioManager()
{
	extensions.efx					=
	extensions.soft_buffer_samples	= false;

	if (COM_CheckParm("-nosound") || COM_CheckParm("-noaudio"))
		return;

	Con_Printf("Initializing Audio Manager...\n");

	Cmd_AddCommand("audio_playsound", Audio_PlaySoundCommand);
	Cmd_AddCommand("audio_listsounds", Audio_ListSoundsCommand);

	// Reserve up to AUDIO_MAX_SOUNDS, and we can expand on this as necessary.
	sounds.reserve(AUDIO_MAX_SOUNDS * 2);
	samples.reserve(AUDIO_MAX_SOUNDS);

	device = alcOpenDevice(NULL);
	if (!device)
	{
		Con_Warning("Failed to open audio device!\n");
		return;
	}

	int attr[] =
	{
		ALC_FREQUENCY, AUDIO_SAMPLE_SPEED,
		0
	};

	context = alcCreateContext(device, attr);
	if (!context || alcMakeContextCurrent(context) == FALSE)
		throw EngineException("Failed to create audio context!\n");

	// Check for extensions...
	if (alcIsExtensionPresent(alcGetContextsDevice(context), "ALC_EXT_EFX"))
	{
		Con_Printf(" EFX support detected!\n");

		alGenEffects		= (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
		alDeleteEffects		= (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
		alIsEffect			= (LPALISEFFECT)alGetProcAddress("alIsEffect");
		alEffecti			= (LPALEFFECTI)alGetProcAddress("alEffecti");

		extensions.efx = true;
	}
	if (alIsExtensionPresent("AL_SOFT_buffer_samples"))
		extensions.soft_buffer_samples = true;
}

AudioManager::~AudioManager()
{
	Con_Printf("Shutting down Audio Manager...\n");

	if (device)
	{
		if (context)
		{
			ClearSounds();

			alcMakeContextCurrent(NULL);
			alcDestroyContext(context);
		}

		alcCloseDevice(device);
	}
}

void AudioManager::Frame()
{
	MathVector3f_t	position, orientation, velocity;

	// TODO: Have nothing to assign this to yet.
	plVectorCopy3fv(pl_origin3f, velocity);

	if (cls.signon == SIGNONS)
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

AudioSound_t *AudioManager::AddSound()
{
	AudioSound_t *sample = new AudioSound_t;
	sounds.push_back(sample);

	memset(sample, 0, sizeof(AudioSound_t));

	alGenSources(1, &sample->source);
	if (alGetError() != AL_NO_ERROR)
		Con_Warning("Failed to create audio source!\n");

	alGenBuffers(1, &sample->buffer);
	if (alGetError() != AL_NO_ERROR)
		Con_Warning("Failed to create audio buffer!\n");

	alGenEffects(1, &sample->effect);
	if (alGetError() != AL_NO_ERROR)
		Con_Warning("Failed to create audio effect!\n");

	sample->volume			= 1.0f;
	sample->pitch			= 1.0f;
	sample->max_distance	= 1024.0f;

	return sample;
}

void AudioManager::PlaySound(const AudioSound_t *sample)
{
	if (!sample || IsSoundPlaying(sample))
		return;

	alSourcefv(sample->source, AL_POSITION, sample->position);
	alSourcefv(sample->source, AL_VELOCITY, sample->velocity);

	alSourcef(sample->source, AL_PITCH, sample->pitch);
	alSourcef(sample->source, AL_GAIN, sample->volume);
	alSourcef(sample->source, AL_MAX_DISTANCE, sample->max_distance);

	// Play the source.
	alSourcePlay(sample->source);
}

void AudioManager::PlaySound(const char *path)
{
	if (!path || (path[0] == ' '))
	{
		Con_Warning("Invalid path for sound!\n");
		return;
	}

	AudioSound_t *sample = AddSound();
	LoadSound(sample, path);
	PlaySound(sample);
}

void AudioManager::LoadSound(AudioSound_t *sample, const char *path)
{
	if (!path || (path[0] == ' '))
	{
		Con_Warning("Invalid path for sound!\n");
		return;
	}
	else if (!sample)
		return;

	sample->sfx = Audio_PrecacheSample(path);
	if (!sample->sfx)
	{
		Con_Warning("Failed to load sound! (%s)\n", path);
		return;
	}

	sfxcache_t *cache = S_LoadSound(sample->sfx);
	if (!cache)
	{
		Con_Warning("Failed to load sound data! (%s)\n", path);

		sample->sfx = nullptr;
		return;
	}

	alBufferData(sample->buffer, AL_FORMAT_MONO16, cache->data, cache->length, cache->speed);
}

void AudioManager::StopSound(const AudioSound_t *sample)
{
	if (!sample || !IsSoundPlaying(sample))
		return;

	// Stop the source.
	alSourceStop(sample->source);
}

void AudioManager::PauseSound(const AudioSound_t *sample)
{
	if (!sample || !IsSoundPlaying(sample))
		return;

	// Pause the source.
	alSourcePause(sample->source);
}

void AudioManager::DeleteSound(AudioSound_t *sample)
{
	if (!sample)
		return;

	StopSound(sample);

	// Delete OpenAL-specific data.
	if (alIsBuffer(sample->buffer))
		alDeleteBuffers(1, &sample->buffer);
	if (alIsEffect(sample->effect))
		alDeleteEffects(1, &sample->effect);
	if (alIsSource(sample->source))
		alDeleteSources(1, &sample->source);

	// Remove it from the list.
	for (auto iterator = sounds.begin(); iterator != sounds.end(); iterator++)
		if (sample == *iterator)
			sounds.erase(iterator);
}

bool AudioManager::IsSoundPlaying(const AudioSound_t *sample)
{
	int state;

	alGetSourcei(sample->source, AL_SOURCE_STATE, &state);
	if (state == AL_PLAYING)
		return true;

	return false;
}

bool AudioManager::IsSoundPaused(const AudioSound_t *sample)
{
	int state;

	alGetSourcei(sample->source, AL_SOURCE_STATE, &state);
	if (state == AL_PAUSED)
		return true;
	
	return false;
}

// Samples

sfx_t *AudioManager::FindSample(const char *path)
{
	if (!path || (path[0] == ' '))
	{
		Con_Warning("Invalid path for audio sample!\n");
		return nullptr;
	}

	// See if we've already loaded the sample.
	for (unsigned int i = 0; i < samples.size(); i++)
		if (!strcmp(samples[i]->name, path))
			return samples[i];

	sfx_t *sfx = new sfx_t;
	samples.push_back(sfx);

	strncpy(sfx->name, path, sizeof(sfx->name));

	return sfx;
}

sfx_t *AudioManager::PrecacheSample(const char *path)
{
	if (!path || (path[0] == ' '))
	{
		Con_Warning("Invalid path for audio sample!\n");
		return nullptr;
	}

	sfx_t *sfx = FindSample(path);
	S_LoadSound(sfx);
	return sfx;
}

void AudioManager::TouchSample(const char *path)
{
	if (!path || (path[0] == ' '))
	{
		Con_Warning("Invalid path for audio sample!\n");
		return;
	}

	sfx_t *sfx = FindSample(path);
	Cache_Check(&sfx->cache);
}

// Global

void AudioManager::StopSounds()
{
	for (unsigned int i = 0; i < sounds.size(); i++)
		StopSound(sounds[i]);
}

void AudioManager::ClearSounds()
{
	for (unsigned int i = 0; i < sounds.size(); i++)
		DeleteSound(sounds[i]);

	sounds.clear();
	sounds.shrink_to_fit();	// Free up mem.
}

void AudioManager::ListSounds()
{
	int			mem = 0, size;
	sfxcache_t	*sc;

	for (unsigned int i = 0; i < sounds.size(); i++)
	{
		sfx_t *sfx = sounds[i]->sfx;
		if (!sfx)
			continue;

		sc = (sfxcache_t*)Cache_Check(&sfx->cache);
		if (!sc)
			continue;
		size = sc->length * sc->width * (sc->stereo + 1);
		mem += size;
		if (sc->loopstart >= 0)
			Con_SafePrintf("L");
		else
			Con_SafePrintf(" ");
		Con_SafePrintf("(%2db) %6i : %s\n", sc->width * 8, size, sfx->cache);
	}

	Con_Printf("%i sounds, %i bytes\n", sounds.size(), mem);
}

/*
	Wrapper Functions
*/

AudioManager *g_audiomanager;

void Audio_Initialize(void)
{
	g_audiomanager = new AudioManager();
}

void Audio_PlaySoundCommand(void)
{
	char name[256];

	int i = 1;
	while (i < Cmd_Argc())
	{
		if (!strrchr(Cmd_Argv(i), '.'))
		{
			strcpy(name, Cmd_Argv(i));
			strcat(name, ".wav");
		}
		else
			strcpy(name, Cmd_Argv(i));

		g_audiomanager->PlaySound(name);

		i++;
	}
}

void Audio_ListSoundsCommand(void)
{
	g_audiomanager->ListSounds();
}

void Audio_PlayLocalSound(const char *path)
{
	g_audiomanager->PlaySound(path);
}

void Audio_StopSound(const AudioSound_t *sample)
{
	g_audiomanager->StopSound(sample);
}

void Audio_StopSounds(void)
{
	g_audiomanager->StopSounds();
}

void Audio_LoadSound(AudioSound_t *sample, const char *path)
{}

sfx_t *Audio_PrecacheSample(const char *path)
{
	return g_audiomanager->PrecacheSample(path);
}

void Audio_TouchSample(const char *path)
{
	g_audiomanager->TouchSample(path);
}

/*	Deletes the sound.
*/
void Audio_DeleteSound(AudioSound_t *sample)
{
	g_audiomanager->DeleteSound(sample);
}

/*	Called per-frame to update listener position and more!
*/
void Audio_Frame(void)
{
	g_audiomanager->Frame();
}

/*	Called during shutdown.
*/
void Audio_Shutdown(void)
{
	delete g_audiomanager;
}
