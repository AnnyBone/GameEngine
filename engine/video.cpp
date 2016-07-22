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

#include "engine_base.h"

#include "video.h"
#include "video_shader.h"
#include "video_light.h"

#include "client/effect_sprite.h"

/*	Video System	*/

bool 
	r_drawflat_cheatsafe, 
	r_fullbright_cheatsafe,
	r_lightmap_cheatsafe, 
	r_drawworld_cheatsafe;

#define VIDEO_STATE_ENABLE   0
#define VIDEO_STATE_DISABLE  1

ConsoleVariable_t
	cv_video_shaders = { "video_shaders", "1", true, false, "If enabled, disables usage of shaders and other fancy features." },
	cvLitParticles = { "video_particles_lit", "0", true, false, "Sets whether or not particles are lit by dynamic lights." },

	cv_video_msaasamples = { "video_multisamplesamples", "0", true, false, "Changes the number of samples." },
	cv_video_msaamaxsamples = { "video_msaamaxsamples", "16", true, false, "Sets the maximum number of allowed samples." },
	cv_video_fullscreen = { "video_fullscreen", "0", true, false, "1: Fullscreen, 0: Windowed" },
	cv_video_width = { "video_width", "640", true, false, "Sets the width of the window." },
	cv_video_height = { "video_height", "480", true, false, "Sets the height of the window." },
	cv_video_verticlesync = { "video_verticalsync", "0", true },

	cv_video_drawmirrors = { "video_drawmirrors", "1", true, false, "Enables and disables the rendering of mirror surfaces." },
	cv_video_drawmodels = { "video_drawmodels", "1", false, false, "Toggles models." },
	cv_video_drawdepth = { "video_drawdepth", "0", false, false, "If enabled, previews the debth buffer." },
	cv_video_drawdetail = { "video_drawdetail", "1", true, false, "If enabled, detail maps are drawn." },
	cv_video_drawmaterials = { "video_drawmaterials", "1", false, false, "If enabled, materials are drawn." },
	cv_video_drawsky = { "video_drawsky", "1", false, false, "Toggles rendering of the sky." },
	cv_video_drawplayershadow = { "video_drawplayershadow", "1", true, false, "If enabled, the players own shadow will be drawn." },
	
	cv_video_shownormals = { "video_shownormals", "0", false, false, "If enabled, draws lines representing vertex normals." },

	cv_video_clearbuffers = { "video_clearbuffers", "1", true, false },
	cv_video_detailscale = { "video_detailscale", "3", true, false, "Changes the scaling used for detail maps." },
	cv_video_alphatrick = { "video_alphatrick", "1", true, false, "If enabled, draws alpha-tested surfaces twice for extra quality." },
	cv_video_finish = { "video_finish", "0", true, false, "If enabled, calls glFinish at the end of the frame." },
	cv_video_log = { "video_debuglog", "video", true, false, "The name of the output log for video debugging." },

	cv_video_drawshadowmap = { "video_draw_shadowmap", "1", true, false, "Enables/disables the rendering of shadow maps." },
	cv_video_drawshadowblob = { "video_draw_shadowblob", "1", true, false, "Enables/disables the rendering of a shadow blob." },

	cv_video_entity_distance = { "video_entity_distance", "1000" },
	cv_video_entity_fade = { "video_entity_fade", "1" };

// TODO: Move this? It's used mainly for silly client stuff...
struct gltexture_s *g_effecttextures[MAX_EFFECTS];

Video_t	Video;

texture_t	*r_notexture_mip;
texture_t	*r_notexture_mip2;	//johnfitz -- used for non-lightmapped surfs with a missing texture

using namespace core;

Viewport *video_viewport = nullptr;

