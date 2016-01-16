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

/*
	Video System

	TODO:
		Move all/most API-specific code here.
		Scale TMU support based on actual hardware, rather than set limitations.
*/

viddef_t vid; // Legacy global video state (TODO: Replace!)

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

/*	Initialize the renderer
*/
void Video_Initialize(void)
{
	int i;

	// Ensure we haven't already been initialized.
	if(Video.bInitialized)
		return;

	Con_Printf("Initializing video...\n");

	memset(Video.current_texture, -1, sizeof(int)*VIDEO_MAX_UNITS); // "To avoid unnecessary texture sets"

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

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &Video.num_textureunits);
	if (Video.num_textureunits < VIDEO_MAX_UNITS)
		Sys_Error("Your system doesn't support the required number of TMUs! (%i)\n", Video.num_textureunits);

	// Attempt to dynamically allocated the number of supported TMUs.
	Video.textureunits = (VideoTextureMU_t*)Hunk_Alloc(sizeof(VideoTextureMU_t)*Video.num_textureunits);
	if (!Video.textureunits)
		Sys_Error("Failed to allocated handler for the number of supported TMUs! (%i)\n", Video.num_textureunits);

	for (i = 0; i < Video.num_textureunits; i++)
	{
		Video.textureunits[i].isactive			= false;
		Video.textureunits[i].current_envmode	= VIDEO_TEXTURE_MODE_REPLACE;
		Video.textureunits[i].current_texture	= 0;
	}

	// All units are initially disabled.
	memset(Video.textureunit_state, 0, sizeof(Video.textureunit_state));

	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &Video.fMaxAnisotropy);

	// Get any information that will be presented later.
	Video.gl_vendor = (char*)glGetString(GL_VENDOR);
	Video.gl_renderer = (char*)glGetString(GL_RENDERER);
	Video.gl_version = (char*)glGetString(GL_VERSION);
	Video.gl_extensions = (char*)glGetString(GL_EXTENSIONS);

	GLeeInit();

	Con_DPrintf(" Checking for extensions...\n");

	// Check that the required capabilities are supported.
	if (!GLEE_ARB_multitexture) Sys_Error("Video hardware incapable of multi-texturing!\n");
	else if (!GLEE_ARB_texture_env_combine && !GLEE_EXT_texture_env_combine) Sys_Error("ARB/EXT_texture_env_combine isn't supported by your hardware!\n");
	else if (!GLEE_ARB_texture_env_add && !GLEE_EXT_texture_env_add) Sys_Error("ARB/EXT_texture_env_add isn't supported by your hardware!\n");
	//else if (!GLEE_EXT_fog_coord) Sys_Error("EXT_fog_coord isn't supported by your hardware!\n");
#ifdef VIDEO_SUPPORT_SHADERS
	else if (!GLEE_ARB_vertex_program || !GLEE_ARB_fragment_program) Sys_Error("Shaders aren't supported by this hardware!\n");
#endif

	// Optional capabilities.
	if (GLEE_SGIS_generate_mipmap) Video.extensions.generate_mipmap = true;
	else Con_Warning("Hardware mipmap generation isn't supported!\n");
	if (GLEE_ARB_depth_texture) Video.extensions.depth_texture = true;
	else Con_Warning("ARB_depth_texture isn't supported by your hardware!\n");
	if (GLEE_ARB_shadow) Video.extensions.shadow = true;
	else Con_Warning("ARB_shadow isn't supported by your hardware!\n");
	if (GLEE_ARB_vertex_buffer_object) Video.extensions.vertex_buffer_object = true;
	else Con_Warning("Hardware doesn't support Vertex Buffer Objects!\n");

	// Set the default states...

	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
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

	Video_SelectTexture(VIDEO_TEXTURE_LIGHT);

	// Overbrights.
	VideoLayer_SetTextureEnvironmentMode(VIDEO_TEXTURE_MODE_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 4);

	Video_SelectTexture(0);

	vid.conwidth = (scr_conwidth.value > 0) ? (int)scr_conwidth.value : (scr_conscale.value > 0) ? (int)(Video.iWidth / scr_conscale.value) : Video.iWidth;
	vid.conwidth = Math_Clamp(320, vid.conwidth, Video.iWidth);
	vid.conwidth &= 0xFFFFFFF8;
	vid.conheight = vid.conwidth*Video.iHeight / Video.iWidth;

	Video.vertical_sync = cv_video_verticlesync.bValue;

