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
#define AUDIO_MAX_SOURCES	2

#define	AUDIO_SAMPLE_SPEED	44100	//22050
#define	AUDIO_SAMPLE_BITS	16

typedef unsigned int AudioSource_t;
typedef unsigned int AudioBuffer_t;
typedef unsigned int AudioEffect_t;

typedef struct
{
	AudioBuffer_t	buffer;
	AudioEffect_t	effect;
	AudioSource_t	source;

	sfx_t *sfx;

	plVector3f_t position;
	plVector3f_t velocity;

	bool started;
} AudioSound_t;

typedef struct
{
	bool	efx;					// EFX support.
	bool	soft_buffer_samples;
} AudioExtensions_t;

plEXTERN_C_START

void Audio_Initialize(void);
void Audio_Frame(void);
void Audio_Shutdown(void);

plEXTERN_C_END
