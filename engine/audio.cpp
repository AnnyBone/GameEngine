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

	Samples represent the actual sound data.
	Sounds represent the individual emitters of each sound.

	Confused?

	You should be.
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

	ALCdevice *device = alcOpenDevice(NULL);
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

	ALCcontext *context = alcCreateContext(device, attr);
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

	ALCcontext *context = alcGetCurrentContext();
	if (context)
	{
		ClearSounds();

		ALCdevice *device = alcGetContextsDevice(context);
		alcMakeContextCurrent(NULL);
		alcDestroyContext(context);

		if (device)
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

	// Check if there's any sounds we can delete.
#if 0
	for (unsigned int i = 0; i < sounds.size(); i++)
		if (sounds[i]->auto_delete && !IsSoundPlaying(sounds[i]))
			DeleteSound(sounds[i]);
#endif
}

AudioSound_t *AudioManager::AddSound()
{
	AudioSound_t *sound = new AudioSound_t;
	if (!sound)
		throw EngineException("Failed to allocate new sound!\n");

	sounds.push_back(sound);

	memset(sound, 0, sizeof(AudioSound_t));

	alGenSources(1, &sound->source);
	switch (alGetError())
	{
	case AL_OUT_OF_MEMORY:
		Con_Warning("There is not enough memory to generate the requested sound source!\n");
		break;
	case AL_INVALID_VALUE:
		Con_Warning("");
		break;
	case AL_INVALID_OPERATION:
		Con_Warning("Invalid context for creation of sound source!\n");
		break;
	default:break;
	}

	alGenBuffers(1, &sound->buffer);
	switch (alGetError())
	{
	case AL_INVALID_VALUE:
		Con_Warning("Sound buffer isn't large enough to hold buffer!\n");
		break;
	case AL_OUT_OF_MEMORY:
		Con_Warning("There isn't enough memory to generate the requested sound buffer!\n");
		break;
	default:break;
	}

	alGenEffects(1, &sound->effect);
	if (alGetError() != AL_NO_ERROR)
		Con_Warning("Failed to create audio effect!\n");

	sound->volume			= 1.0f;
	sound->pitch			= 1.0f;
	sound->max_distance		= 1024.0f;

	return sound;
}

void AudioManager::PlaySound(const AudioSound_t *sound)
{
	if (!sound || IsSoundPlaying(sound))
		return;

	alSourcefv(sound->source, AL_POSITION, sound->position);
	alSourcefv(sound->source, AL_VELOCITY, sound->velocity);

	alSourcef(sound->source, AL_PITCH, sound->pitch);
	alSourcef(sound->source, AL_GAIN, sound->volume);
	alSourcef(sound->source, AL_MAX_DISTANCE, sound->max_distance);

	alSourcei(sound->source, AL_BUFFER, sound->buffer);
	
	if (sound->local)
	{
		alSourcei(sound->source, AL_SOURCE_RELATIVE, AL_TRUE);
		alSourcef(sound->source, AL_ROLLOFF_FACTOR, 0);
	}
	else
	{
		alSourcei(sound->source, AL_SOURCE_RELATIVE, AL_TRUE);
		alSourcef(sound->source, AL_ROLLOFF_FACTOR, 1.0f);
	}

	// Play the source.
	alSourcePlay(sound->source);
}

void AudioManager::PlaySound(ClientEntity_t *entity, const AudioSample_t *cache, float volume)
{
	if (!cache)
	{
		Con_Warning("Invalid sound!\n");
		return;
	}

	AudioSound_t *sound = AddSound();
	sound->auto_delete	= true;
	sound->volume		= volume;
	sound->cache		= cache;

	plVectorCopy3fv(entity->origin, sound->position);

	PlaySound(sound);
}

void AudioManager::PlaySound(const char *path)
{
	if (!path || (path[0] == ' '))
	{
		Con_Warning("Invalid path for sound!\n");
		return;
	}

	AudioSound_t *sound = AddSound();
	sound->auto_delete	= true;
	sound->local		= true;

	LoadSound(sound, path);
	PlaySound(sound);
}