#ifdef VIDEO_SUPPORT_SHADERS
	VideoShader_Initialize();
#endif

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

	int clear = 0;
	if (r_showtris.bValue || (cls.state != ca_connected) || g_state.embedded)
		clear |= GL_COLOR_BUFFER_BIT;
	if (cv_video_drawmirrors.bValue)
		clear |= GL_STENCIL_BUFFER_BIT;

	glClear(GL_DEPTH_BUFFER_BIT | clear);
}

/*	Displays the depth buffer for testing purposes.
	Unfinished
*/
void Video_DrawDepthBuffer(void)
{
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
	Draw_Rectangle(0, 0, 512, 512, g_colourwhite);
	GL_SetCanvas(CANVAS_DEFAULT);

	// Delete the texture, so we can recreate it later.
	TexMgr_FreeTexture(depth_texture);

	// Free the pixel data.
	free(uByte);
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
}

/**/

/*	Bind our current texture.
*/
void Video_SetTexture(gltexture_t *gTexture)
{
	if(!gTexture)
		gTexture = notexture;
	// If it's the same as the last, don't bother.
	else if (gTexture->texnum == Video.current_texture[Video.current_textureunit])
		return;

	Video.current_texture[Video.current_textureunit] = gTexture->texnum;

	gTexture->visframe = r_framecount;

	// Bind it.
	glBindTexture(GL_TEXTURE_2D,gTexture->texnum);

	if (Video.debug_frame)
		plWriteLog(cv_video_log.string, "Video: Bound texture (%s) (%i)\n", gTexture->name, Video.current_textureunit);
}

/*
	Multitexturing Management
*/

/*	Conversion between our TMU selection and OpenGL.
*/
unsigned int Video_GetTextureUnit(unsigned int uiTarget)
{
	if (Video.debug_frame)
		plWriteLog(cv_video_log.string, "Video: Attempting to get TMU target %i\n", uiTarget);

#if 0
	switch (uiTarget)
	{
	case VIDEO_TEXTURE_DIFFUSE:
		uiUnit = GL_TEXTURE0;
		break;
	case VIDEO_TEXTURE_LIGHT:
		uiUnit = GL_TEXTURE1;
		break;
	case VIDEO_TEXTURE_DETAIL:
		uiUnit = GL_TEXTURE2;
		break;
	case VIDEO_TEXTURE_FULLBRIGHT:
		uiUnit = GL_TEXTURE3;
		break;
	case VIDEO_TEXTURE_SPHERE:
		uiUnit = GL_TEXTURE4;
		break;
	default:
		Sys_Error("Unknown texture unit! (%i)\n", uiTarget);
	}
#endif

	if (Video.debug_frame)
		plWriteLog(cv_video_log.string, "Video: Returning TMU %i\n", GL_TEXTURE0 + uiTarget);

	return GL_TEXTURE0 + uiTarget;
}

void Video_SelectTexture(unsigned int uiTarget)
{
	if (uiTarget == Video.current_textureunit)
		return;

	if (uiTarget > VIDEO_MAX_UNITS)
		Sys_Error("Invalid texture unit! (%i)\n",uiTarget);

	glActiveTexture(Video_GetTextureUnit(uiTarget));

	Video.current_textureunit = uiTarget;

	if (Video.debug_frame)
		plWriteLog(cv_video_log.string, "Video: Texture Unit %i\n", Video.current_textureunit);
}

/*
	Object Management
*/

MathVector4f_t mvVideoGlobalColour;

void Video_ObjectTexture(VideoObjectVertex_t *voObject, unsigned int uiTextureUnit, float S, float T)
{
	voObject->mvST[uiTextureUnit][0] = S;
	voObject->mvST[uiTextureUnit][1] = T;
}

void Video_ObjectVertex(VideoObjectVertex_t *voObject, float X, float Y, float Z)
{
	voObject->mvPosition[0] = X;
	voObject->mvPosition[1] = Y;
	voObject->mvPosition[2] = Z;
}

void Video_ObjectNormal(VideoObjectVertex_t *voObject, float X, float Y, float Z)
{
	voObject->mvNormal[0] = X;
	voObject->mvNormal[1] = Y;
	voObject->mvNormal[2] = Z;
}

void Video_ObjectColour(VideoObjectVertex_t *voObject, float R, float G, float B, float A)
{
	voObject->mvColour[pRED] = R;
	voObject->mvColour[pGREEN] = G;
	voObject->mvColour[pBLUE] = B;
	voObject->mvColour[pALPHA] = A;
}

