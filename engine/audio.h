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

#define AUDIO_MAX_BUFFERS	4
#define AUDIO_MAX_SOURCES	1

#define	AUDIO_SAMPLE_SPEED	44100	//22050
#define	AUDIO_SAMPLE_BITS	16

typedef struct
{
	unsigned int buffer;
	unsigned int source;
	unsigned int effect;

	plVector3f_t position;
	plVector3f_t velocity;
} AudioSound_t;

typedef struct
{
	bool	efx;					// EFX support.
	bool	soft_buffer_samples;
} AudioExtensions_t;

typedef struct
{
	unsigned int	current_source,	// Current source.
					current_buffer;	// Current buffer.

	unsigned int	buffers[AUDIO_MAX_BUFFERS];
	unsigned int	sources[AUDIO_MAX_SOURCES];

	AudioExtensions_t	extensions;
} Audio_t;

plEXTERN_C_START

extern Audio_t g_audio;

void Audio_Initialize(void);
void Audio_Frame(void);
void Audio_Shutdown(void);

plEXTERN_C_END
