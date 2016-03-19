/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others

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
// snd_dma.c -- main control for any streaming sound output device

#include "engine_base.h"

#ifdef _WIN32
#include "winquake.h"
#endif

// =======================================================================
// Internal sound data & structures
// =======================================================================

channel_t   channels[MAX_CHANNELS];
int			total_channels;

int			snd_blocked = 0;
static bool	snd_ambient = 1;
bool		snd_initialized = false;

// pointer should go away
volatile dma_t  *shm = 0;
volatile dma_t sn;

MathVector3f_t	listener_origin;
MathVector3f_t	listener_forward;
MathVector3f_t	listener_right;
MathVector3f_t	listener_up;
MathVectorf_t	sound_nominal_clip_dist=1000.0;

int			soundtime;		// sample PAIRS
int   		paintedtime; 	// sample PAIRS

#define	MAX_SFX		512

sfx_t		*ambient_sfx[BSP_AMBIENT_END];

int	sound_started=0;

cvar_t bgmvolume			= {"bgmvolume", "1", TRUE};
cvar_t volume				= {"volume", "0.7", TRUE};
cvar_t nosound				= {"nosound", "0"};
cvar_t precache				= {"precache", "1"};
cvar_t loadas8bit			= {"loadas8bit", "0"};
cvar_t ambient_level		= {"ambient_level", "0.3"};
cvar_t ambient_fade			= {"ambient_fade", "100"};
cvar_t snd_noextraupdate	= {"snd_noextraupdate", "0"};
cvar_t snd_show				= {"snd_show", "0"};
cvar_t _snd_mixahead		= {"_snd_mixahead", "0.1", TRUE};

// ====================================================================
// User-setable variables
// ====================================================================


//
// Fake dma is a synchronous faking of the DMA progress used for
// isolating performance in the renderer.  The fakedma_updates is
// number of times S_Update() is called per second.
//

bool fakedma = false;
int fakedma_updates = 15;

void S_Startup (void)
{
	int		rc;

	if (!snd_initialized)
		return;

	if (!fakedma)
	{
#ifdef KATANA_AUDIO_DIRECTSOUND
		rc = SNDDMA_Init();
#else
		rc = 0;
#endif

		if (!rc)
		{
#ifndef	_WIN32
			Con_Printf("S_Startup: SNDDMA_Init failed.\n");
#endif
			sound_started = 0;
			return;
		}
	}

	sound_started = 1;
}

void S_Init (void)
{
	Cvar_RegisterVariable(&nosound, NULL);
	Cvar_RegisterVariable(&volume, NULL);
	Cvar_RegisterVariable(&precache, NULL);
	Cvar_RegisterVariable(&loadas8bit, NULL);
	Cvar_RegisterVariable(&bgmvolume, NULL);
	Cvar_RegisterVariable(&ambient_level, NULL);
	Cvar_RegisterVariable(&ambient_fade, NULL);
	Cvar_RegisterVariable(&snd_noextraupdate, NULL);
	Cvar_RegisterVariable(&snd_show, NULL);
	Cvar_RegisterVariable(&_snd_mixahead, NULL);

	snd_initialized = true;

	S_Startup();

	SND_InitScaletable();

// create a piece of DMA memory

		shm = (volatile dma_t *) Hunk_AllocName(sizeof(*shm), "shm");
		shm->splitbuffer		= 0;
		shm->samplebits			= 16;
		shm->speed				= 22050;
		shm->channels			= 2;
		shm->samples			= 32768;
		shm->samplepos			= 0;
		shm->soundalive			= true;
		shm->gamealive			= true;
		shm->submission_chunk	= 1;
		shm->buffer				= (unsigned char*)Hunk_AllocName(1<<16, "shmbuf");

	// TODO: Make this work? Segfaults on Linux due to shm being NULL.
	//Con_Printf("Sound sampling rate: %i\n", shm->speed);

	// provides a tick sound until washed clean
//	if (shm->buffer)
//		shm->buffer[4] = shm->buffer[5] = 0x7f;	// force a pop for debugging

	//ambient_sfx[BSP_AMBIENT_WATER] = Audio_PrecacheSample("ambience/water_loop0.wav");
	//ambient_sfx[BSP_AMBIENT_SKY]	= Audio_PrecacheSample("ambience/wind2.wav");
}

//=============================================================================

channel_t *SND_PickChannel(unsigned int entnum, int entchannel)
{
	int ch_idx,first_to_die,life_left;

// Check for replacement sound, or find the best one to replace
	first_to_die = -1;
	life_left = 0x7fffffff;
	for(ch_idx = BSP_AMBIENT_END; ch_idx < BSP_AMBIENT_END+MAX_DYNAMIC_CHANNELS; ch_idx++)
	{
		if(entchannel != 0		// channel 0 never overrides
		&& channels[ch_idx].entnum == entnum
		&& (channels[ch_idx].entchannel == entchannel || entchannel == -1))
		{
			// allways override sound from same entity
			first_to_die = ch_idx;
			break;
		}

		// don't let monster sounds override player sounds
		if (channels[ch_idx].entnum == cl.viewentity && entnum != cl.viewentity && channels[ch_idx].sfx)
			continue;

		if (channels[ch_idx].end - paintedtime < life_left)
		{
			life_left = channels[ch_idx].end - paintedtime;
			first_to_die = ch_idx;
		}
   }

	if (first_to_die == -1)
		return NULL;

	if (channels[first_to_die].sfx)
		channels[first_to_die].sfx = NULL;

	return &channels[first_to_die];
}