void AudioManager::LoadSound(AudioSound_t *sound, const char *path)
{
	if (!path || (path[0] == ' '))
	{
		Con_Warning("Invalid path for sound!\n");
		return;
	}
	else if (!sound)
		return;

	sound->cache = PrecacheSample(path);
	if (!sound->cache)
	{
		Con_Warning("Failed to load sound! (%s)\n", path);
		return;
	}

	// Check the format.
	int format = AL_FORMAT_MONO16;
	if (sound->cache->width == 1)
		format = AL_FORMAT_MONO8;

	alBufferData(sound->buffer, format, sound->cache->data, sound->cache->size, sound->cache->speed);
	switch (alGetError())
	{
	case AL_OUT_OF_MEMORY:
		Con_Warning("There is not enough memory avaliable to create this audio buffer!\n");
		break;
	case AL_INVALID_VALUE:
		Con_Warning("The size parameter is not valid for the format specified, the audio buffer is in use, or the data is a NULL pointer!\n");
		break;
	case AL_INVALID_ENUM:
		Con_Warning("The specified format does not exist!\n");
		break;
	default:break;
	}
}

void AudioManager::StopSound(const AudioSound_t *sound)
{
	if (!sound || !IsSoundPlaying(sound))
		return;

	// Stop the source.
	alSourceStop(sound->source);
}

void AudioManager::PauseSound(const AudioSound_t *sound)
{
	if (!sound || !IsSoundPlaying(sound))
		return;

	// Pause the source.
	alSourcePause(sound->source);
}

