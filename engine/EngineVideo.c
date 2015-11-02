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

#include "EngineBase.h"

#include "EngineVideo.h"
#include "EngineVideoShader.h"
#include "EngineGame.h"

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

static unsigned int	iSavedCapabilites[VIDEO_MAX_UNITS][2];

#define VIDEO_STATE_ENABLE   0
#define VIDEO_STATE_DISABLE  1

ConsoleVariable_t
	cvVideoLegacy = { "video_legacy", "0", true, false, "If enabled, disables usage of shaders and other fancy features." },
	cvDrawFlares = { "video_flares", "1", true, false, "Toggles the rendering of environmental flares." },
	cvLitParticles = { "video_particles_lit", "0", true, false, "Sets whether or not particles are lit by dynamic lights." },
	cvMultisampleSamples = { "video_multisamplesamples", "0", true, false, "Changes the number of samples." },
	cvMultisampleMaxSamples = { "video_multisamplemaxsamples", "16", true, false, "Sets the maximum number of allowed samples." },
	cvFullscreen = { "video_fullscreen", "0", true, false, "1: Fullscreen, 0: Windowed" },
	cvWidth = { "video_width", "640", true, false, "Sets the width of the window." },
	cvHeight = { "video_height", "480", true, false, "Sets the height of the window." },
	cvVerticalSync = { "video_verticalsync", "0", true },
	cvVideoMirror = { "video_drawmirror", "1", true, false, "Enables and disables the rendering of mirror surfaces." },
	cvVideoDrawModels = { "video_drawmodels", "1", false, false, "Toggles models." },
	cvVideoDrawDepth = { "video_drawdepth", "0", false, false, "If enabled, previews the debth buffer." },
	cvVideoDrawDetail = { "video_drawdetail", "1", true, false, "If enabled, detail maps are drawn." },
	cvVideoDrawMaterials = { "video_drawmaterials", "1", true, false, "If enabled, materials are drawn." },
	cvVideoDrawSky = { "video_drawsky", "1", false, false, "Toggles rendering of the sky." },
	cvVideoDetailScale = { "video_detailscale", "3", true, false, "Changes the scaling used for detail maps." },
	cvVideoAlphaTrick = { "video_alphatrick", "1", true, false, "If enabled, draws alpha-tested surfaces twice for extra quality." },
	cvVideoFinish = { "video_finish", "0", true, false, "If enabled, calls glFinish at the end of the frame." },
	cvVideoVBO = { "video_vbo", "0", true, false, "Enables support of Vertex Buffer Objects." },
	cvVideoPlayerShadow = { "video_playershadow", "1", true, false, "If enabled, the players own shadow will be drawn." },
	cvVideoDebugLog = { "video_debuglog", "video", true, false, "The name of the output log for video debugging." };
ConsoleVariable_t cvVideoDrawShadowMap = { "video_draw_shadowmap", "1", true, false, "Enables/disables the rendering of shadow maps." };
ConsoleVariable_t cvVideoDrawShadowBlob = { "video_draw_shadowblob", "1", true, false, "Enables/disables the rendering of a shadow blob." };

#define VIDEO_MAX_SAMPLES	cvMultisampleMaxSamples.iValue
#define VIDEO_MIN_SAMPLES	0

gltexture_t	*gDepthTexture;

// TODO: Move this? It's used mainly for silly client stuff...
struct gltexture_s *gEffectTexture[MAX_EFFECTS];

bool bVideoIgnoreCapabilities = false;