/*	Initialize the renderer
*/
void Video_Initialize(void)
{
	// Ensure we haven't already been initialized.
	if(Video.bInitialized)
		return;

	Con_Printf("Initializing video...\n");

	// Only enabled if the hardware supports it.
	Video.extensions.vertex_buffer_object	= false;
	Video.extensions.generate_mipmap		= false;
	Video.extensions.depth_texture			= false;
	Video.extensions.shadow					= false;

	Cvar_RegisterVariable(&cv_video_msaasamples, NULL);
	Cvar_RegisterVariable(&cv_video_drawmodels, NULL);
	Cvar_RegisterVariable(&cv_video_fullscreen, NULL);
	Cvar_RegisterVariable(&cv_video_width, NULL);
	Cvar_RegisterVariable(&cv_video_height, NULL);
	Cvar_RegisterVariable(&cv_video_verticlesync, NULL);
	Cvar_RegisterVariable(&cvLitParticles, NULL);
	Cvar_RegisterVariable(&cv_video_log, NULL);
	Cvar_RegisterVariable(&cv_video_drawdepth, NULL);
	Cvar_RegisterVariable(&cv_video_finish, NULL);
	Cvar_RegisterVariable(&cv_video_alphatrick, NULL);
	Cvar_RegisterVariable(&cv_video_drawmirrors, NULL);
	Cvar_RegisterVariable(&cv_video_drawmaterials, NULL);
	Cvar_RegisterVariable(&cv_video_drawdetail, NULL);
	Cvar_RegisterVariable(&cv_video_drawsky, NULL);
	Cvar_RegisterVariable(&cv_video_drawshadowmap, NULL);
	Cvar_RegisterVariable(&cv_video_drawshadowblob, NULL);
	Cvar_RegisterVariable(&cv_video_detailscale, NULL);
	Cvar_RegisterVariable(&cv_video_drawplayershadow, NULL);
	Cvar_RegisterVariable(&cv_video_shaders, NULL);
	Cvar_RegisterVariable(&cv_video_clearbuffers, NULL);
	Cvar_RegisterVariable(&cv_video_entity_distance, NULL);
	Cvar_RegisterVariable(&cv_video_entity_fade, NULL);
	Cvar_RegisterVariable(&cv_video_shownormals, NULL);

	Cmd_AddCommand("video_restart", Window_Update);

	vlInit();

	// Attempt to dynamically allocate the number of supported TMUs.
	vlGetMaxTextureImageUnits(&Video.num_textureunits);
	Video.textureunits = (VideoTextureMU_t*)Hunk_Alloc(sizeof(VideoTextureMU_t)*Video.num_textureunits);
	if (!Video.textureunits)
		Sys_Error("Failed to allocated handler for the number of supported TMUs! (%i)\n", Video.num_textureunits);

	for (int i = 0; i < Video.num_textureunits; i++)
	{
		Video.textureunits[i].isactive			= false;						// All units are initially disabled.
		Video.textureunits[i].current_envmode	= VIDEO_TEXTUREMODE_REPLACE;
		Video.textureunits[i].current_texture	= (unsigned int)-1;
	}

	vlGetMaxTextureAnistropy(&Video.fMaxAnisotropy);

	//johnfitz -- create notexture miptex
	r_notexture_mip = (texture_t*)Hunk_AllocName(sizeof(texture_t), "r_notexture_mip");
	strcpy(r_notexture_mip->name, "notexture");
	r_notexture_mip->height = r_notexture_mip->width = 32;

	r_notexture_mip2 = (texture_t*)Hunk_AllocName(sizeof(texture_t), "r_notexture_mip2");
	strcpy(r_notexture_mip2->name, "notexture2");
	r_notexture_mip2->height = r_notexture_mip2->width = 32;
	//johnfitz

	Video.vertical_sync = cv_video_verticlesync.bValue;

	draw::SetDefaultState();

	Light_Initialize();

	g_shadermanager = new ShaderManager();
	g_cameramanager = new CameraManager();
	g_spritemanager = new SpriteManager();

	if (!g_state.embedded)
	{
		Camera *newcam = g_cameramanager->CreateCamera();
		g_cameramanager->SetCurrentCamera(newcam);

		video_viewport = new Viewport(0, 0, g_mainwindow.width, g_mainwindow.height);
		video_viewport->SetCamera(newcam);

		SetPrimaryViewport(video_viewport);
		SetCurrentViewport(video_viewport);
	}

	Video.bInitialized = true;
}

/*	Window Management	*/

void Video_SetViewportSize(unsigned int w, unsigned int h)
{
	video_viewport->SetSize(w, h);
}

/*	Coordinate Generation	*/

void Video_GenerateSphereCoordinates(void)
{
#ifdef VL_MODE_OPENGL
#if 0
	MathMatrix4x4f_t mmMatrix, mmInversed;

	glPushMatrix();
	glMatrixMode(GL_TEXTURE);
	glGetFloatv(GL_PROJECTION_MATRIX, mmMatrix);

	// Inverse the matrix.
	Math_Matrix4x4Negate(mmMatrix, mmInversed);

	// Apply it.
	glLoadMatrixf(mmInversed);
#endif

	// Generate the sphere map coords.
	glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
	glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);

#if 0
	// Reset the matrix.
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
#endif
#endif
}

/**/

/*	Bind our current texture.
*/
void Video_SetTexture(gltexture_t *gTexture)
{
	if(!gTexture)
		gTexture = notexture;
	// If it's the same as the last, don't bother.
	else if (gTexture->texnum == Video.textureunits[Video.current_textureunit].current_texture)
		return;

	Video.textureunits[Video.current_textureunit].current_texture = gTexture->texnum;

	gTexture->visframe = r_framecount;

	// Bind it.
	vlBindTexture(VL_TEXTURE_2D, gTexture->texnum);
}

/*	Object Management	*/

void Video_ObjectTexture(vlVertex_t *object, unsigned int uiTextureUnit, float S, float T)
{
	object->ST[uiTextureUnit][0] = S;
	object->ST[uiTextureUnit][1] = T;
}

void Video_ObjectVertex(vlVertex_t *object, float x, float y, float z)
{
	plVectorSet3f(object->position, x, y, z);
}

void Video_ObjectNormal(vlVertex_t *object, float x, float y, float z)
{
	plVectorSet3f(object->normal, x, y, z);
}