void AudioManager::DeleteSound(AudioSound_t *sound)
{
	if (!sound)
		throw EngineException("Attempted to delete an invalid sound!\n");

	StopSound(sound);

	// Delete OpenAL-specific data.
	if (alIsBuffer(sound->buffer))
		alDeleteBuffers(1, &sound->buffer);
	if (alIsEffect(sound->effect))
		alDeleteEffects(1, &sound->effect);
	if (alIsSource(sound->source))
		alDeleteSources(1, &sound->source);

	// Set cache as null.
	sound->cache = nullptr;

	// Remove it from the list.
	for (auto iterator = sounds.begin(); iterator != sounds.end(); iterator++)
		if (sound == *iterator)
		{
			sounds.erase(iterator);
			break;
		}
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

AudioSample_t *AudioManager::FindSample(const char *path)
{
	if (!path || (path[0] == ' '))
	{
		Con_Warning("Invalid path for audio sample!\n");
		return nullptr;
	}

	// See if we've already loaded the sample.
	auto sample = samples.find(path);
	if (sample != samples.end())
		return sample->second;

	return nullptr;
}

AudioSample_t *AudioManager::PrecacheSample(const char *path)
{
	if (!path || (path[0] == ' '))
	{
		Con_Warning("Invalid path for audio sample!\n");
		return nullptr;
	}

	AudioSample_t *cache = FindSample(path);
	if (cache)
		return cache;

	cache = LoadSample(path);
	return cache;
}

AudioSample_t *AudioManager::LoadSample(const char *path)
{
	if (!path || (path[0] == ' '))
	{
		Con_Warning("Invalid path for audio sample!\n");
		return nullptr;
	}

	std::string complete_path(g_state.path_sounds);
	complete_path += path;

	uint8_t *data = (uint8_t*)COM_LoadHeapFile(complete_path.c_str());
	if (!data)
	{
		Con_Warning("Failed to load audio sample! (%s)\n", path);
		return nullptr;
	}

	wavinfo_t info = GetWavinfo(path, data, com_filesize);
	float stepscale = (float)info.rate / AUDIO_SAMPLE_SPEED;
	int len = info.samples / stepscale;
	len *= info.width * info.channels;
	
	AudioSample_t *cache = new AudioSample_t;
	cache->length		= info.samples;		// Length of the sample.
	cache->loopstart	= info.loopstart;
	cache->speed		= info.rate;		// Rate of the sample.
	cache->width		= info.width;
	cache->stereo		= info.channels;	// Stereo / Mono.
	cache->size			= com_filesize;		// Size of the sample.

	strncpy(cache->path, path, sizeof(cache->path));

	// Add it to the global list.
	samples.emplace(path, cache);

	// Now resample the sample...
#if 1
	uint8_t *dataofs = data + info.dataofs;

	stepscale = (float)cache->speed / AUDIO_SAMPLE_SPEED;
	int outcount = cache->length / stepscale;
	cache->length = outcount;
	if (cache->loopstart != -1)
		cache->loopstart = cache->loopstart / stepscale;

	cache->id		= samples.size();
	cache->speed	= AUDIO_SAMPLE_SPEED;
	cache->stereo	= 0;

	// Resample / decimate to the current source rate.
	if ((stepscale == 1) && (cache->width == 1))
	{
		// Fast special case.
		for (int i = 0; i < outcount; i++)
			((signed char *)data)[i] = (int)((unsigned char)(dataofs[i]) - 128);
	}
	else
	{
		// General case.
		int 
			samplefrac = 0, 
			srcsample, sample,
			fracstep = stepscale * 256;
		for (int i = 0; i < outcount; i++)
		{
			srcsample = samplefrac >> 8;
			samplefrac += fracstep;
			if (cache->width == 2)
			{
				sample = LittleShort(((short*)dataofs)[srcsample]);
				((short*)data)[i] = sample;
			}
			else
			{
				sample = (int)((unsigned char)(dataofs[srcsample]) - 128) << 8;
				((signed char*)data)[i] = sample >> 8;
			}
		}
	}
#else
/*	if (cache->width == 2)
	{
		int num_samples = info.samples;
		if (LittleShort(256) != 256)
		{
			if (info.channels == 2)
				num_samples <<= 1;

			for (int i = 0; i < info.samples; i++)
				((short*)data)[i] = LittleShort(((short*)data)[i]);
		}
	}*/
#endif
	cache->data = data;

	return cache;
}

void AudioManager::DeleteSample(AudioSample_t *sample)
{
	samples.erase(sample->path);
}

void AudioManager::ClearSamples()
{
#if 0
	for (auto &sample : samples)
		DeleteSample(sample.second);
#endif

	samples.clear();
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
	int	mem = 0, size;

	for (auto &sample : samples)
	{
		size = sample.second->length * sample.second->width * (sample.second->stereo + 1);
		mem += size;
		Con_SafePrintf(" Sample : ");
		if (sample.second->loopstart >= 0)
			Con_SafePrintf("Loop(true) ");
		else
			Con_SafePrintf("Loop(false) ");
		Con_SafePrintf("Length(%i) Speed(%i) Stereo(%i) Size(%6i)\n",
			sample.second->length, 
			sample.second->speed,
			sample.second->stereo,
			size);
	}

	Con_Printf("\n");

	for (unsigned int i = 0; i < sounds.size(); i++)
		Con_SafePrintf(" Sound : Volume(%5.1f) Velocity(%i %i %i) Position(%i %i %i) Pitch(%5.1f)\n",
			sounds[i]->volume,
			(int)sounds[i]->velocity[0],
			(int)sounds[i]->velocity[1],
			(int)sounds[i]->velocity[2],
			(int)sounds[i]->position[0],
			(int)sounds[i]->position[1],
			(int)sounds[i]->position[2],
			sounds[i]->pitch);

	Con_Printf("%i sounds, %i bytes\n", samples.size(), mem);
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

void Audio_PlayTemporarySound(ClientEntity_t *entity, const AudioSample_t *cache, float volume)
{
	g_audiomanager->PlaySound(entity, cache, volume);
}

void Audio_StopSound(const AudioSound_t *sample)
{
	g_audiomanager->StopSound(sample);
}

void Audio_StopSounds(void)
{
	g_audiomanager->StopSounds();
}

AudioSample_t *Audio_PrecacheSample(const char *path)
{
	return g_audiomanager->PrecacheSample(path);
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