unsigned int uiVideoDrawObjectCalls = 0;

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

	memset(Video.uiCurrentTexture, -1, sizeof(int)*VIDEO_MAX_UNITS); // "To avoid unnecessary texture sets"

	// Give everything within the video sub-system its default value.
	Video.bSupportsVBO			= false;	// Only enabled if the hardware supports it.
	Video.bSupportsHWMipmap		= false;	// Only enabled if the hardware supports it.
	Video.bSupportsDepthTexture = false;	// Only enabled if the hardware supports it.
	Video.bSupportsShadow		= false;	// Only enabled if the hardware supports it.
	Video.bDebugFrame			= false;	// Not debugging the initial frame!
	Video.bActive				= true;		// Window is intially assumed active.
	Video.bUnlocked				= true;		// Video mode is initially locked.

	Cvar_RegisterVariable(&cvMultisampleSamples,NULL);
	Cvar_RegisterVariable(&cvVideoDrawModels,NULL);
	Cvar_RegisterVariable(&cvFullscreen,NULL);
	Cvar_RegisterVariable(&cvWidth,NULL);
	Cvar_RegisterVariable(&cvHeight,NULL);
	Cvar_RegisterVariable(&cvVerticalSync,NULL);
	Cvar_RegisterVariable(&cvLitParticles, NULL);
	Cvar_RegisterVariable(&cvDrawFlares, NULL);
	Cvar_RegisterVariable(&cvVideoDebugLog,NULL);
	Cvar_RegisterVariable(&cvVideoDrawDepth,NULL);
	Cvar_RegisterVariable(&cvVideoFinish, NULL);
	Cvar_RegisterVariable(&cvVideoAlphaTrick, NULL);
	Cvar_RegisterVariable(&cvVideoMirror, NULL);
	Cvar_RegisterVariable(&cvVideoDrawMaterials, NULL);
	Cvar_RegisterVariable(&cvVideoDrawDetail, NULL);
	Cvar_RegisterVariable(&cvVideoDrawSky, NULL);
	Cvar_RegisterVariable(&cvVideoDrawShadowMap, NULL);
	Cvar_RegisterVariable(&cvVideoDrawShadowBlob, NULL);
	Cvar_RegisterVariable(&cvVideoDetailScale, NULL);
	Cvar_RegisterVariable(&cvVideoPlayerShadow, NULL);
	Cvar_RegisterVariable(&cvVideoLegacy, NULL);

	Cmd_AddCommand("video_restart",Video_UpdateWindow);
	Cmd_AddCommand("video_debug",Video_DebugCommand);

	// Figure out what resolution we're going to use.
	if (COM_CheckParm("-window"))
	{
		Video.bFullscreen = false;
		Video.bUnlocked = false;
	}
	else
		// Otherwise set us as fullscreen.
		Video.bFullscreen = cvFullscreen.bValue;

	if (COM_CheckParm("-width"))
	{
		Video.iWidth = atoi(com_argv[COM_CheckParm("-width") + 1]);
		Video.bUnlocked = false;
	}
	else
		Video.iWidth = cvWidth.iValue;

	if (COM_CheckParm("-height"))
	{
		Video.iHeight = atoi(com_argv[COM_CheckParm("-height") + 1]);
		Video.bUnlocked = false;
	}
	else
		Video.iHeight = cvHeight.iValue;

	if (!Global.bEmbeddedContext)
		Window_InitializeVideo();

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &Video.iSupportedUnits);
	if (Video.iSupportedUnits < VIDEO_MAX_UNITS)
		Sys_Error("Your system doesn't support the required number of TMUs! (%i)\n", Video.iSupportedUnits);

	// Attempt to dynamically allocated the number of supported TMUs.
	Video.TextureUnits = (VideoTextureMU_t*)Hunk_Alloc(sizeof(VideoTextureMU_t)*Video.iSupportedUnits);
	if (!Video.TextureUnits)
		Sys_Error("Failed to allocated handler for the number of supported TMUs! (%i)\n", Video.iSupportedUnits);

	for (i = 0; i < Video.iSupportedUnits; i++)
	{
		Video.TextureUnits[i].bIsActive = false;
		Video.TextureUnits[i].CurrentTexEnvMode = VIDEO_TEXTURE_MODE_REPLACE;
		Video.TextureUnits[i].uiCurrentTexture = 0;
	}

	// All units are initially disabled.
	Q_memset(Video.bUnitState, 0, sizeof(Video.bUnitState));

	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &Video.fMaxAnisotropy);

	// Get any information that will be presented later.
	Video.cGLVendor = (char*)glGetString(GL_VENDOR);
	Video.cGLRenderer = (char*)glGetString(GL_RENDERER);
	Video.cGLVersion = (char*)glGetString(GL_VERSION);
	Video.cGLExtensions = (char*)glGetString(GL_EXTENSIONS);

	GLeeInit();

	Con_DPrintf(" Checking for extensions...\n");

	// Check that the required capabilities are supported.
	if (!GLEE_ARB_multitexture)
		Sys_Error("Video hardware incapable of multi-texturing!\n");
	else if (!GLEE_ARB_texture_env_combine && !GLEE_EXT_texture_env_combine)
		Sys_Error("ARB/EXT_texture_env_combine isn't supported by your hardware!\n");
	else if (!GLEE_ARB_texture_env_add && !GLEE_EXT_texture_env_add)
		Sys_Error("ARB/EXT_texture_env_add isn't supported by your hardware!\n");
	else if (!GLEE_EXT_fog_coord)
		Sys_Error("EXT_fog_coord isn't supported by your hardware!\n");

	if (GLEE_SGIS_generate_mipmap) Video.bSupportsHWMipmap = true;
	else Con_Warning("Hardware mipmap generation isn't supported!\n");
	if (GLEE_ARB_depth_texture) Video.bSupportsDepthTexture = true;
	else Con_Warning("ARB_depth_texture isn't supported by your hardware!\n");
	if (GLEE_ARB_shadow) Video.bSupportsShadow = true;
	else Con_Warning("ARB_shadow isn't supported by your hardware!\n");
	if (GLEE_ARB_vertex_buffer_object) Video.bSupportsVBO = true;
	else Con_Warning("Hardware doesn't support Vertex Buffer Objects!\n");

