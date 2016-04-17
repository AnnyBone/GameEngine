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

#include "engine_base.h"

#include "video.h"
#include "video_shader.h"
#include "video_light.h"

#include "client/effect_sprite.h"

/*
	Video System
*/

extern "C" {
	viddef_t vid; // Legacy global video state (TODO: Replace!)
}

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
	cv_video_log = { "video_debuglog", "video", true, false, "The name of the output log for video debugging." };
ConsoleVariable_t cv_video_drawshadowmap = { "video_draw_shadowmap", "1", true, false, "Enables/disables the rendering of shadow maps." };
ConsoleVariable_t cv_video_drawshadowblob = { "video_draw_shadowblob", "1", true, false, "Enables/disables the rendering of a shadow blob." };

#define VIDEO_MAX_SAMPLES	cv_video_msaamaxsamples.iValue
#define VIDEO_MIN_SAMPLES	0

// TODO: Move this? It's used mainly for silly client stuff...
struct gltexture_s *gEffectTexture[MAX_EFFECTS];

bool bVideoIgnoreCapabilities = false;

void Video_DebugCommand(void);

Video_t	Video;

texture_t	*r_notexture_mip;
texture_t	*r_notexture_mip2;	//johnfitz -- used for non-lightmapped surfs with a missing texture

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

	// Give everything within the video sub-system its default value.
	Video.debug_frame		= false;	// Not debugging the initial frame!
	Video.bActive			= true;		// Window is intially assumed active.
	Video.unlocked			= true;		// Video mode is initially locked.
	Video.current_program	= 0;

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

	Cvar_RegisterVariable(&cv_video_shownormals, NULL);

	Cmd_AddCommand("video_restart",Video_UpdateWindow);
	Cmd_AddCommand("video_debug",Video_DebugCommand);

	// Figure out what resolution we're going to use.
	if (COM_CheckParm("-window"))
	{
		Video.fullscreen = false;
		Video.unlocked = false;
	}
	else
		// Otherwise set us as fullscreen.
		Video.fullscreen = cv_video_fullscreen.bValue;

	if (COM_CheckParm("-width"))
	{
		Video.iWidth = atoi(com_argv[COM_CheckParm("-width") + 1]);
		Video.unlocked = false;
	}
	else
		Video.iWidth = cv_video_width.iValue;

	if (COM_CheckParm("-height"))
	{
		Video.iHeight = atoi(com_argv[COM_CheckParm("-height") + 1]);
		Video.unlocked = false;
	}
	else
		Video.iHeight = cv_video_height.iValue;

	if (!g_state.embedded)
		Window_InitializeVideo();

	vlInit();

	// Attempt to dynamically allocated the number of supported TMUs.
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

	// Get any information that will be presented later.
	Video.gl_vendor			= vlGetString(VL_STRING_VENDOR);
	Video.gl_renderer		= vlGetString(VL_STRING_RENDERER);
	Video.gl_version		= vlGetString(VL_STRING_VERSION);
	Video.gl_extensions		= vlGetString(VL_STRING_EXTENSIONS);

	// Set the default states...
	vlSetCullMode(VL_CULL_NEGATIVE);
#ifdef VL_MODE_OPENGL
	glAlphaFunc(GL_GREATER, 0.5f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glDepthRange(0, 1);
	glDepthFunc(GL_LEQUAL);
	glClearStencil(1);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Overbrights.
	vlActiveTexture(VIDEO_TEXTURE_LIGHT);
	vlSetTextureEnvironmentMode(VIDEO_TEXTUREMODE_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 4);
#endif
	vlActiveTexture(0);

	//johnfitz -- create notexture miptex
	r_notexture_mip = (texture_t*)Hunk_AllocName(sizeof(texture_t), "r_notexture_mip");
	strcpy(r_notexture_mip->name, "notexture");
	r_notexture_mip->height = r_notexture_mip->width = 32;

	r_notexture_mip2 = (texture_t*)Hunk_AllocName(sizeof(texture_t), "r_notexture_mip2");
	strcpy(r_notexture_mip2->name, "notexture2");
	r_notexture_mip2->height = r_notexture_mip2->width = 32;
	//johnfitz

	vid.conwidth = (scr_conwidth.value > 0) ? (int)scr_conwidth.value : (scr_conscale.value > 0) ? (int)(Video.iWidth / scr_conscale.value) : Video.iWidth;
	vid.conwidth = Math_Clamp(320, vid.conwidth, Video.iWidth);
	vid.conwidth &= 0xFFFFFFF8;
	vid.conheight = vid.conwidth*Video.iHeight / Video.iWidth;

	Video.vertical_sync = cv_video_verticlesync.bValue;

	Light_Initialize();

	g_shadermanager = new Core::ShaderManager();
	g_spritemanager = new Core::SpriteManager();

	Video.bInitialized = true;
}

/*
	Video Commands
*/

void Video_DebugCommand(void)
{
	if (!Video.debug_frame)
		Video.debug_frame = true;

	plClearLog(cv_video_log.string);
}

/**/

/*	Clears the color, stencil and depth buffers.
*/
void Video_ClearBuffer(void)
{
	if (!cv_video_clearbuffers.bValue)
		return;

	vlClearBuffers(VL_MASK_DEPTH | VL_MASK_COLOUR | VL_MASK_STENCIL);
}

