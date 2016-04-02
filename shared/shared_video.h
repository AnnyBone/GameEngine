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

// Primitive Types
typedef enum
{
	VIDEO_PRIMITIVE_IGNORE = -1,

	VIDEO_PRIMITIVE_LINES,
	VL_PRIMITIVE_LINE_STRIP,
	VIDEO_PRIMITIVE_POINTS,
	VIDEO_PRIMITIVE_TRIANGLES,
	VIDEO_PRIMITIVE_TRIANGLE_STRIP,
	VIDEO_PRIMITIVE_TRIANGLE_FAN,
	VIDEO_PRIMITIVE_TRIANGLE_FAN_LINE,
	VIDEO_PRIMITIVE_QUADS,				// Advised to avoid this.

	VIDEO_PRIMITIVE_END
} VideoPrimitive_t;

// Texture Environment Modes
typedef enum VideoTextureEnvironmentMode_e
{
	VIDEO_TEXTURE_MODE_IGNORE = -1,

	VIDEO_TEXTURE_MODE_ADD,
	VIDEO_TEXTURE_MODE_MODULATE,
	VIDEO_TEXTURE_MODE_DECAL,
	VIDEO_TEXTURE_MODE_BLEND,
	VIDEO_TEXTURE_MODE_REPLACE,
	VIDEO_TEXTURE_MODE_COMBINE
} VideoTextureEnvironmentMode_t;

#define	VIDEO_MAX_UNITS	16

typedef struct VLvertex_struct VLvertex;
typedef struct VLdraw_struct VLdraw;

// Canvas
typedef enum
{
	CANVAS_NONE,
	CANVAS_DEFAULT,
	CANVAS_CONSOLE,
	CANVAS_MENU,
	CANVAS_SBAR,
	CANVAS_WARPIMAGE,
	CANVAS_CROSSHAIR,
	CANVAS_BOTTOMLEFT,
	CANVAS_BOTTOMRIGHT,
	CANVAS_TOPRIGHT
} VideoCanvasType_t;