#ifdef VIDEO_SUPPORT_SHADERS
	// Shaders?
	if (!GLEE_ARB_vertex_program || !GLEE_ARB_fragment_program)
		Sys_Error("Unsupported video hardware!\n");
#endif

	// Set the default states...

	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glAlphaFunc(GL_GREATER, 0.5f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glDepthRange(0, 1);
	glDepthFunc(GL_LEQUAL);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	Video_SelectTexture(VIDEO_TEXTURE_LIGHT);

	// Overbrights
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

	Video.bVerticalSync = cvVerticalSync.bValue;

#ifdef VIDEO_SUPPORT_SHADERS
	VideoShader_Initialize();
#endif

	Video.bInitialized = true;
}

/*	Change the render mode, primarily for the editor.
*/
void Video_SetRenderMode(VideoRenderMode_t NewMode)
{
	Video.CurrentRenderMode = NewMode;
}

/*
	Video Commands
*/

void Video_DebugCommand(void)
{
	if(!Video.bDebugFrame)
		Video.bDebugFrame = true;

	pLog_Clear(cvVideoDebugLog.string);
}

/**/

/*	Clears the color and depth buffers.
*/
void Video_ClearBuffer(void)
{
	int	iClear =
		GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT;

	// Handle the stencil buffer too.
	if(r_shadows.value >= 2)
	{
		glClearStencil(1);

		iClear |= GL_STENCIL_BUFFER_BIT;
	}

	glClear(iClear);
}

/*	Displays the depth buffer for testing purposes.
	Unfinished
*/
void Video_DrawDepthBuffer(void)
{
	float *uByte;

	if(!cvVideoDrawDepth.bValue)
		return;

	// Allocate the pixel data.
	uByte = (float*)malloc(Video.iWidth*Video.iHeight*sizeof(float));
	if (!uByte)
		return;

	// Read le pixels, and copy them to uByte.
	glReadPixels(0, 0, Video.iWidth, Video.iHeight, GL_DEPTH_COMPONENT, GL_FLOAT, uByte);

	// Create our depth texture.
	gDepthTexture = TexMgr_NewTexture();

	// Set the texture.
	Video_SetTexture(gDepthTexture);

	// Copy it to the texture.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, Video.iWidth, Video.iHeight, 0, GL_LUMINANCE, GL_FLOAT, uByte);

	// Draw the buffer to the bottom left corner of the screen.
	GL_SetCanvas(CANVAS_BOTTOMLEFT);
	Draw_Fill(0,0,512,512,1.0f,1.0f,1.0f,1.0f);
	GL_SetCanvas(CANVAS_DEFAULT);

	// Delete the texture, so we can recreate it later.
	TexMgr_FreeTexture(gDepthTexture);

	// Free the pixel data.
	free(uByte);
}

