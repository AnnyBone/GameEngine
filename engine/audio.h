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

#pragma once

#define	AUDIO_MAX_SOUNDS	2048

#define	AUDIO_SAMPLE_SPEED	44100	//22050
#define	AUDIO_SAMPLE_BITS	16

typedef struct
{
	char path[MAX_QPATH];

	int 	length;
	int 	loopstart;
	int 	speed;		// Frequency of the sample.
	int 	width;
	int 	stereo;
	int		size;

	bool preserve;	// Allow this to be deleted.

	uint8_t	*data;
} AudioSample_t;

typedef struct
{
	unsigned int id;
} AudioEffect_t;

typedef unsigned int AudioAuxiliaryEffect_t;
typedef unsigned int AudioSource_t;
typedef unsigned int AudioBuffer_t;

/*	To keep reverb implementation
	as easy to use as possible,
	we're just going to rely on
	these preset types for now
	that are provided by OpenAL.
	(okay not all of them, but most)
*/
typedef enum
{
	AUDIO_REVERB_GENERIC,
	AUDIO_REVERB_PADDEDCELL,
	AUDIO_REVERB_ROOM,
	AUDIO_REVERB_BATHROOM,
	AUDIO_REVERB_LIVINGROOM,
	AUDIO_REVERB_STONEROOM,
	AUDIO_REVERB_AUDITORIUM,
	AUDIO_REVERB_CONCERTHALL,
	AUDIO_REVERB_CAVE,
	AUDIO_REVERB_ARENA,
	AUDIO_REVERB_HANGAR,
	AUDIO_REVERB_CARPETEDHALLWAY,
	AUDIO_REVERB_HALLWAY,
	AUDIO_REVERB_STONECORRIDOR,
	AUDIO_REVERB_ALLEY,
	AUDIO_REVERB_FOREST,
	AUDIO_REVERB_CITY,
	AUDIO_REVERB_MOUNTAINS,
	AUDIO_REVERB_QUARRY,
	AUDIO_REVERB_PLAIN,
	AUDIO_REVERB_PARKINGLOT,
	AUDIO_REVERB_SEWERPIPE,
	AUDIO_REVERB_UNDERWATER,
	AUDIO_REVERB_DRUGGED,
	AUDIO_REVERB_DIZZY,
	AUDIO_REVERB_PSYCHOTIC,
	AUDIO_REVERB_CHAPEL
} AudioEffectReverb_t;

typedef struct
{
	// OpenAL-specific data.
	AudioBuffer_t			buffer;
	AudioAuxiliaryEffect_t	effect_slot;
	AudioSource_t			source;

	const AudioSample_t *cache;

	plVector3f_t current_position;	// Position of the sound.
	plVector3f_t current_velocity;	// Speed of movement.

	ClientEntity_t	*entity;	// Owner, typically sound will follow this.
	int				channel;	// Only really relevent for legacy stuff!

	float max_distance;		// Maximum distance.
	float pitch;			// Pitch of the sound.
	float volume;			// Volume of the sound.

	bool local;		// Only played locally.
	bool loop;		// Loop the sound?

	bool preserve;
} AudioSound_t;

#ifdef __cplusplus

namespace Core
{
	class AudioManager : public CoreManager
	{
	public:
		AudioManager();
		~AudioManager();

		void Frame();

		AudioSound_t *AddSound();
		void DeleteSound(AudioSound_t *sound);
		void PlaySound(const AudioSound_t *sound);
		void PlaySound(const char *path);
		void LoadSound(AudioSound_t *sound, const char *path);
		void StopSound(const AudioSound_t *sound);
		void PauseSound(const AudioSound_t *sound);

		void SetSoundPosition(AudioSound_t *sound, plVector3f_t position);
		void SetSoundVelocity(AudioSound_t *sound, plVector3f_t velocity);

		AudioEffect_t *AddEffect();
		void DeleteEffect(AudioEffect_t *effect);
		void SetEffectReverb(const AudioEffect_t *effect, AudioEffectReverb_t mode);

		void StopSounds();		// Stops all sounds from playing.
		void ClearSounds();		// Clears all sounds.
		void ListSounds();		// Lists all currently active sounds.

		bool IsSoundPlaying(const AudioSound_t *sound);
		bool IsSoundPaused(const AudioSound_t *sound);

		AudioSample_t *AddSample(const char *path);
		void DeleteSample(AudioSample_t *sample);
		AudioSample_t *FindSample(const char *path);
		void PrecacheSample(const char *path, bool preserve);
		void LoadSample(AudioSample_t *cache, const char *path);

		void ClearSamples();	// Clears all loaded samples.
	protected:
	private:
		struct AudioExtensions
		{
			bool	efx;					// EFX support.
			bool	soft_buffer_samples;
		};
		AudioExtensions extensions;

		AudioEffect_t *effect_global;

		std::vector<AudioSound_t*> sounds;

		std::unordered_map<std::string, AudioSample_t*> samples;
	};
}

#endif

plEXTERN_C_START

void Audio_Initialize(void);
void Audio_Frame(void);
void Audio_PlayTemporarySound(ClientEntity_t *entity, const char *path, float volume);
void Audio_StopSounds(void);
void Audio_PrecacheSample(const char *path, bool preserve);
void Audio_Shutdown(void);

plEXTERN_C_END
