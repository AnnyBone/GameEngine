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

#define	VIDEO_MAX_UNITS	16

// Drawing

// Primitive Types
typedef enum vlPrimitive_s
{
	VL_PRIMITIVE_IGNORE = -1,

	VL_PRIMITIVE_LINES,
	VL_PRIMITIVE_LINE_STRIP,
	VL_PRIMITIVE_POINTS,
	VL_PRIMITIVE_TRIANGLES,
	VL_PRIMITIVE_TRIANGLE_STRIP,
	VL_PRIMITIVE_TRIANGLE_FAN,
	VL_PRIMITIVE_TRIANGLE_FAN_LINE,
	VL_PRIMITIVE_QUADS,				// Advised to avoid this.

	VL_PRIMITIVE_END
} vlPrimitive_t;

typedef struct vlVertex_s
{
	plVector3f_t position;
	plVector3f_t normal;

	plVector2f_t ST[16];

	plColour_t colour;
} vlVertex_t;

typedef unsigned int vlVBO_t;

typedef struct vlDraw_s
{
	vlVertex_t *vertices;	// Array of vertices for the object.

	unsigned int numverts;		// Number of vertices.
	unsigned int numtriangles;	// Number of triangles.

	uint8_t	*indices;	// List of indeces.

	vlPrimitive_t primitive, primitive_restore;

	vlVBO_t	vbo_vertices;
} vlDraw_t;

// Textures

// Texture Environment Modes
typedef enum
{
	VIDEO_TEXTURE_MODE_IGNORE = -1,

	VIDEO_TEXTURE_MODE_ADD,
	VIDEO_TEXTURE_MODE_MODULATE,
	VIDEO_TEXTURE_MODE_DECAL,
	VIDEO_TEXTURE_MODE_BLEND,
	VIDEO_TEXTURE_MODE_REPLACE,
	VIDEO_TEXTURE_MODE_COMBINE
} vlTextureEnvironmentMode_t;

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
