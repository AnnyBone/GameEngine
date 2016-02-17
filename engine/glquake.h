/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
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

#ifdef _MSC_VER
#pragma warning(disable : 4244)     // MIPS
#pragma warning(disable : 4136)     // X86
#pragma warning(disable : 4051)     // ALPHA
#endif

//#define	KATANA_CORE_GL
#define	KATANA_CORE_GLIDE

// TODO: 
//		move these into video_layer.c once everything
//		is abstracted out
#if 0
#ifdef KATANA_CORE_GL
#include <GL/glcorearb.h>
#else
#include "GLee.h"
#include <GL/glu.h>
#endif
#else
#	define __MSC__

#	include <glide.h>
#endif

plEXTERN_C_START

void GL_BeginRendering (int *x, int *y, int *width, int *height);
void GL_EndRendering (void);

extern	int glx, gly, glwidth, glheight;

#define BACKFACE_EPSILON	0.01

void R_TimeRefresh_f (void);
texture_t *R_TextureAnimation (texture_t *base, int frame);

typedef struct surfcache_s
{
	struct surfcache_s	*next;
	struct surfcache_s 	**owner;		// NULL is an empty chunk of memory
	int					lightadj[BSP_MAX_LIGHTMAPS]; // checked for strobe flush
	int					dlight;
	int					size;		// including header
	unsigned			width;
	unsigned			height;		// DEBUG only needed for debug
	float				mipscale;
	struct texture_s	*texture;	// checked for animating textures
	uint8_t				data[4];	// width*height elements
} surfcache_t;


typedef struct
{
	pixel_t		*surfdat;	// destination for generated surface
	int			rowbytes;	// destination logical width in bytes
	msurface_t	*surf;		// description for surface to generate
	int			lightadj[BSP_MAX_LIGHTMAPS];
							// adjust for lightmap levels for dynamic lighting
	texture_t	*texture;	// corrected for animating textures
	int			surfmip;	// mipmapped ratio of surface texels / world pixels
	int			surfwidth;	// in mipmapped texels
	int			surfheight;	// in mipmapped texels
} drawsurf_t;

//====================================================

extern	MathVector3f_t	modelorg, r_entorigin;
extern	ClientEntity_t	*currententity;
extern	int				r_visframecount;	// ??? what difs?
extern	int				r_framecount;
extern	mplane_t		frustum[4];

//
// view origin
//
extern	MathVector3f_t	vup;
extern	MathVector3f_t	vpn;
extern	MathVector3f_t	vright;
extern	MathVector3f_t	r_origin;

//
// screen size info
//
extern	refdef_t	r_refdef;
extern	mleaf_t		*r_viewleaf, *r_oldviewleaf;
extern	int		d_lightstylevalue[256];	// 8.8 fraction of base light value

#define OFFSET_NONE		0
#define OFFSET_DECAL	-1
#define OFFSET_FOG		-2
#define OFFSET_SHOWTRIS -3
void GL_PolygonOffset(int);

//johnfitz -- rendering statistics
extern int rs_brushpolys, rs_aliaspolys, rs_skypolys, rs_particles, rs_fogpolys;
extern int rs_dynamiclightmaps, rs_brushpasses, rs_aliaspasses, rs_skypasses;
extern float rs_megatexels;
//johnfitz

//johnfitz -- track developer statistics that vary every frame
extern ConsoleVariable_t devstats;
typedef struct {
	int		packetsize;
	int		edicts;
	unsigned int	visedicts;
	int		efrags;
	int		tempents;
	int		beams;
	int		dlights;
} devstats_t;
extern devstats_t dev_stats, dev_peakstats;
//johnfitz

//ohnfitz -- reduce overflow warning spam
typedef struct {
	double	packetsize;
	double	efrags;
	double	beams;
} overflowtimes_t;
extern overflowtimes_t dev_overflows; //this stores the last time overflow messages were displayed, not the last time overflows occured
#define CONSOLE_RESPAM_TIME 3 // seconds between repeated warning messages
//johnfitz

//johnfitz -- moved here from r_brush.c
#define MAX_LIGHTMAPS 512 //johnfitz -- was 64
extern gltexture_t *lightmap_textures[MAX_LIGHTMAPS]; //johnfitz -- changed to an array
//johnfitz

extern int gl_warpimagesize; //johnfitz -- for water warp

extern bool r_drawflat_cheatsafe, r_fullbright_cheatsafe, r_lightmap_cheatsafe, r_drawworld_cheatsafe; //johnfitz

// [31/1/2013] TODO: Move this into its own header? ~hogsy
void Sky_Init(void);
void Sky_NewMap(void);

//johnfitz -- fog functions called from outside gl_fog.c
void Fog_ParseServerMessage (void);
float *Fog_GetColor (void);
float Fog_GetDensity (void);
void Fog_EnableGFog (void);
void Fog_DisableGFog (void);
void Fog_StartAdditive (void);
void Fog_StopAdditive (void);
void Fog_SetupFrame (void);
void Fog_NewMap (void);
void Fog_Init (void);
//johnfitz

bool R_CullModelForEntity(ClientEntity_t *e);
void R_RotateForEntity(MathVector3f_t origin, MathVector3f_t angles);

plEXTERN_C_END