/*
	Window Management
*/

void Video_UpdateWindow(void)
{
	if (Global.bEmbeddedContext || !Video.bInitialized || !Video.bActive)
		return;

	if(!Video.bUnlocked)
	{
		Cvar_SetValue(cvFullscreen.name,(float)Video.bFullscreen);
		Cvar_SetValue(cvWidth.name,(float)Video.iWidth);
		Cvar_SetValue(cvHeight.name,(float)Video.iHeight);
		Cvar_SetValue(cvVerticalSync.name,(float)Video.bVerticalSync);

		Video.bUnlocked = true;
		return;
	}

	// Ensure the given width and height are within reasonable bounds.
	if (cvWidth.iValue < WINDOW_MINIMUM_WIDTH ||
		cvHeight.iValue < WINDOW_MINIMUM_HEIGHT)
	{
		Con_Warning("Failed to get an appropriate resolution!\n");

		Cvar_SetValue(cvWidth.name, WINDOW_MINIMUM_WIDTH);
		Cvar_SetValue(cvHeight.name, WINDOW_MINIMUM_HEIGHT);
	}
	// If we're not fullscreen, then constrain our window size to the size of the desktop.
	else if (!Video.bFullscreen && ((cvWidth.iValue > pWindow_GetScreenWidth()) || (cvHeight.iValue > pWindow_GetScreenHeight())))
	{
		Con_Warning("Attempted to set resolution beyond scope of desktop!\n");

		Cvar_SetValue(cvWidth.name, pWindow_GetScreenWidth());
		Cvar_SetValue(cvHeight.name, pWindow_GetScreenHeight());
	}

	Video.iWidth = cvWidth.iValue;
	Video.iHeight = cvHeight.iValue;

	Window_UpdateVideo();

	// Update console size.
	SCR_Conwidth_f();
}

void Video_SetViewportSize(int iWidth, int iHeight)
{
	if (iWidth <= 0)
		iWidth = 1;
	if (iHeight <= 0)
		iHeight = 1;

	Video.iWidth = iWidth;
	Video.iHeight = iHeight;

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
	else if (gTexture->texnum == Video.uiCurrentTexture[Video.uiActiveUnit])
		return;

	Video.uiCurrentTexture[Video.uiActiveUnit] = gTexture->texnum;

	gTexture->visframe = r_framecount;

	// Bind it.
	glBindTexture(GL_TEXTURE_2D,gTexture->texnum);

	if(Video.bDebugFrame)
		pLog_Write(cvVideoDebugLog.string,"Video: Bound texture (%s) (%i)\n",gTexture->name,Video.uiActiveUnit);
}

/*  Changes the active blending mode.
	This should be used in conjunction with the VIDEO_BLEND mode.
*/
void Video_SetBlend(VideoBlend_t voBlendMode, VideoDepth_t vdDepthMode)
{
	if (vdDepthMode != VIDEO_DEPTH_IGNORE)
		glDepthMask(vdDepthMode);

	if(voBlendMode != VIDEO_BLEND_IGNORE)
	{
		switch(voBlendMode)
		{
		case VIDEO_BLEND_ONE:
			glBlendFunc(GL_ONE,GL_ONE);
			break;
		case VIDEO_BLEND_TWO:
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			break;
		case VIDEO_BLEND_THREE:
			glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);
			break;
		case VIDEO_BLEND_FOUR:
			glBlendFunc(GL_ZERO,GL_ZERO);
			break;
		default:
			Sys_Error("Unknown blend mode! (%i)\n",voBlendMode);
		}
	}

	if(Video.bDebugFrame)
		pLog_Write(cvVideoDebugLog.string, "Video: Setting blend mode (%i) (%i)\n", voBlendMode, vdDepthMode);
}

/*
	Multitexturing Management
*/