void SND_Spatialize(channel_t *ch)
{
	MathVectorf_t	dot,dist,lscale,rscale,scale;
	MathVector3f_t	source_vec;

	// Anything coming from the view entity will allways be full volume
	if (ch->entnum == cl.viewentity)
	{
		ch->leftvol		= ch->master_vol;
		ch->rightvol	= ch->master_vol;
		return;
	}

	// Calculate stereo seperation and distance attenuation
	Math_VectorSubtract(ch->origin, listener_origin, source_vec);

	dist = plVectorNormalize(source_vec) * ch->dist_mult;

	dot = Math_DotProduct(listener_right, source_vec);

	if (shm->channels == 1)
	{
		rscale = 1.0;
		lscale = 1.0;
	}
	else
	{
		rscale = 1.0 + dot;
		lscale = 1.0 - dot;
	}

// add in distance effect
	scale = (1.0 - dist) * rscale;
	ch->rightvol = (int) (ch->master_vol * scale);
	if (ch->rightvol < 0)
		ch->rightvol = 0;

	scale = (1.0 - dist) * lscale;
	ch->leftvol = (int) (ch->master_vol * scale);
	if (ch->leftvol < 0)
		ch->leftvol = 0;
}

//=============================================================================

void S_UpdateAmbientSounds (void)
{
	mleaf_t		*l;
	float		vol;
	int			ambient_channel;
	channel_t	*chan;

	if (!snd_ambient)
		return;

	//johnfitz -- no ambients when disconnected
	if (cls.state != ca_connected)
		return;
	//johnfitz

// calc ambient sound levels
	if (!cl.worldmodel)
		return;

	l = Mod_PointInLeaf (listener_origin, cl.worldmodel);
	if (!l || !ambient_level.value)
	{
		for (ambient_channel = 0 ; ambient_channel< BSP_AMBIENT_END ; ambient_channel++)
			channels[ambient_channel].sfx = NULL;
		return;
	}

	for (ambient_channel = 0 ; ambient_channel< BSP_AMBIENT_END ; ambient_channel++)
	{
		chan = &channels[ambient_channel];
		chan->sfx = ambient_sfx[ambient_channel];

		vol = ambient_level.value * l->ambient_sound_level[ambient_channel];
		if (vol < 8)
			vol = 0;

	// don't adjust volume too fast
		if (chan->master_vol < vol)
		{
			chan->master_vol += host_frametime * ambient_fade.value;
			if (chan->master_vol > vol)
				chan->master_vol = vol;
		}
		else if (chan->master_vol > vol)
		{
			chan->master_vol -= host_frametime * ambient_fade.value;
			if (chan->master_vol < vol)
				chan->master_vol = vol;
		}

		chan->leftvol = chan->rightvol = chan->master_vol;
	}
}

/*	Called once each time through the main loop
*/
void S_Update(plVector3f_t origin, plVector3f_t forward, plVector3f_t right, plVector3f_t up)
{
	int			i, j;
	int			total;
	channel_t	*ch;
	channel_t	*combine;

	if (!sound_started || (snd_blocked > 0))
		return;

	Math_VectorCopy(origin, listener_origin);
	Math_VectorCopy(forward, listener_forward);
	Math_VectorCopy(right, listener_right);
	Math_VectorCopy(up, listener_up);

// update general area ambient sound sources
	S_UpdateAmbientSounds ();

	combine = NULL;

// update spatialization for static and dynamic sounds
	ch = channels+BSP_AMBIENT_END;
	for (i=BSP_AMBIENT_END ; i<total_channels; i++, ch++)
	{
		if (!ch->sfx)
			continue;
		SND_Spatialize(ch);         // respatialize channel
		if (!ch->leftvol && !ch->rightvol)
			continue;

	// try to combine static sounds with a previous channel of the same
	// sound effect so we don't mix five torches every frame

		if (i >= MAX_DYNAMIC_CHANNELS + BSP_AMBIENT_END)
		{
		// see if it can just use the last one
			if (combine && combine->sfx == ch->sfx)
			{
				combine->leftvol += ch->leftvol;
				combine->rightvol += ch->rightvol;
				ch->leftvol = ch->rightvol = 0;
				continue;
			}
		// search for one
			combine = channels+MAX_DYNAMIC_CHANNELS + BSP_AMBIENT_END;
			for (j=MAX_DYNAMIC_CHANNELS + BSP_AMBIENT_END ; j<i; j++, combine++)
				if (combine->sfx == ch->sfx)
					break;

			if (j == total_channels)
				combine = NULL;
			else
			{
				if (combine != ch)
				{
					combine->leftvol += ch->leftvol;
					combine->rightvol += ch->rightvol;
					ch->leftvol = ch->rightvol = 0;
				}
				continue;
			}
		}
	}

	// Debugging output
	if (snd_show.value)
	{
		total = 0;
		ch = channels;
		for (i=0 ; i<total_channels; i++, ch++)
			if (ch->sfx && (ch->leftvol || ch->rightvol) )
			{
				//Con_Printf ("%3i %3i %s\n", ch->leftvol, ch->rightvol, ch->sfx->name);
				total++;
			}

		Con_Printf ("----(%i)----\n", total);
	}
}

