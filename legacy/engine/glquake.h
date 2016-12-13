/*
Copyright (C) 1996-2001 Id Software, Inc.
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

// todo, get rid of these... damn it.
#ifdef _MSC_VER
#	pragma warning(disable : 4244)     // MIPS
#	pragma warning(disable : 4136)     // X86
#	pragma warning(disable : 4051)     // ALPHA
#endif
plEXTERN_C_START

#define BACKFACE_EPSILON	0.01

void R_TimeRefresh_f (void);
texture_t *R_TextureAnimation (texture_t *base, int frame);

//====================================================

extern	MathVector3f_t	modelorg, r_entorigin;
extern	ClientEntity_t	*currententity;
extern	int				r_visframecount;	// ??? what difs?
extern	int				r_framecount;

extern int d_lightstylevalue[256];	// 8.8 fraction of base light value

//johnfitz -- rendering statistics
extern int rs_brushpolys, rs_aliaspolys, rs_skypolys, rs_particles, rs_fogpolys;
extern int rs_dynamiclightmaps, rs_brushpasses, rs_aliaspasses, rs_skypasses;
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
extern XTexture *lightmap_textures[MAX_LIGHTMAPS]; //johnfitz -- changed to an array
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