/*	Conversion between our TMU selection and OpenGL.
*/
unsigned int Video_GetTextureUnit(unsigned int uiTarget)
{
	if (Video.bDebugFrame)
		pLog_Write(cvVideoDebugLog.string, "Video: Attempting to get TMU target %i\n", uiTarget);

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

	if (Video.bDebugFrame)
		pLog_Write(cvVideoDebugLog.string, "Video: Returning TMU %i\n", GL_TEXTURE0 + uiTarget);

	return GL_TEXTURE0 + uiTarget;
}

void Video_SelectTexture(unsigned int uiTarget)
{
	if(uiTarget == Video.uiActiveUnit)
		return;

	if (uiTarget > VIDEO_MAX_UNITS)
		Sys_Error("Invalid texture unit! (%i)\n",uiTarget);

	glActiveTexture(Video_GetTextureUnit(uiTarget));

	Video.uiActiveUnit = uiTarget;

	if(Video.bDebugFrame)
		pLog_Write(cvVideoDebugLog.string, "Video: Texture Unit %i\n", Video.uiActiveUnit);
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

/*	Draw terrain.
	Unfinished
*/
void Video_DrawTerrain(VideoObjectVertex_t *voTerrain)
{
	if(!voTerrain)
		Sys_Error("Invalid video object!\n");
}

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
	VideoObjectVertex_t	*voSurface;
	float			*fVert;
	int				i;

	voSurface = (VideoObjectVertex_t*)Hunk_TempAlloc(mSurface->polys->numverts*sizeof(VideoObjectVertex_t));
	if (!voSurface)
		Sys_Error("Failed to allocate surface video object!\n");

	fVert = mSurface->polys->verts[0];
	for (i = 0; i < mSurface->polys->numverts; i++, fVert += VERTEXSIZE)
	{
#ifdef _MSC_VER
#pragma warning(suppress: 6011)
#endif
		Video_ObjectVertex(&voSurface[i], fVert[0], fVert[1], fVert[2]);
		Video_ObjectTexture(&voSurface[i], VIDEO_TEXTURE_DIFFUSE, fVert[3], fVert[4]);
		Video_ObjectTexture(&voSurface[i], VIDEO_TEXTURE_LIGHT, fVert[5], fVert[6]);
		Video_ObjectColour(&voSurface[i], 1.0f, 1.0f, 1.0f, fAlpha);
	}

	Video_DrawObject(voSurface, VIDEO_PRIMITIVE_TRIANGLE_FAN, mSurface->polys->numverts, mMaterial, 0);
}

/*	Draw 3D object.
	TODO: Add support for VBOs ?
*/
void Video_DrawObject(
	VideoObjectVertex_t *voObject, VideoPrimitive_t vpPrimitiveType, unsigned int uiVerts,
	Material_t *mMaterial, int iSkin)
{
	unsigned int i;

	if (uiVerts <= 0)
		return;

	if (Video.bDebugFrame)
	{
		uiVideoDrawObjectCalls++;
		pLog_Write(cvVideoDebugLog.string, "Video: Drawing object (%i) (%i)\n", uiVerts, vpPrimitiveType);
	}

	bVideoIgnoreCapabilities = true;

	Material_Draw(mMaterial, iSkin, voObject, vpPrimitiveType, uiVerts, false);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(VideoObjectVertex_t), voObject->mvPosition);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, sizeof(VideoObjectVertex_t), voObject->mvColour);

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, sizeof(VideoObjectVertex_t), voObject->mvNormal);

	for (i = 0; i < VIDEO_MAX_UNITS; i++)
		if (Video.bUnitState[i])
		{
			glClientActiveTexture(Video_GetTextureUnit(i));
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, sizeof(VideoObjectVertex_t), voObject->mvST[i]);
		}

	bool bShowWireframe = r_showtris.bValue;
	if (mMaterial && mMaterial->bWireframeOverride)
		bShowWireframe = false;

	VideoLayer_DrawArrays(vpPrimitiveType, uiVerts, bShowWireframe);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	for (i = 0; i < VIDEO_MAX_UNITS; i++)
		if (Video.bUnitState[i])
		{
			glClientActiveTexture(Video_GetTextureUnit(i));
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}

	Material_Draw(mMaterial, iSkin, voObject, vpPrimitiveType, uiVerts, true);

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
			Video.bUnitState[Video.uiActiveUnit] = true;

		if(iCapabilities & vcCapabilityList[i].uiFirst)
		{
			if(Video.bDebugFrame)
				pLog_Write(cvVideoDebugLog.string, "Video: Enabling %s (%i)\n", vcCapabilityList[i].ccIdentifier, Video.uiActiveUnit);

			if(!bVideoIgnoreCapabilities)
				// [24/2/2014] Collect up a list of the new capabilities we set ~hogsy
				iSavedCapabilites[Video.uiActiveUnit][VIDEO_STATE_ENABLE] |= vcCapabilityList[i].uiFirst;

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
			Video.bUnitState[Video.uiActiveUnit] = false;

		if(iCapabilities & vcCapabilityList[i].uiFirst)
		{
			if(Video.bDebugFrame)
				pLog_Write(cvVideoDebugLog.string, "Video: Disabling %s (%i)\n", vcCapabilityList[i].ccIdentifier, Video.uiActiveUnit);

			if(!bVideoIgnoreCapabilities)
				// Collect up a list of the new capabilities we disabled.
				iSavedCapabilites[Video.uiActiveUnit][VIDEO_STATE_DISABLE] |= vcCapabilityList[i].uiFirst;

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

		if (iCapability & iSavedCapabilites[Video.uiActiveUnit][VIDEO_STATE_ENABLE])
			return true;
	}

	return false;
}

