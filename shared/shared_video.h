/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
	Copyright (C) 2011-2015 OldTimes Software

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

#ifndef __SHAREDVIDEO__
#define	__SHAREDVIDEO__

#define	VIDEO_MAX_UNITS	16

// Video Capabilities
#define	VIDEO_ALPHA_TEST	1	// Alpha-testing
#define	VIDEO_BLEND			2	// Blending
#define	VIDEO_TEXTURE_2D	4	// Enables/disables textures.
#define	VIDEO_DEPTH_TEST	8	// Depth-testing.
#define	VIDEO_TEXTURE_GEN_T	16	// Generate T coordinate.
#define	VIDEO_TEXTURE_GEN_S	32	// Generate S coordinate.
#define	VIDEO_CULL_FACE		64	// Automatically cull faces.
#define	VIDEO_STENCIL_TEST	128	// Stencil-testing.
#define	VIDEO_NORMALIZE		256	// Normalization for scaled models that are lit.
#define	VIDEO_MULTISAMPLE	512	// Multisampling.

// Primitive Types
typedef enum
{
	VIDEO_PRIMITIVE_IGNORE,

	VIDEO_PRIMITIVE_LINE,
	VIDEO_PRIMITIVE_TRIANGLES,
	VIDEO_PRIMITIVE_TRIANGLE_STRIP,
	VIDEO_PRIMITIVE_TRIANGLE_FAN,
	VIDEO_PRIMITIVE_TRIANGLE_FAN_LINE
} VideoPrimitive_t;

// Blending Modes
typedef enum VideoBlend_e
{
	VIDEO_BLEND_ZERO,
	VIDEO_BLEND_ONE,
	/*	As nice as it would be, these shouldn't 
		use GL macros directly,	otherwise we'll 
		need to include GL headers for game and 
		menu projects.
	*/
	VIDEO_BLEND_SRC_COLOR			= 0x0300,
	VIDEO_BLEND_ONE_MINUS_SRC_COLOR = 0x0301,
	VIDEO_BLEND_SRC_ALPHA			= 0x0302,
	VIDEO_BLEND_ONE_MINUS_SRC_ALPHA = 0x0303,
	VIDEO_BLEND_DST_ALPHA			= 0x0304,
	VIDEO_BLEND_ONE_MINUS_DST_ALPHA = 0x0305,
	VIDEO_BLEND_DST_COLOR			= 0x0306,
	VIDEO_BLEND_ONE_MINUS_DST_COLOR = 0x0307,
	VIDEO_BLEND_SRC_ALPHA_SATURATE	= 0x0308
} VideoBlend_t;

#define	VIDEO_BLEND_ADDITIVE	VIDEO_BLEND_SRC_ALPHA, VIDEO_BLEND_ONE
#define	VIDEO_BLEND_DEFAULT		VIDEO_BLEND_SRC_ALPHA, VIDEO_BLEND_ONE_MINUS_SRC_ALPHA

// Depth Modes
typedef enum VideoDepth_e
{
	VIDEO_DEPTH_IGNORE = -1,	// Don't bother changing depth mode.
	VIDEO_DEPTH_FALSE,			// Don't enable depth mask.
	VIDEO_DEPTH_TRUE			// Enable depth mask.
} VideoDepth_t;

// Texture Environment Modes
typedef enum VideoTextureEnvironmentMode_e
{
	VIDEO_TEXTURE_MODE_ADD,
	VIDEO_TEXTURE_MODE_MODULATE,
	VIDEO_TEXTURE_MODE_DECAL,
	VIDEO_TEXTURE_MODE_BLEND,
	VIDEO_TEXTURE_MODE_REPLACE,
	VIDEO_TEXTURE_MODE_COMBINE
} VideoTextureEnvironmentMode_t;

// Vertex
typedef struct
{
	MathVector3f_t mvPosition;				// Vertex position.

	MathVector4f_t mvColour;				// Vertex RGBA.

	MathVector3f_t mvNormal;				// Vertex normal.

	MathVector2f_t mvST[VIDEO_MAX_UNITS];	// Vertex texture coord, per unit.
} VideoObjectVertex_t;

// Object
typedef struct
{
	VideoObjectVertex_t *ovVertices;	// Array of vertices for the object.

	int iVertices;						// Number of vertices.

	VideoPrimitive_t vpPrimitiveType;

	bool bWireframeOverride;			// If wireframe view is active, override it for this object.

	unsigned int uiVertexBuffer;
	unsigned int uiColourBuffer;
	unsigned int uiTextureBuffer;
} VideoObject_t;

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

#endif