/*
	Drawing
*/

/*  Draw a simple rectangle.
*/
void Video_DrawFill(VideoObjectVertex_t *voFill, Material_t *mMaterial, int iSkin)
{
	Video_DrawObject(voFill, VIDEO_PRIMITIVE_TRIANGLE_FAN, 4, mMaterial, iSkin);
}

/*	Surfaces
*/
void Video_DrawSurface(msurface_t *mSurface,float fAlpha, Material_t *mMaterial, unsigned int uiSkin)
{
	VideoObjectVertex_t	*drawsurf;
	float				*fVert;
	int					i;

	drawsurf = (VideoObjectVertex_t*)Hunk_TempAlloc(mSurface->polys->numverts*sizeof(VideoObjectVertex_t));
	if (!drawsurf)
		Sys_Error("Failed to allocate surface video object!\n");

	fVert = mSurface->polys->verts[0];
	for (i = 0; i < mSurface->polys->numverts; i++, fVert += VERTEXSIZE)
	{
#ifdef _MSC_VER
#pragma warning(suppress: 6011)
#endif
		Video_ObjectVertex(&drawsurf[i], fVert[0], fVert[1], fVert[2]);
		Video_ObjectTexture(&drawsurf[i], VIDEO_TEXTURE_DIFFUSE, fVert[3], fVert[4]);
		Video_ObjectTexture(&drawsurf[i], VIDEO_TEXTURE_LIGHT, fVert[5], fVert[6]);
		Video_ObjectColour(&drawsurf[i], 1.0f, 1.0f, 1.0f, fAlpha);
	}

	Video_DrawObject(drawsurf, VIDEO_PRIMITIVE_TRIANGLE_FAN, mSurface->polys->numverts, mMaterial, 0);
}

/*	Draw 3D object.
	TODO: Add support for VBOs ?
*/
void Video_DrawObject(VideoObjectVertex_t *vobject, VideoPrimitive_t primitive, 
	unsigned int numverts, Material_t *mMaterial, int iSkin)
{
	if (numverts == 0)
		return;

	if (Video.debug_frame)
		plWriteLog(cv_video_log.string, "Drawing object (%i) (%i)\n", numverts, primitive);

	bVideoIgnoreCapabilities = true;

	Material_Draw(mMaterial, iSkin, vobject, primitive, numverts, false);

	VideoObject_EnableDrawState();
	VideoObject_SetupPointers(vobject);

	bool showwireframe = r_showtris.bValue;
	if (mMaterial && mMaterial->override_wireframe)
		showwireframe = false;

	VideoLayer_DrawArrays(primitive, numverts, showwireframe);

	VideoObject_DisableDrawState();

	Material_Draw(mMaterial, iSkin, vobject, primitive, numverts, true);

	bVideoIgnoreCapabilities = false;
}

/*
	Capabilities management
*/

typedef struct
{
	unsigned	int	    uiFirst,
						uiSecond;

	const       char    *ccIdentifier;
} VideoCapabilities_t;

VideoCapabilities_t	vcCapabilityList[]=
{
	{ VIDEO_ALPHA_TEST, GL_ALPHA_TEST, "ALPHA_TEST" },
	{ VIDEO_BLEND, GL_BLEND, "BLEND" },
	{ VIDEO_DEPTH_TEST, GL_DEPTH_TEST, "DEPTH_TEST" },
	{ VIDEO_TEXTURE_2D, GL_TEXTURE_2D, "TEXTURE_2D" },
	{ VIDEO_TEXTURE_GEN_S, GL_TEXTURE_GEN_S, "TEXTURE_GEN_S" },
	{ VIDEO_TEXTURE_GEN_T, GL_TEXTURE_GEN_T, "TEXTURE_GEN_T" },
	{ VIDEO_CULL_FACE, GL_CULL_FACE, "CULL_FACE" },
	{ VIDEO_STENCIL_TEST, GL_STENCIL_TEST, "STENCIL_TEST" },
	{ VIDEO_NORMALIZE, GL_NORMALIZE, "NORMALIZE" },

	{ 0 }
};

