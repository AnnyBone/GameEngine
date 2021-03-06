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

#define    AUDIO_MAX_SOUNDS    2048

#define    AUDIO_SAMPLE_SPEED    44100    //22050
#define    AUDIO_SAMPLE_BITS    16

#define DEFAULT_SOUND_PACKET_VOLUME 255
#define DEFAULT_SOUND_PACKET_ATTENUATION 1.0

typedef struct {
    char path[PL_SYSTEM_MAX_PATH];

    int length;
    int loopstart;
    int speed;        // Frequency of the sample.
    int width;
    int stereo;
    int size;

    bool preserve;    // Allow this to be deleted.

    uint8_t *data;
} AudioSample_t;

typedef struct {
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
typedef enum {
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

typedef struct {
    // OpenAL-specific data.
    AudioBuffer_t buffer;
    AudioAuxiliaryEffect_t effect_slot;
    AudioSource_t source;

    const AudioSample_t *cache;

    PLVector3D current_position;    // Position of the sound.
    PLVector3D current_velocity;    // Speed of movement.

    unsigned int entity;        // Unique identifier.
    AudioChannel_t channel;    // Used for channel overriding.

    float max_distance;        // Maximum distance.
    float pitch;            // Pitch of the sound.
    float volume;            // Volume of the sound.

    bool local;        // Only played locally.

    bool preserve;
} AudioSound_t;

namespace xenon {
    class AudioManager {
    public:
        AudioManager();

        ~AudioManager();

        void Frame();

        AudioSound_t *AddSound();

        void DeleteSound(AudioSound_t *sound);

        void PlaySound(const AudioSound_t *sound);

        void PlaySound(const char *path);

        void LoadSound(AudioSound_t *sound, std::string path);

        void StopSound(const AudioSound_t *sound);

        void PauseSound(const AudioSound_t *sound);

        void SetSoundPosition(AudioSound_t *sound, PLVector3D position);

        void SetSoundVelocity(AudioSound_t *sound, PLVector3D velocity);

        AudioEffect_t *AddEffect();

        void DeleteEffect(AudioEffect_t *effect);

        void SetEffectReverb(const AudioEffect_t *effect, AudioEffectReverb_t mode);

        void StopSounds();        // Stops all sounds from playing.
        void ClearSounds();        // Clears all sounds.
        void ListSounds();        // Lists all currently active sounds.

        bool IsSoundPlaying(const AudioSound_t *sound);

        bool IsSoundPaused(const AudioSound_t *sound);

        AudioSample_t *AddSample(std::string path);

        void DeleteSample(AudioSample_t *sample, bool force);

        AudioSample_t *GetSample(std::string path);

        void PrecacheSample(const char *path, bool preserve);

        void LoadSample(AudioSample_t *cache, const char *path);

        void ClearSamples(bool force);    // Clears all loaded samples.
    protected:
    private:
        struct AudioExtensions {
            bool efx;                    // EFX support.
            bool soft_buffer_samples;
        };
        AudioExtensions extensions;

        AudioEffect_t *effect_global;

        std::vector<AudioSound_t *> sounds;

        std::unordered_map<std::string, AudioSample_t *> samples;
    };
}
