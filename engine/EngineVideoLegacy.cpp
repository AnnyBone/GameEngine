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

#include "engine_base.h"

#include "EngineEditor.h"
#include "video.h"
#include "EngineVideoParticle.h"
#include "video_shadow.h"

#include "client/effect_sprite.h"

MathVector3f_t modelorg, r_entorigin;
ClientEntity_t *currententity;

int	r_visframecount,	// bumped when going to a new PVS
	r_framecount;		// used for dlight push checking

mplane_t frustum[4];

//johnfitz -- rendering statistics
int rs_brushpolys, rs_aliaspolys, rs_skypolys, rs_particles, rs_fogpolys;
int rs_dynamiclightmaps, rs_brushpasses, rs_aliaspasses, rs_skypasses;
float rs_megatexels;

// view origin
MathVector3f_t	vup,vpn,vright,
		r_origin;

float r_world_matrix[16], r_base_world_matrix[16];

float r_fovx, r_fovy; //johnfitz -- rendering fov may be different becuase of r_waterwarp and r_stereo

int		d_lightstylevalue[256];	// 8.8 fraction of base light value

cvar_t	r_norefresh				= {	"r_norefresh",			"0"					};
cvar_t	r_drawentities			= {	"r_drawentities",		"1"					};
cvar_t	r_drawviewmodel			= {	"r_drawviewmodel",		"1"					},
	cv_video_drawbrushes = { "video_drawbrushes", "1", false, true, "Toggles whether brushes are drawn in the level." };
cvar_t	r_speeds				= {	"r_speeds",				"0"					};
cvar_t	r_fullbright			= {	"r_fullbright",			"0"					};
cvar_t	r_lightmap				= {	"r_lightmap",			"0"					};
cvar_t	r_shadows				= {	"r_shadows",			"1",		true,	false,	"0 = Disabled\n1 = Blob shadows\n2 = Blob and planar shadows"	};
cvar_t	r_dynamic				= {	"r_dynamic",			"1"					};
cvar_t	r_novis					= {	"r_novis",				"0"					};
cvar_t	r_nocull				= {	"r_nocull",				"0"					};
cvar_t	gl_cull					= {	"gl_cull",				"1"					};
cvar_t	gl_polyblend			= {	"gl_polyblend",			"1"					};
cvar_t	gl_flashblend			= {	"gl_flashblend",		"0"					};
cvar_t	r_stereo				= {	"r_stereo",				"0"					};
cvar_t	r_stereodepth			= {	"r_stereodepth",		"128"				};
cvar_t	r_drawflat				= {	"r_drawflat",			"0"					};
cvar_t	r_flatlightstyles		= {	"r_flatlightstyles",	"0"					};
cvar_t	gl_fullbrights			= {	"gl_fullbrights",		"1",		true	};
cvar_t	gl_farclip				= {	"gl_farclip",			"16384",	true	};
ConsoleVariable_t cv_video_lightmapoversample = { "video_lightmapoversample", "8", true };
cvar_t	r_oldskyleaf			= {	"r_oldskyleaf",			"0"					};
cvar_t	r_drawworld				= {	"r_drawworld",			"1"					};
cvar_t	r_showtris				= {	"r_showtris",			"0"					};
cvar_t	r_showbboxes			= {	"r_showbboxes",			"0"					};
cvar_t	r_lerpmodels			= {	"r_lerpmodels",			"1"					};
cvar_t	r_lerpmove				= {	"r_lerpmove",			"1"					};

bool R_CullModelForEntity(ClientEntity_t *e)
{
	plVector3f_t mins, maxs;

	if(e == &cl.viewent)
		return false;

	Core::Camera *camera = g_cameramanager->GetCurrentCamera();
	if (!camera)
		return true;

	if(e->angles[PL_PITCH] || e->angles[PL_ROLL])
	{
		Math_VectorAdd (e->origin, e->model->rmins, mins);
		Math_VectorAdd (e->origin, e->model->rmaxs, maxs);
	}
	else if(e->angles[PL_YAW])
	{
		Math_VectorAdd (e->origin, e->model->ymins, mins);
		Math_VectorAdd (e->origin, e->model->ymaxs, maxs);
	}
	else //no rotation
	{
		Math_VectorAdd(e->origin,e->model->mins,mins);
		Math_VectorAdd(e->origin,e->model->maxs,maxs);
	}

	return camera->IsBoxOutsideFrustum(mins, maxs);
}

void R_RotateForEntity(plVector3f_t origin, plVector3f_t angles)
{
#ifdef VL_MODE_OPENGL
	glTranslatef(origin[0],origin[1],origin[2]);
	glRotatef(angles[1],0,0,1);
	glRotatef(-angles[0],0,1,0);
	glRotatef(angles[2],1,0,0);
#endif
}

/*	Negative offset moves polygon closer to camera
*/
void GL_PolygonOffset (int offset)
{
#ifdef VL_MODE_OPENGL
	if (offset > 0)
	{
		glEnable (GL_POLYGON_OFFSET_FILL);
		glEnable (GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(1, offset);
	}
	else if (offset < 0)
	{
		glEnable (GL_POLYGON_OFFSET_FILL);
		glEnable (GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1, offset);
	}
	else
	{
		glDisable (GL_POLYGON_OFFSET_FILL);
		glDisable (GL_POLYGON_OFFSET_LINE);
	}
#endif
}

int SignbitsForPlane (mplane_t *out)
{
	int	bits, j;

	// for fast box on planeside test
	bits = 0;
	for (j=0 ; j<3 ; j++)
		if (out->normal[j] < 0)
			bits |= 1<<j;

	return bits;
}

void R_RenderScene(void);
void R_UpdateWarpTextures(void);    // [25/11/2013] See gl_warp.c ~hogsy

void R_RenderScene(void)
{
	R_PushDlights();
	Light_Animate();

	r_framecount++;
}

void R_RenderView (void)
{
	double	time1 = 0,
			time2;

	if (r_norefresh.value)
		return;

#ifdef VL_MODE_OPENGL
	if(r_speeds.value)
	{
		glFinish ();
		time1 = System_DoubleTime();

		//johnfitz -- rendering statistics
		rs_brushpolys = rs_aliaspolys = rs_skypolys = rs_particles = rs_fogpolys = rs_megatexels =
		rs_dynamiclightmaps = rs_aliaspasses = rs_skypasses = rs_brushpasses = 0;
	}
#endif

	// stuff

	//johnfitz -- modified r_speeds output
	time2 = System_DoubleTime();
	if(r_speeds.value == 2)
		Con_Printf(	"%3i ms  %4i/%4i wpoly %4i/%4i epoly %3i lmap %4i/%4i sky %1.1f mtex\n",
					(int)((time2-time1)*1000),
					rs_brushpolys,
					rs_brushpasses,
					rs_aliaspolys,
					rs_aliaspasses,
					rs_dynamiclightmaps,
					rs_skypolys,
					rs_skypasses,
					TexMgr_FrameUsage ());
	else if(r_speeds.value)
		Con_Printf ("%3i ms  %4i wpoly %4i epoly %3i lmap\n",
					(int)((time2-time1)*1000),
					rs_brushpolys,
					rs_aliaspolys,
					rs_dynamiclightmaps);
	//johnfitz
}