/*	Resets our capabilities.
	Give an argument of true to only clear the list, not the capabilities.
	Also resets active blending mode.
*/
void Video_ResetCapabilities(bool bClearActive)
{
	int i;

	if(Video.bDebugFrame)
		pLog_Write(cvVideoDebugLog.string, "Video: Resetting capabilities...\n");

	Video_SelectTexture(VIDEO_TEXTURE_DIFFUSE);

	if(bClearActive)
	{
		if(Video.bDebugFrame)
			pLog_Write(cvVideoDebugLog.string, "Video: Clearing active capabilities...\n");

		bVideoIgnoreCapabilities = true;

		// Set this back too...
		VideoLayer_SetTextureEnvironmentMode(VIDEO_TEXTURE_MODE_MODULATE);

		Video_DisableCapabilities(iSavedCapabilites[VIDEO_TEXTURE_DIFFUSE][VIDEO_STATE_ENABLE]);
		Video_EnableCapabilities(iSavedCapabilites[VIDEO_TEXTURE_DIFFUSE][VIDEO_STATE_DISABLE]);

		Video_SetBlend(VIDEO_BLEND_TWO, VIDEO_DEPTH_TRUE);

		bVideoIgnoreCapabilities = false;

		if(Video.bDebugFrame)
			pLog_Write(cvVideoDebugLog.string, "Video: Finished clearing capabilities.\n");
	}

	// Clear out capability list.
	for(i = 0; i < VIDEO_MAX_UNITS; i++)
		iSavedCapabilites[i][0] =
		iSavedCapabilites[i][1] = 0;
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
	if (Global.bEmbeddedContext || (Video.bInitialized == false))
		return;

	if (Video.bDebugFrame)
		pLog_Write(cvVideoDebugLog.string, "Video: Start of frame\n");

#ifdef VIDEO_SUPPORT_SHADERS
#ifdef VIDEO_SUPPORT_FRAMEBUFFERS
	VideoPostProcess_BindFrameBuffer();
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

#ifdef VIDEO_SUPPORT_SHADERS
#ifdef VIDEO_SUPPORT_FRAMEBUFFERS
	VideoPostProcess_Draw();
#endif
#endif

	Screen_DrawFPS();

	if (cvVideoFinish.bValue)
		glFinish();

	if (Video.bDebugFrame)
		Video.bDebugFrame = false;
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

	if (!Global.bEmbeddedContext)
		Window_Shutdown();

	// Set the initialisation value to false, in-case we want to try again later.
	Video.bInitialized = false;
}
