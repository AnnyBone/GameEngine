/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others
Copyright (C) 2011-2017 OldTimes Software

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

#include "engine.h"

/*
===============================================================================

	WAV loading

===============================================================================
*/

byte *data_p,
        *iff_end,
        *last_chunk,
        *iff_data;
int iff_chunk_len;

short GetLittleShort(void) {
    short val = 0;
    val = *data_p;
    val = val + (*(data_p + 1) << 8);
    data_p += 2;
    return val;
}

int GetLittleLong(void) {
    int val = 0;
    val = *data_p;
    val = val + (*(data_p + 1) << 8);
    val = val + (*(data_p + 2) << 16);
    val = val + (*(data_p + 3) << 24);
    data_p += 4;
    return val;
}

void FindNextChunk(const char *name) {
    for (;;) {
        data_p = last_chunk;

        if (data_p >= iff_end) {    // didn't find the chunk
            data_p = NULL;
            return;
        }

        data_p += 4;
        iff_chunk_len = GetLittleLong();
        if (iff_chunk_len < 0) {
            data_p = NULL;
            return;
        }

        data_p -= 8;
        last_chunk = data_p + 8 + ((iff_chunk_len + 1) & ~1);
        if (!strncmp((const char *) data_p, name, 4))
            return;
    }
}

void FindChunk(const char *name) {
    last_chunk = iff_data;
    FindNextChunk(name);
}

void DumpChunks(void) {
    char str[5];

    str[4] = 0;
    data_p = iff_data;
    do {
        memcpy(str, data_p, 4);
        data_p += 4;
        iff_chunk_len = GetLittleLong();
        Con_Printf("%p : %s (%d)\n", (data_p - 4), str, iff_chunk_len);
        data_p += (iff_chunk_len + 1) & ~1;
    } while (data_p < iff_end);
}

typedef struct
{
    int		rate;
    int		width;
    int		channels;
    int		loopstart;
    int		samples;
    int		dataofs;		// chunk starts this many bytes from file start
} wavinfo_t;

wavinfo_t GetWavinfo(const char *name, uint8_t *wav, int wavlength) {
    wavinfo_t info = {0};
    if (!wav)
        return info;

    iff_data = wav;
    iff_end = wav + wavlength;

    // Find "RIFF" chunk
    FindChunk("RIFF");
    if (!(data_p && !strncmp((const char *) data_p + 8, "WAVE", 4))) {
        Con_Printf("Missing RIFF/WAVE chunks\n");
        return info;
    }

// get "fmt " chunk
    iff_data = data_p + 12;
// DumpChunks ();

    FindChunk("fmt ");
    if (!data_p) {
        Con_Printf("Missing fmt chunk\n");
        return info;
    }
    data_p += 8;
    int format = GetLittleShort();
    if (format != 1) {
        Con_Printf("Microsoft PCM format only\n");
        return info;
    }

    info.channels = GetLittleShort();
    info.rate = GetLittleLong();
    data_p += 4 + 2;
    info.width = GetLittleShort() / 8;

// get cue chunk
    FindChunk("cue ");
    if (data_p) {
        data_p += 32;
        info.loopstart = GetLittleLong();

        // if the next chunk is a LIST chunk, look for a cue length marker
        FindNextChunk("LIST");
        if (data_p) {
            if (!strncmp((const char *) data_p + 28, "mark",
                         4)) {    // this is not a proper parse, but it works with cooledit...
                data_p += 24;
                int i = GetLittleLong();    // samples in loop
                info.samples = info.loopstart + i;
//				Con_Printf("looped length: %i\n", i);
            }
        }
    } else
        info.loopstart = -1;

// find data chunk
    FindChunk("data");
    if (!data_p) {
        Con_Printf("Missing data chunk\n");
        return info;
    }

    data_p += 4;
    int samples = GetLittleLong() / info.width;

    if (info.samples) {
        if (samples < info.samples)
            System_Error("Sound %s has a bad loop length", name);
    } else
        info.samples = samples;

    info.dataofs = data_p - wav;

    return info;
}