/*	Displays the depth buffer for testing purposes.
	Unfinished
*/
void Video_DrawDepthBuffer(void)
{
#if 0
	static gltexture_t	*depth_texture = NULL;
	float				*uByte;

	if(!cv_video_drawdepth.bValue)
		return;

	// Allocate the pixel data.
	uByte = (float*)malloc(Video.iWidth*Video.iHeight*sizeof(float));
	if (!uByte)
		return;

	// Read le pixels, and copy them to uByte.
	glReadPixels(0, 0, Video.iWidth, Video.iHeight, GL_DEPTH_COMPONENT, GL_FLOAT, uByte);

	// Create our depth texture.
	depth_texture = TexMgr_NewTexture();

	// Set the texture.
	Video_SetTexture(depth_texture);

	// Copy it to the texture.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, Video.iWidth, Video.iHeight, 0, GL_LUMINANCE, GL_FLOAT, uByte);

	// Draw the buffer to the bottom left corner of the screen.
	GL_SetCanvas(CANVAS_BOTTOMLEFT);
	Draw_Rectangle(0, 0, 512, 512, pl_white);
	GL_SetCanvas(CANVAS_DEFAULT);

	// Delete the texture, so we can recreate it later.
	TexMgr_FreeTexture(depth_texture);

	// Free the pixel data.
	free(uByte);
#endif
}

/*
	Window Management
*/

void Video_UpdateWindow(void)
{
	if (g_state.embedded || !Video.bInitialized || !Video.bActive)
		return;

	if (!Video.unlocked)
	{
		Cvar_SetValue(cv_video_fullscreen.name, (float)Video.fullscreen);
		Cvar_SetValue(cv_video_width.name, (float)Video.iWidth);
		Cvar_SetValue(cv_video_height.name, (float)Video.iHeight);
		Cvar_SetValue(cv_video_verticlesync.name, (float)Video.vertical_sync);

		Video.unlocked = true;
		return;
	}

	// Ensure the given width and height are within reasonable bounds.
	if (cv_video_width.iValue < WINDOW_MINIMUM_WIDTH ||
		cv_video_height.iValue < WINDOW_MINIMUM_HEIGHT)
	{
		Con_Warning("Failed to get an appropriate resolution!\n");

		Cvar_SetValue(cv_video_width.name, WINDOW_MINIMUM_WIDTH);
		Cvar_SetValue(cv_video_height.name, WINDOW_MINIMUM_HEIGHT);
	}
	// If we're not fullscreen, then constrain our window size to the size of the desktop.
	else if (!Video.fullscreen && ((cv_video_width.iValue > plGetScreenWidth()) || (cv_video_height.iValue > plGetScreenHeight())))
	{
		Con_Warning("Attempted to set resolution beyond scope of desktop!\n");

		Cvar_SetValue(cv_video_width.name, plGetScreenWidth());
		Cvar_SetValue(cv_video_height.name, plGetScreenHeight());
	}

	Video.iWidth = cv_video_width.iValue;
	Video.iHeight = cv_video_height.iValue;

	Window_UpdateVideo();

	// Update console size.
	SCR_Conwidth_f();
}

void Video_SetViewportSize(int w, int h)
{
	if (w <= 0)
		w = 1;
	if (h <= 0)
		h = 1;

	Video.iWidth = w;
	Video.iHeight = h;

	vid.bRecalcRefDef = true;

	// Update console size.
	vid.conwidth = Video.iWidth & 0xFFFFFFF8;
	vid.conheight = vid.conwidth*Video.iHeight / Video.iWidth;
}

/*
	Coordinate Generation
*/

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
#ifdef VL_MODE_OPENGL
	if(!gTexture)
		gTexture = notexture;
	// If it's the same as the last, don't bother.
	else if (gTexture->texnum == Video.textureunits[Video.current_textureunit].current_texture)
		return;

	Video.textureunits[Video.current_textureunit].current_texture = gTexture->texnum;

	gTexture->visframe = r_framecount;

	// Bind it.
	glBindTexture(GL_TEXTURE_2D,gTexture->texnum);

	if (Video.debug_frame)
		plWriteLog(cv_video_log.string, "Video: Bound texture (%s) (%i)\n", gTexture->name, Video.current_textureunit);
#endif
}

/*
}

/*
	Object Management
*/

MathVector4f_t mvVideoGlobalColour;

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

/*
	Drawing
*/

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

/**/

void Video_PreFrame(void)
{
	VIDEO_FUNCTION_START

	GL_BeginRendering(&glx, &gly, &glwidth, &glheight);

	Screen_UpdateSize();
	Screen_SetUpToDrawConsole();

	R_SetupGenericView();

	r_framecount++;

	R_SetupScene();

	Video_ShowBoundingBoxes();

	VIDEO_FUNCTION_END
}

/*	Main rendering loop.
*/
void Video_Frame(void)
{
	if (g_state.embedded || (Video.bInitialized == false))
		return;

	if (Video.debug_frame)
		plWriteLog(cv_video_log.string, "Video: Start of frame\n");

	SCR_UpdateScreen();

	GL_EndRendering();

	Video_PostFrame();
}

void Video_PostFrame(void)
{
	VIDEO_FUNCTION_START
	VIDEO_FUNCTION_END

	Draw_ResetCanvas();

	Screen_DrawFPS();

	if (cv_video_finish.bValue)
		vlFinish();

	if (Video.debug_frame)
		Video.debug_frame = false;
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

	if (g_spritemanager)
		delete g_spritemanager;
	if (g_shadermanager)
		delete g_shadermanager;

	if (!g_state.embedded)
		Window_Shutdown();

	// Set the initialisation value to false, in-case we want to try again later.
	Video.bInitialized = false;
}
