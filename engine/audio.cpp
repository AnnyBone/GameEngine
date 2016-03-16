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

// fuuuuuucccckkkkkkkkk wiiiinnndddoooowwwwwwwsss
#ifdef PlaySound
#	undef PlaySound
#endif

namespace Core
{
	class AudioManager : public CoreManager
	{
	public:
		AudioManager();
		~AudioManager();

		void Frame();

		AudioSound_t *AddSound();
		void DeleteSound(AudioSound_t *sample);

		void PlaySound(const AudioSound_t *sample);
		void PlaySound(const char *path);
		void StopSound(const AudioSound_t *sample);
		void PauseSound(const AudioSound_t *sample);

		void ClearSounds();
	protected:
	private:
		ALCdevice	*device;
		ALCcontext	*context;

		AudioExtensions_t extensions;

		std::vector<AudioSound_t*> sounds;
	};
}

using namespace Core;

AudioManager::AudioManager()
{
	extensions.efx					=
	extensions.soft_buffer_samples	= false;

	if (COM_CheckParm("-nosound") || COM_CheckParm("-noaudio"))
		return;

	Con_Printf("Initializing Audio Manager...\n");

	Cmd_AddCommand("audio_play", Audio_PlayCommand);

	// Reserve up to AUDIO_MAX_SOUNDS, and we can expand on this as necessary.
	sounds.reserve(AUDIO_MAX_SOUNDS);

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

		alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
		alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
		alIsEffect = (LPALISEFFECT)alGetProcAddress("alIsEffect");
		alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");

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
			for (unsigned int i = 0; i < sounds.size(); i++)
			{
				if (sounds[i]->buffer)
					alDeleteBuffers(1, &sounds[i]->buffer);
				if (sounds[i]->effect)
					alDeleteEffects(1, &sounds[i]->effect);
				if (sounds[i]->source)
					alDeleteSources(1, &sounds[i]->source);
			}

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

	return sample;
}

void AudioManager::PlaySound(const AudioSound_t *sample)
{
	if (!sample)
		return;

	alSourcefv(sample->source, AL_POSITION, sample->position);
	alSourcefv(sample->source, AL_VELOCITY, sample->velocity);

	// Play the source.
	alSourcePlay(sample->source);
}

void AudioManager::PlaySound(const char *path)
{
	if (!path || (path[0] == ' '))
	{
		Con_Printf("Invalid path for sound!\n");
		return;
	}

	AudioSound_t *sample = AddSound();
	sample->sfx = S_PrecacheSound(path);

	PlaySound(sample);
}

void AudioManager::StopSound(const AudioSound_t *sample)
{
	if (!sample)
		return;

	// Stop the source.
	alSourceStop(sample->source);
}

void AudioManager::PauseSound(const AudioSound_t *sample)
{}

void AudioManager::DeleteSound(AudioSound_t *sample)
{
	if (!sample)
		return;

	if (sample->buffer)
		alDeleteBuffers(1, &sample->buffer);
	if (sample->effect)
		alDeleteEffects(1, &sample->effect);
	if (sample->source)
		alDeleteSources(1, &sample->source);
}

void AudioManager::ClearSounds()
{
	sounds.clear();
	sounds.shrink_to_fit();				// Free up mem.
	sounds.reserve(AUDIO_MAX_SOUNDS);	// Reserve again.
}

AudioManager *g_audiomanager;

void Audio_Initialize(void)
{
	g_audiomanager = new AudioManager();
}

void Audio_StopSound(const AudioSound_t *sample)
{
	g_audiomanager->StopSound(sample);
}

void Audio_LoadSound(AudioSound_t *sample, const char *path)
{}

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
