/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
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
} AudioSample;

typedef struct
{
	unsigned int id;
} AudioEffect;

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
} AudioEffectReverb;

typedef struct
{
	// OpenAL-specific data.
	AudioBuffer_t			buffer;
	AudioAuxiliaryEffect_t	effect_slot;
	AudioSource_t			source;

	const AudioSample *cache;

	plVector3f_t current_position;	// Position of the sound.
	plVector3f_t current_velocity;	// Speed of movement.

	unsigned int	entity;		// Unique identifier.
	AudioChannel_t	channel;	// Used for channel overriding.

	float max_distance;		// Maximum distance.
	float pitch;			// Pitch of the sound.
	float volume;			// Volume of the sound.

	bool local;		// Only played locally.

	bool preserve;
} AudioSound;

#ifdef __cplusplus

namespace core
{
	class AudioManager : public XManager
	{
	public:
		AudioManager();
		~AudioManager();

		void Simulate();

		AudioSound *AddSound();
		void DeleteSound(AudioSound *sound);
		void PlaySound(const AudioSound *sound);
		void PlaySound(const char *path);
		void LoadSound(AudioSound *sound, std::string path);
		void StopSound(const AudioSound *sound);
		void PauseSound(const AudioSound *sound);

		void SetSoundPosition(AudioSound *sound, plVector3f_t position);
		void SetSoundVelocity(AudioSound *sound, plVector3f_t velocity);

		AudioEffect *AddEffect();
		void DeleteEffect(AudioEffect *effect);
		void SetEffectReverb(const AudioEffect *effect, AudioEffectReverb mode);

		void StopSounds();		// Stops all sounds from playing.
		void ClearSounds();		// Clears all sounds.
		void ListSounds();		// Lists all currently active sounds.

		bool IsSoundPlaying(const AudioSound *sound);
		bool IsSoundPaused(const AudioSound *sound);

		AudioSample *AddSample(std::string path);
		void DeleteSample(AudioSample *sample, bool force);
		AudioSample *GetSample(std::string path);
		void PrecacheSample(const char *path, bool preserve);
		void LoadSample(AudioSample *cache, const char *path);

		void ClearSamples(bool force);	// Clears all loaded samples.
	protected:
	private:
		struct AudioExtensions
		{
			bool	efx;					// EFX support.
			bool	soft_buffer_samples;
		};
		AudioExtensions extensions;

		AudioEffect *effect_global;

		std::vector<AudioSound*> sounds;

		std::unordered_map<std::string, AudioSample*> samples;
	};
}

#endif

plEXTERN_C_START

void Audio_Initialize(void);
void Audio_Frame(void);
void Audio_PlayAmbientSound(plVector3f_t position, const char *path, float volume);
void Audio_PlayTemporarySound(unsigned int ent, AudioChannel_t channel, plVector3f_t position, bool local, const char *path, float volume);
void Audio_StopSounds(void);
void Audio_PrecacheSample(const char *path, bool preserve);
void Audio_Shutdown(void);

plEXTERN_C_END
