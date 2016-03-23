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

typedef unsigned int AudioSource_t;
typedef unsigned int AudioBuffer_t;
typedef unsigned int AudioEffect_t;

typedef struct
{
	// OpenAL-specific data.
	AudioBuffer_t	buffer;
	AudioEffect_t	effect;
	AudioSource_t	source;

	const AudioSample_t *cache;

	plVector3f_t position;	// Position of the sound.
	plVector3f_t velocity;	// Speed of movement.

	ClientEntity_t	*entity;	// Owner, typically sound will follow this.
	int				channel;	// Only really relevent for legacy stuff!

	float max_distance;		// Maximum distance.
	float pitch;			// Pitch of the sound.
	float volume;			// Volume of the sound.

	bool local;		// Only played locally.
	bool loop;		// Loop the sound?

	bool auto_delete;
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
		void PlaySound(ClientEntity_t *entity, const AudioSample_t *cache, float volume);
		void PlaySound(const char *path);
		void LoadSound(AudioSound_t *sound, const char *path);
		void StopSound(const AudioSound_t *sound);
		void PauseSound(const AudioSound_t *sound);

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

		std::vector<AudioSound_t*> sounds;

		std::unordered_map<std::string, AudioSample_t*> samples;
	};
}

#endif

plEXTERN_C_START

void Audio_Initialize(void);
void Audio_Frame(void);
void Audio_PlayTemporarySound(ClientEntity_t *entity, const AudioSample_t *cache, float volume);
void Audio_StopSounds(void);
void Audio_PrecacheSample(const char *path, bool preserve);
void Audio_Shutdown(void);

plEXTERN_C_END
