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

#ifndef AUDIO_H
#define AUDIO_H

#define	AUDIO_MAX_BUFFERS	1024

#define	AUDIO_SAMPLE_RATE	44100

typedef struct
{
	unsigned int buffer;
} AudioSound_t;

typedef struct
{
	unsigned int	current_source,	// Current source.
					current_buffer;	// Current buffer.

	unsigned int	buffers[AUDIO_MAX_BUFFERS];
} Audio_t;

plEXTERN_C_START

extern Audio_t g_audio;

void Audio_Initialize(void);
void Audio_Frame(void);
void Audio_Shutdown(void);

plEXTERN_C_END

#endif // !AUDIO_H