/*	Set rendering capabilities for current draw.
	Cleared using Video_DisableCapabilities.
*/
void Video_EnableCapabilities(unsigned int iCapabilities)
{
	int	i;

	if(!iCapabilities)
		return;

	for(i = 0; i < sizeof(vcCapabilityList); i++)
	{
		if(!vcCapabilityList[i].uiFirst)
			break;

		if (iCapabilities & VIDEO_TEXTURE_2D)
			Video.textureunit_state[Video.current_textureunit] = true;

		if(iCapabilities & vcCapabilityList[i].uiFirst)
		{
			if(!bVideoIgnoreCapabilities)
				// Collect up a list of the new capabilities we set.
				Video.textureunits[Video.current_textureunit].capabilities[VIDEO_STATE_ENABLE] |= vcCapabilityList[i].uiFirst;

			glEnable(vcCapabilityList[i].uiSecond);
		}
	}
}

/*	Disables specified capabilities for the current draw.
*/
void Video_DisableCapabilities(unsigned int iCapabilities)
{
	int	i;

	if(!iCapabilities)
		return;

	for(i = 0; i < sizeof(vcCapabilityList); i++)
	{
		if(!vcCapabilityList[i].uiFirst)
			break;

		if (iCapabilities & VIDEO_TEXTURE_2D)
			Video.textureunit_state[Video.current_textureunit] = false;

		if(iCapabilities & vcCapabilityList[i].uiFirst)
		{
			if (Video.debug_frame)
				plWriteLog(cv_video_log.string, "Video: Disabling %s (%i)\n", vcCapabilityList[i].ccIdentifier, Video.current_textureunit);

			if(!bVideoIgnoreCapabilities)
				// Collect up a list of the new capabilities we disabled.
				Video.textureunits[Video.current_textureunit].capabilities[VIDEO_STATE_DISABLE] |= vcCapabilityList[i].uiFirst;

			glDisable(vcCapabilityList[i].uiSecond);
		}
	}
}

/*	Checks if the given capability is enabled or not.
*/
bool Video_GetCapability(unsigned int iCapability)
{
	int	i;

	if (!iCapability)
		return false;

	for (i = 0; i < sizeof(vcCapabilityList); i++)
	{
		if (!vcCapabilityList[i].uiFirst)
			break;

		if (iCapability & Video.textureunits[Video.current_textureunit].capabilities[VIDEO_STATE_ENABLE])
			return true;
	}

	return false;
}

/*	Resets our capabilities.
	Give an argument of true to only clear the list, not the capabilities.
	Also resets active blending mode.

	TODO: GET RID OF THIS!!!!
*/
void Video_ResetCapabilities(bool bClearActive)
{
	VIDEO_FUNCTION_START
	int i;

	if (Video.debug_frame)
		plWriteLog(cv_video_log.string, "Video: Resetting capabilities...\n");

	Video_SelectTexture(VIDEO_TEXTURE_DIFFUSE);

	if(bClearActive)
	{
		if (Video.debug_frame)
			plWriteLog(cv_video_log.string, "Video: Clearing active capabilities...\n");

		bVideoIgnoreCapabilities = true;

		// Set this back too...
		VideoLayer_SetTextureEnvironmentMode(VIDEO_TEXTURE_MODE_MODULATE);

		// Clear out capability list.
		for (i = 0; i < VIDEO_MAX_UNITS; i++)
		{
			Video_DisableCapabilities(Video.textureunits[i].capabilities[VIDEO_STATE_ENABLE]);
			Video_EnableCapabilities(Video.textureunits[i].capabilities[VIDEO_STATE_DISABLE]);

			Video.textureunits[i].capabilities[0] =
			Video.textureunits[i].capabilities[1] = 0;
		}

		VideoLayer_BlendFunc(VIDEO_BLEND_DEFAULT);
		VideoLayer_DepthMask(true);

		bVideoIgnoreCapabilities = false;

		if (Video.debug_frame)
			plWriteLog(cv_video_log.string, "Video: Finished clearing capabilities.\n");
	}
	VIDEO_FUNCTION_END
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

#ifdef VIDEO_SUPPORT_SHADERS
#ifdef VIDEO_SUPPORT_FRAMEBUFFERS
	//VideoPostProcess_BindFrameBuffer();
	DEBUG_FrameBufferBind();
#endif
#endif

	SCR_UpdateScreen();

#if 0
	// Attempt to draw the depth buffer.
	Video_DrawDepthBuffer();
#endif

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
		glFinish();

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

	if (!g_state.embedded)
		Window_Shutdown();

	// Set the initialisation value to false, in-case we want to try again later.
	Video.bInitialized = false;
}
