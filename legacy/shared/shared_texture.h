/*
Copyright (C) 2011-2016 OldTimes Software

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

#include "shared_model.h"

enum XTextureFormat {
    // Legacy
    SRC_INDEXED,
    SRC_RGBA
};

// XTexture Flags
enum {
    XTEXTURE_FLAG_NEAREST = (1 << 0),
    XTEXTURE_FLAG_ALPHA = (1 << 1),
    XTEXTURE_FLAG_MIPMAP = (1 << 2),
    XTEXTURE_FLAG_PRESERVE = (1 << 3)
};

#ifdef __cplusplus

class XTexture;

#else

typedef struct XTexture XTexture;

#endif

// legacy
typedef struct gltexture_s {
    //managed by texture manager
    unsigned int texnum;
    struct gltexture_s *next;
    struct model_s *owner;                    // TODO: Make this obsolete!!
    //managed by image loading
    char name[64];
    unsigned int width;                    // Size of image as it exists in opengl
    unsigned int height;                    // Size of image as it exists in opengl
    unsigned int flags;
    char source_file[MAX_QPATH]; // Relative filepath to data source, or "" if source is in memory
    enum XTextureFormat source_format;            // Format of pixel data (indexed, lightmap, or rgba)
    unsigned int source_width;            // Size of image in source data
    unsigned int source_height;            // Size of image in source data
    char pants;                    // 0-13 pants color, or -1 if never colormapped
    //used for rendering
    int visframe;                // Matches r_framecount if texture was bound this frame
} gltexture_t;