void Video_ObjectColour(vlVertex_t *object, float R, float G, float B, float A)
{
	object->colour[PL_RED]		= R;
	object->colour[PL_GREEN]	= G;
	object->colour[PL_BLUE]		= B;
	object->colour[PL_ALPHA]	= A;
}

/*	Drawing	*/

/*  Draw a simple rectangle.
*/
void Video_DrawFill(vlVertex_t *voFill, Material_t *mMaterial, int iSkin)
{
	Video_DrawObject(voFill, VL_PRIMITIVE_TRIANGLE_FAN, 4, mMaterial, iSkin);
}

/*	Surfaces
*/
void Video_DrawSurface(msurface_t *mSurface,float fAlpha, Material_t *mMaterial, unsigned int uiSkin)
{
	vlVertex_t	*drawsurf;
	float		*fVert;
	int			i;
	
	drawsurf = (vlVertex_t*)calloc_or_die(mSurface->polys->numverts, sizeof(vlVertex_t));

	fVert = mSurface->polys->verts[0];
	for (i = 0; i < mSurface->polys->numverts; i++, fVert += VERTEXSIZE)
	{
#ifdef _MSC_VER
#	pragma warning(suppress: 6011)
#endif
		Video_ObjectVertex(&drawsurf[i], fVert[0], fVert[1], fVert[2]);
		Video_ObjectTexture(&drawsurf[i], VIDEO_TEXTURE_DIFFUSE, fVert[3], fVert[4]);
		Video_ObjectTexture(&drawsurf[i], VIDEO_TEXTURE_LIGHT, fVert[5], fVert[6]);
		Video_ObjectColour(&drawsurf[i], 1.0f, 1.0f, 1.0f, fAlpha);
	}

	Video_DrawObject(drawsurf, VL_PRIMITIVE_TRIANGLE_FAN, mSurface->polys->numverts, mMaterial, 0);
	free(drawsurf);

	rs_brushpasses++;
}

/*	Draw 3D object.
	TODO: Add support for VBOs ?
*/
void Video_DrawObject(vlVertex_t *vobject, vlPrimitive_t primitive,
	unsigned int numverts, Material_t *mMaterial, int iSkin)
{
	if(numverts == 0)
		return;

	vlDraw_t tempobj;
	tempobj.vertices			= vobject;
	tempobj.numverts			= numverts;
	tempobj.primitive			= primitive;
	tempobj.primitive_restore	= primitive;

	// Set the skin and ensure it's valid.
	Material_SetSkin(mMaterial, iSkin);

	Material_DrawObject(mMaterial, &tempobj, false);
	vlDraw(&tempobj);
	Material_DrawObject(mMaterial, &tempobj, true);
}

/*	Main rendering loop.
*/
void Video_Frame(void)
{
	if (g_state.embedded || (Video.bInitialized == false))
		return;

	// Don't let us exceed a limited count.
	Video.framecount++; if (Video.framecount == ((unsigned int)-1)) Video.framecount = 0;

	r_framecount++;

	double time1 = 0;
	if (r_speeds.value)
	{
		vlFinish();

		time1 = System_DoubleTime();

		//johnfitz -- rendering statistics
		rs_brushpolys = rs_aliaspolys = rs_skypolys = rs_particles = rs_fogpolys =
			rs_dynamiclightmaps = rs_aliaspasses = rs_skypasses = rs_brushpasses = 0;
	}

	video_viewport->Draw();

	//johnfitz -- modified r_speeds output
	double time2 = System_DoubleTime();
	if (r_speeds.value == 2)
		Con_Printf("%3i ms  %4i/%4i wpoly %4i/%4i epoly %3i lmap %4i/%4i sky %1.1f mtex\n",
		(int)((time2 - time1) * 1000),
		rs_brushpolys,
		rs_brushpasses,
		rs_aliaspolys,
		rs_aliaspasses,
		rs_dynamiclightmaps,
		rs_skypolys,
		rs_skypasses,
		TexMgr_FrameUsage());
	else if (r_speeds.value)
		Con_Printf("%3i ms  %4i wpoly %4i epoly %3i lmap\n",
		(int)((time2 - time1) * 1000),
		rs_brushpolys,
		rs_aliaspolys,
		rs_dynamiclightmaps);
	//johnfitz

	if (cv_video_finish.bValue)
		vlFinish();

	if (!Video.bSkipUpdate)
		Window_Swap();
}

/*	Shuts down the video sub-system.
*/
void Video_Shutdown(void)
{
	// Check that the video sub-system is actually initialised.
	if(!Video.bInitialized)
		return;

	// Let us know that we're shutting down the video sub-system.
	Con_Printf("Shutting down video...\n");

	if (g_spritemanager) delete g_spritemanager;
	if (g_shadermanager) delete g_shadermanager;
	if (g_cameramanager) delete g_cameramanager;

	if (!g_state.embedded)
	{
		if (video_viewport) delete video_viewport;

		Window_Shutdown();
	}

	// Set the initialisation value to false, in-case we want to try again later.
	Video.bInitialized = false;
}
