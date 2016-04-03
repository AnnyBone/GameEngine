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

	TODO:
		- Add 'channel' override to support some hacky shit
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

	ALCdevice *device = alcOpenDevice(NULL);
	if (!device)
		throw EngineException("Failed to open audio device!\n");

	// Check for extensions...
	if (alcIsExtensionPresent(device, "ALC_EXT_EFX"))
	{
		Con_Printf(" EFX support detected!\n");

		alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
		alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
		alIsEffect = (LPALISEFFECT)alGetProcAddress("alIsEffect");
		alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");
		alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");

		alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
		alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress("alDeleteAuxiliaryEffectSlots");
		alIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT)alGetProcAddress("alIsAuxiliaryEffectSlot");
		alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");

#if 0
		alGenAuxiliaryEffectSlots(1, &sound->effect_slot);
		if (alGetError() != AL_NO_ERROR)
			Con_Warning("Failed to create audio effect slot!\n");
#endif

		extensions.efx = true;
	}

	int attr[] =
	{
		ALC_FREQUENCY,				AUDIO_SAMPLE_SPEED,
		ALC_MAX_AUXILIARY_SENDS,	4,
		0
	};

	ALCcontext *context = alcCreateContext(device, attr);
	if (!context || alcMakeContextCurrent(context) == FALSE)
		throw EngineException("Failed to create audio context!\n");

	if (alIsExtensionPresent("AL_SOFT_buffer_samples"))
		extensions.soft_buffer_samples = true;

	alDopplerFactor(4.0f);
	alDopplerVelocity(350.0f);

	Cmd_AddCommand("audio_playsound", Audio_PlaySoundCommand);
	Cmd_AddCommand("audio_listsounds", Audio_ListSoundsCommand);
	Cmd_AddCommand("audio_stopsounds", Audio_StopSounds);

	// Reserve up to AUDIO_MAX_SOUNDS, and we can expand on this as necessary.
	sounds.reserve(AUDIO_MAX_SOUNDS * 2);
	samples.reserve(AUDIO_MAX_SOUNDS);

	effect_global = AddEffect();
	SetEffectReverb(effect_global, AUDIO_REVERB_AUDITORIUM);
}

AudioManager::~AudioManager()
{
	Con_Printf("Shutting down Audio Manager...\n");

	DeleteEffect(effect_global);

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
	plVector3f_t position, orientation, velocity;

	if (cls.signon == SIGNONS)
	{
		plVectorCopy3fv(r_refdef.vieworg, position);
		plVectorCopy3fv(r_refdef.viewangles, orientation);
		plVectorCopy3fv(cl.velocity, velocity);
	}
	else
	{
		plVectorCopy3fv(pl_origin3f, position);
		plVectorCopy3fv(pl_origin3f, orientation);
		plVectorCopy3fv(pl_origin3f, velocity);
	}

	// Convert orientation to something OpenAL can use.
	plVector3f_t forward, right, up;
	plAngleVectors(orientation, forward, right, up);
	float lis_orientation[6];
	lis_orientation[0] = forward[0]; lis_orientation[3] = up[0];
	lis_orientation[1] = forward[1]; lis_orientation[4] = up[1];
	lis_orientation[2] = forward[2]; lis_orientation[5] = up[2];

	alListenerfv(AL_POSITION, position);
	alListenerfv(AL_ORIENTATION, lis_orientation);
	alListenerfv(AL_VELOCITY, velocity);

	// Check if there's any sounds we can delete.
	for (unsigned int i = 0; i < sounds.size(); i++)
	{
		if ((sounds[i]->preserve == false) && (!IsSoundPlaying(sounds[i]) && !IsSoundPaused(sounds[i])))
			DeleteSound(sounds[i]);
#if 0
		else if (IsSoundPlaying(sounds[i]))
		{
			if (sounds[i]->entity)
				SetSoundPosition(sounds[i], sounds[i]->entity->origin);
		}
#endif
	}
}

AudioSound_t *AudioManager::AddSound()
{
	AudioSound_t *sound = new AudioSound_t;
	if (!sound)
		throw EngineException("Failed to allocate new sound!\n");

	memset(sound, 0, sizeof(AudioSound_t));
	sounds.push_back(sound);

	alGenSources(1, &sound->source);
	switch (alGetError())
	{
	case AL_OUT_OF_MEMORY:
		Con_Warning("There is not enough memory to generate the requested sound source!\n");
		break;
	case AL_INVALID_VALUE:
		Con_Warning("someshittywarning.lol\n");
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

	sound->volume			= 1.0f;
	sound->pitch			= 1.0f;
	sound->max_distance		= 0.5f;
	sound->channel			= CHAN_AUTO;
	sound->ident			= -1;

	return sound;
}

void AudioManager::SetSoundPosition(AudioSound_t *sound, plVector3f_t position)
{
	// Check that it's actually moved.
	if (plVectorCompare(position, sound->current_position))
		return;

	alSourcefv(sound->source, AL_POSITION, position);
	
	// Keep cur position updated.
	plVectorCopy3fv(position, sound->current_position);
}

void AudioManager::SetSoundVelocity(AudioSound_t *sound, plVector3f_t velocity)
{
	// Check that current velocity has changed.
	if (plVectorCompare(velocity, sound->current_velocity))
		return;

	alSourcefv(sound->source, AL_VELOCITY, velocity);

	// Keep cur velocity updated.
	plVectorCopy3fv(velocity, sound->current_velocity);
}

void AudioManager::PlaySound(const AudioSound_t *sound)
{
	if (!sound || !sound->cache || IsSoundPlaying(sound))
		return;

	alSourcef(sound->source, AL_PITCH, sound->pitch);
	alSourcef(sound->source, AL_GAIN, sound->volume);
	alSourcef(sound->source, AL_REFERENCE_DISTANCE, 300.0f);
	//alSourcef(sound->source, AL_MAX_DISTANCE, sound->max_distance);

	alSourcei(sound->source, AL_BUFFER, sound->buffer);
	if (sound->cache->loopstart >= 0)
		alSourcei(sound->source, AL_LOOPING, AL_TRUE);
	
	if (sound->local)
	{
		alSourcei(sound->source, AL_SOURCE_RELATIVE, AL_TRUE);
		alSourcef(sound->source, AL_ROLLOFF_FACTOR, 0);
	}
	else
	{
		alSourcei(sound->source, AL_SOURCE_RELATIVE, AL_FALSE);
		alSourcef(sound->source, AL_ROLLOFF_FACTOR, 1.0f);
	}

	alAuxiliaryEffectSloti(sound->effect_slot, AL_EFFECTSLOT_EFFECT, effect_global->id);
	alSource3i(sound->source, AL_AUXILIARY_SEND_FILTER, sound->effect_slot, 0, 0);

	if ((sound->channel != CHAN_AUTO) && (sound->ident != -1))
	{
		// See if a sound is already playing on this channel.
		for (unsigned int i = 0; i < sounds.size(); i++)
		{

		}
	}

	// Play the source.
	alSourcePlay(sound->source);
}

void AudioManager::PlaySound(const char *path)
{
	if (!path || (path[0] == ' '))
	{
		Con_Warning("Invalid path for sound!\n");
		return;
	}

	AudioSound_t *sound		= AddSound();
	sound->local			= true;

	PrecacheSample(path, false);
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
		Sys_Error("Passed invalid sound pointer to LoadSound!\n");

	sound->cache = FindSample(path);
	if (!sound->cache)
	{
		Con_Warning("Failed to load sound, please ensure it's been cached! (%s)\n", path);
		return;
	}

#if 0
	Con_Printf("PATH: %s\n", sound->cache->path);
	Con_Printf("LENGTH: %i\n", sound->cache->length);
	Con_Printf("DATA: %p\n", sound->cache->data);
#endif

	// Check the format.
	int format = AL_FORMAT_MONO16;
	if (sound->cache->width == 1)
		format = AL_FORMAT_MONO8;

	alBufferData(sound->buffer, format, sound->cache->data, sound->cache->length * 2, sound->cache->speed);
	switch (alGetError())
	{
	case AL_OUT_OF_MEMORY:
		Con_Warning("There is not enough memory avaliable to create this audio buffer!\n");
		break;
	case AL_INVALID_VALUE:
		Con_Warning("Invalid audio buffer data!\n");
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
	if (alIsSource(sound->source))
		alDeleteSources(1, &sound->source);
	if (alIsAuxiliaryEffectSlot(sound->effect_slot))
		alDeleteAuxiliaryEffectSlots(1, &sound->effect_slot);

	// Remove it from the list.
	for (auto iterator = sounds.begin(); iterator != sounds.end(); iterator++)
		if (sound == *iterator)
		{
			sounds.erase(iterator);
			break;
		}

	delete sound;
}

typedef struct
{
	int in;

	EFXEAXREVERBPROPERTIES out;
} AudioReverbEffect_t;

// We have to translate this manually, or rather, suffer because
// we don't want to include any of the OpenAL headers outside
// of audio.cpp
//
// Why do this? It's mainly if we ever need to switch away from
// OpenAL in the future.
AudioReverbEffect_t audio_reverb_effects[] =
{
	{ AUDIO_REVERB_GENERIC, EFX_REVERB_PRESET_GENERIC },
	{ AUDIO_REVERB_ALLEY, EFX_REVERB_PRESET_ALLEY },
	{ AUDIO_REVERB_ARENA, EFX_REVERB_PRESET_ARENA },
	{ AUDIO_REVERB_AUDITORIUM, EFX_REVERB_PRESET_AUDITORIUM },
	{ AUDIO_REVERB_BATHROOM, EFX_REVERB_PRESET_BATHROOM },
	{ AUDIO_REVERB_CARPETEDHALLWAY, EFX_REVERB_PRESET_CARPETEDHALLWAY },
	{ AUDIO_REVERB_CAVE, EFX_REVERB_PRESET_CAVE },
	{ AUDIO_REVERB_CHAPEL, EFX_REVERB_PRESET_CHAPEL },
	{ AUDIO_REVERB_CITY, EFX_REVERB_PRESET_CITY },
	{ AUDIO_REVERB_CONCERTHALL, EFX_REVERB_PRESET_CONCERTHALL },
	{ AUDIO_REVERB_DIZZY, EFX_REVERB_PRESET_DIZZY },
	{ AUDIO_REVERB_DRUGGED, EFX_REVERB_PRESET_DRUGGED },
	{ AUDIO_REVERB_FOREST, EFX_REVERB_PRESET_FOREST },
	{ AUDIO_REVERB_HALLWAY, EFX_REVERB_PRESET_HALLWAY },
	{ AUDIO_REVERB_HANGAR, EFX_REVERB_PRESET_HANGAR },
	{ AUDIO_REVERB_LIVINGROOM, EFX_REVERB_PRESET_LIVINGROOM },
	{ AUDIO_REVERB_MOUNTAINS, EFX_REVERB_PRESET_MOUNTAINS },
	{ AUDIO_REVERB_PADDEDCELL, EFX_REVERB_PRESET_PADDEDCELL },
	{ AUDIO_REVERB_PARKINGLOT, EFX_REVERB_PRESET_PARKINGLOT },
	{ AUDIO_REVERB_PLAIN, EFX_REVERB_PRESET_PLAIN },
	{ AUDIO_REVERB_PSYCHOTIC, EFX_REVERB_PRESET_PSYCHOTIC },
};

void AudioManager::SetEffectReverb(const AudioEffect_t *effect, AudioEffectReverb_t mode)
{
	// Translate it for OpenAL.
	EFXEAXREVERBPROPERTIES reverb = { 0 };
	for (unsigned int i = 0; i < pARRAYELEMENTS(audio_reverb_effects); i++)
	{
		if (audio_reverb_effects[i].in == mode)
		{
			reverb = audio_reverb_effects[i].out;
			break;
		}
	}

	// This is sort of dumb, but whatever...
	if (reverb.flGain <= 0)
	{
		Con_Warning("Invalid reverb effect! (%i)\n", mode);
		return;
	}

	alEffecti(effect->id, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
	alEffectf(effect->id, AL_REVERB_DENSITY, reverb.flDensity);
	alEffectf(effect->id, AL_REVERB_DIFFUSION, reverb.flDiffusion);
	alEffectf(effect->id, AL_REVERB_GAIN, reverb.flGain);
	alEffectf(effect->id, AL_REVERB_GAINHF, reverb.flGainHF);
	alEffectf(effect->id, AL_REVERB_DECAY_TIME, reverb.flDecayTime);
	alEffectf(effect->id, AL_REVERB_DECAY_HFRATIO, reverb.flDecayHFRatio);
	alEffectf(effect->id, AL_REVERB_REFLECTIONS_GAIN, reverb.flReflectionsGain);
	alEffectf(effect->id, AL_REVERB_REFLECTIONS_DELAY, reverb.flReflectionsDelay);
	alEffectf(effect->id, AL_REVERB_LATE_REVERB_GAIN, reverb.flLateReverbGain);
	alEffectf(effect->id, AL_REVERB_LATE_REVERB_DELAY, reverb.flLateReverbDelay);
	alEffectf(effect->id, AL_REVERB_AIR_ABSORPTION_GAINHF, reverb.flAirAbsorptionGainHF);
	alEffectf(effect->id, AL_REVERB_ROOM_ROLLOFF_FACTOR, reverb.flRoomRolloffFactor);
	alEffecti(effect->id, AL_REVERB_DECAY_HFLIMIT, reverb.iDecayHFLimit);
	
	int err = alGetError();
	if (err != AL_NO_ERROR)
		Con_Warning("Failed to apply reverb effect!\n%s\n", alGetString(err));
}

bool AudioManager::IsSoundPlaying(const AudioSound_t *sound)
{
	int state;

	alGetSourcei(sound->source, AL_SOURCE_STATE, &state);
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

// Effects

AudioEffect_t *AudioManager::AddEffect()
{
	AudioEffect_t *effect = new AudioEffect_t;
	alGenEffects(1, &effect->id);
	if (alGetError() != AL_NO_ERROR)
	{
		Con_Warning("Failed to create audio effect!\n");

		delete effect;
		return nullptr;
	}

	return effect;
}

void AudioManager::DeleteEffect(AudioEffect_t *effect)
{
	if (alIsEffect(effect->id))
		alDeleteEffects(1, &effect->id);

	delete effect;
}

// Samples

AudioSample_t *AudioManager::AddSample(const char *path)
{
	AudioSample_t *cache = new AudioSample_t;

	// Add it to the global list.
	samples.emplace(path, cache);

	return cache;
}

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

void AudioManager::PrecacheSample(const char *path, bool preserve)
{
	if (!path || (path[0] == ' '))
	{
		Con_Warning("Invalid path for audio sample!\n");
		return;
	}

	AudioSample_t *cache = FindSample(path);
	if (!cache)
	{
		cache = new AudioSample_t;
		memset(cache, 0, sizeof(AudioSample_t));

		LoadSample(cache, path);
		if (!cache->data)
		{
			Con_Warning("Failed to precache audio sample! (%s)\n", path);
			delete cache;
			return;
		}
	}

	if (preserve)
		cache->preserve = true;

	samples.emplace(path, cache);
}

void AudioManager::LoadSample(AudioSample_t *cache, const char *path)
{
	if (!path || (path[0] == ' '))
	{
		Con_Warning("Invalid path for audio sample!\n");
		return;
	}

	std::string complete_path(g_state.path_sounds);
	complete_path += path;

	uint8_t *data = (uint8_t*)COM_LoadHeapFile(complete_path.c_str());
	if (!data)
	{
		Con_Warning("Failed to load audio sample! (%s)\n", path);
		return;
	}

	wavinfo_t info = GetWavinfo(path, data, com_filesize);
	float stepscale = (float)info.rate / AUDIO_SAMPLE_SPEED;
	int len = info.samples / stepscale;
	len *= info.width * info.channels;

	cache->length		= info.samples;		// Length of the sample.
	cache->loopstart	= info.loopstart;
	cache->speed		= info.rate;		// Rate of the sample.
	cache->width		= info.width;
	cache->stereo		= info.channels;	// Stereo / Mono.
	cache->size			= com_filesize;		// Size of the sample.

	// Now resample the sample...
#if 1
	uint8_t *dataofs = data + info.dataofs;

	stepscale = (float)cache->speed / AUDIO_SAMPLE_SPEED;
	int outcount = cache->length / stepscale;
	cache->length = outcount;
	if (cache->loopstart != -1)
		cache->loopstart = cache->loopstart / stepscale;

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
	if (cache->width == 2)
	{
		int num_samples = info.samples;
		if (LittleShort(256) != 256)
		{
			if (info.channels == 2)
				num_samples <<= 1;

			for (int i = 0; i < info.samples; i++)
				((short*)data)[i] = LittleShort(((short*)data)[i]);
		}
	}
#endif
	cache->data = data;

	strncpy(cache->path, path, sizeof(cache->path));
}

void AudioManager::DeleteSample(AudioSample_t *sample, bool force)
{
	if (!sample || (sample->preserve && !force))
		return;

	samples.erase(sample->path);
	if (sample->data)
		delete sample->data;

	delete sample;
}

void AudioManager::ClearSamples(bool force)
{
	for (auto &sample : samples)
		DeleteSample(sample.second, force);

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
		Con_SafePrintf(" Sample(%s) : ", sample.first.c_str());
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
			(int)sounds[i]->current_velocity[0],
			(int)sounds[i]->current_velocity[1],
			(int)sounds[i]->current_velocity[2],
			(int)sounds[i]->current_position[0],
			(int)sounds[i]->current_position[1],
			(int)sounds[i]->current_position[2],
			sounds[i]->pitch);

	Con_Printf("%i samples, %i sounds, %i bytes\n", samples.size(), sounds.size(), mem);
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

void Audio_PlayAmbientSound(plVector3f_t position, const char *path, float volume)
{
	AudioSound_t *sound		= g_audiomanager->AddSound();
	sound->volume			= volume;

	g_audiomanager->SetSoundPosition(sound, position);
	g_audiomanager->LoadSound(sound, path);
	g_audiomanager->PlaySound(sound);
}

void Audio_PlayTemporarySound(plVector3f_t position, bool local, const char *path, float volume)
{
	AudioSound_t *sound		= g_audiomanager->AddSound();
	sound->volume			= volume;
	sound->local			= local;
	sound->pitch			= 1.0f - ((rand() % 3) / 10.0f);

	if (!sound->local)
		g_audiomanager->SetSoundPosition(sound, position);

	g_audiomanager->LoadSound(sound, path);
	g_audiomanager->PlaySound(sound);
}

void Audio_StopSound(const AudioSound_t *sample)
{
	g_audiomanager->StopSound(sample);
}

void Audio_StopSounds(void)
{
	g_audiomanager->StopSounds();
}

void Audio_PrecacheSample(const char *path, bool preserve)
{
	return g_audiomanager->PrecacheSample(path, preserve);
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
