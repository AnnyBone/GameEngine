/*	Copyright (C) 2011-2015 OldTimes Software
*/
#include "quakedef.h"

/*
	Video System

	This is our future renderer code, I imagine most of this
	will be fairly incomplete before OpenKatana's release but the
	base should at least be finished by then just so it's stable...
	If not then do give me some serious scolding.

	TODO:
		- Move all/most API-specific code here.
*/

// Main header
#include "engine_video.h"

#include "engine_modgame.h"
#include "engine_console.h"

#include <SDL_syswm.h>

SDL_Window		*sMainWindow;
SDL_GLContext	sMainContext;

static unsigned int	iSavedCapabilites[VIDEO_MAX_UNITS][2];

#define VIDEO_STATE_ENABLE   0
#define VIDEO_STATE_DISABLE  1

cvar_t	cvMultisampleSamples		= {	"video_multisamplesamples",		"0",			true,   false,  "Changes the number of samples."									},
		cvMultisampleMaxSamples		= { "video_multisamplemaxsamples",	"16",			true,	false,	"Sets the maximum number of allowed samples."						},
		cvFullscreen				= {	"video_fullscreen",				"0",			true,   false,  "1: Fullscreen, 0: Windowed"										},
		cvWidth						= {	"video_width",					"640",			true,   false,  "Sets the width of the window."										},
		cvHeight					= {	"video_height",					"480",			true,   false,  "Sets the height of the window."									},
		cvVerticalSync				= {	"video_verticalsync",			"0",			true																				},
		cvVideoMirror				= {	"video_drawmirror",				"1",			true,	false,	"Enables and disables the rendering of mirror surfaces."			},
		cvVideoDraw					= {	"video_draw",					"1",			false,	false,	"If disabled, nothing is drawn."									},
		cvVideoDrawModels			= {	"video_drawmodels",				"1",			false,  false,  "Toggles models."													},
		cvVideoDrawDepth			= {	"video_drawdepth",				"0",			false,	false,	"If enabled, previews the debth buffer."							},
		cvVideoDrawDetail			= {	"video_drawdetail",				"1",			true,	false,	"If enabled, detail maps are drawn."								},
		cvVideoDrawMaterials		= {	"video_drawmaterials",			"1",			true,	false,	"If enabled, materials are drawn."									},
		cvVideoDetailScale			= { "video_detailscale",			"4",			true,	false,	"Changes the scaling used for detail maps."							},
		cvVideoAlphaTrick			= { "video_alphatrick",				"1",			true,	false,	"If enabled, draws alpha-tested surfaces twice for extra quality."	},
		cvVideoFinish				= { "video_finish",					"0",			true,	false,	"If enabled, calls glFinish at the end of the frame."				},
		cvVideoVBO					= { "video_vbo",					"0",			true,	false,	"Enables support of Vertex Buffer Objects."							},
		cvVideoPlayerShadow			= { "video_playershadow",			"1",			true,	false,	"If enabled, the players own shadow will be drawn."					},
		cvVideoDebugLog				= {	"video_debuglog",				"log_video",	true,	false,	"The name of the output log for video debugging."					};

#define VIDEO_MIN_WIDTH		640
#define VIDEO_MIN_HEIGHT	480
#define VIDEO_MAX_SAMPLES	cvMultisampleMaxSamples.iValue
#define VIDEO_MIN_SAMPLES	0

gltexture_t	*gDepthTexture;

bool	bVideoIgnoreCapabilities	= false,
		bVideoDebug					= false;

MathVector2_t	**vVideoTextureArray;
MathVector3_t	*vVideoVertexArray;
MathVector4_t	*vVideoColourArray;

unsigned int	uiVideoArraySize = 32768;

void Video_DebugCommand(void);
void Video_AllocateArrays(int iSize);

SDL_DisplayMode	sDisplayMode;

/*	Initialize the renderer
*/
void Video_Initialize(void)
{
	int i;

	if(Video.bInitialized)
		return;

	Con_Printf("Initializing video...\n");

	// [23/7/2013] Set default values ~hogsy
	Video.iCurrentTexture		= (unsigned int)-1;	// [29/8/2012] "To avoid unnecessary texture sets" ~hogsy
	Video.bVertexBufferObject	= false;
	Video.bActive				=			// Window is intially assumed active.
	Video.bUnlocked				= true;		// Video mode is initially locked.

	// All units are initially disabled.
	for (i = 0; i < VIDEO_MAX_UNITS; i++)
		Video.bUnitState[i] = false;

	Video_AllocateArrays(uiVideoArraySize);
	
	Cvar_RegisterVariable(&cvMultisampleSamples,NULL);
	Cvar_RegisterVariable(&cvVideoDrawModels,NULL);
	Cvar_RegisterVariable(&cvFullscreen,NULL);
	Cvar_RegisterVariable(&cvWidth,NULL);
	Cvar_RegisterVariable(&cvHeight,NULL);
	Cvar_RegisterVariable(&cvVerticalSync,NULL);
	Cvar_RegisterVariable(&cvVideoDebugLog,NULL);
	Cvar_RegisterVariable(&cvVideoDraw,NULL);
	Cvar_RegisterVariable(&cvVideoDrawDepth,NULL);
	Cvar_RegisterVariable(&cvVideoFinish, NULL);
	Cvar_RegisterVariable(&cvVideoAlphaTrick, NULL);
	Cvar_RegisterVariable(&cvVideoMirror, NULL);
	Cvar_RegisterVariable(&cvVideoDrawMaterials, NULL);
	Cvar_RegisterVariable(&cvVideoDrawDetail, NULL);
	Cvar_RegisterVariable(&cvVideoDetailScale, NULL);
	Cvar_RegisterVariable(&cvVideoPlayerShadow, NULL);

	Cmd_AddCommand("video_restart",Video_UpdateWindow);
	Cmd_AddCommand("video_debug",Video_DebugCommand);

	// [28/7/2013] Moved check here and corrected, seems more secure ~hogsy
	if(SDL_VideoInit(NULL) < 0)
		Sys_Error("Failed to initialize video!\n%s\n",SDL_GetError());

	SDL_DisableScreenSaver();

	// Get display information.
	if (SDL_GetCurrentDisplayMode(0, &sDisplayMode) != 0)
		Sys_Error("Failed to get current display information!\n%s\n", SDL_GetError());

	Video.bInitialized = true;

	// [9/7/2013] TEMP: Should honestly be called from the launcher (in a perfect world) ~hogsy
	Video_CreateWindow();

	if (!SDL_GetWindowWMInfo(sMainWindow, &Video.sSystemInfo))
		Sys_Error("Failed to get WM information!\n");
}

/*
	Video Commands
*/

void Video_DebugCommand(void)
{
	if(!bVideoDebug)
		bVideoDebug = true;

	Console_ClearLog(cvVideoDebugLog.string);
}

/**/

/*	Clears the color and depth buffers.
*/
void Video_ClearBuffer(void)
{
	int	iClear =
		GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT;

	// [5/9/2013] Handle the stencil buffer too ~hogsy
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
	if(!cvVideoDrawDepth.bValue)
		return;

	// Create our depth texture.
	if(!gDepthTexture)
		gDepthTexture = TexMgr_NewTexture();

	GL_SetCanvas(CANVAS_BOTTOMLEFT);

	Video_SetTexture(gDepthTexture);

	glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT32,Video.iWidth,Video.iHeight,0,GL_DEPTH_COMPONENT32,GL_UNSIGNED_BYTE,0);

	Draw_Fill(0,0,512,512,1.0f,1.0f,1.0f,1.0f);
}

/*
	Window Management
*/

/*	Set the gamma level.
	Based on Darkplaces implementation.
*/
void Video_SetGamma(unsigned short *usRamp,int iRampSize)
{
	if(!SDL_SetWindowGammaRamp(sMainWindow,usRamp,usRamp+iRampSize,usRamp+iRampSize*2))
		Con_Warning("Failed to set gamma level!\n");
}

/*	Get gamma level.
	Based on the Darkplaces implementation.
*/
void Video_GetGamma(unsigned short *usRamp,int iRampSize)
{
	if(!SDL_GetWindowGammaRamp(sMainWindow,usRamp,usRamp+iRampSize,usRamp+iRampSize*2))
		Con_Warning("Failed to get gamma level!\n");
}

/*	Get the current displays width.
*/
unsigned int Video_GetDesktopWidth(void)
{
	return sDisplayMode.w;
}

/*	Get the current displays height.
*/
unsigned int Video_GetDesktopHeight(void)
{
	return sDisplayMode.h;
}

/*	Create our window.
*/
void Video_CreateWindow(void)
{
	int			iFlags =
		SDL_WINDOW_SHOWN		|
		SDL_WINDOW_OPENGL		|
		SDL_WINDOW_FULLSCREEN,
				iSupportedUnits;
	SDL_Surface	*sIcon;

	if(!Video.bInitialized)
		Sys_Error("Attempted to create window before video initialization!\n");

	// [15/8/2012] Figure out what resolution we're going to use ~hogsy
	if(COM_CheckParm("-window"))
	{
		Video.bFullscreen	=
		Video.bUnlocked		= false;
	}
	else
		// [15/8/2012] Otherwise set us as fullscreen ~hogsy
		Video.bFullscreen = cvFullscreen.bValue;

	if(COM_CheckParm("-width"))
	{
		Video.iWidth	= atoi(com_argv[COM_CheckParm("-width")+1]);
		Video.bUnlocked	= false;
	}
	else
		Video.iWidth = cvWidth.iValue;

	if(COM_CheckParm("-height"))
	{
		Video.iHeight	= atoi(com_argv[COM_CheckParm("-height")+1]);
		Video.bUnlocked	= false;
	}
	else
		Video.iHeight = cvHeight.iValue;

	if(!Video.bFullscreen)
		iFlags &= ~SDL_WINDOW_FULLSCREEN;

#if 0
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,2);
#endif
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);

	// Ensure the given width and height are within reasonable bounds.
	if(	Video.iWidth	< VIDEO_MIN_WIDTH	||
		Video.iHeight	< VIDEO_MIN_HEIGHT)
	{
		Con_Warning("Failed to get an appropriate resolution!\n");

		Video.iWidth	= VIDEO_MIN_WIDTH;
		Video.iHeight	= VIDEO_MIN_HEIGHT;
	}
	// If we're not fullscreen, then constrain our window size to the size of the desktop.
	else if (!Video.bFullscreen && ((Video.iWidth > Video_GetDesktopWidth()) || (Video.iHeight > Video_GetDesktopHeight())))
	{
		Con_Warning("Attempted to set resolution beyond scope of desktop!\n");

		Video.iWidth = Video_GetDesktopWidth();
		Video.iHeight = Video_GetDesktopHeight();
	}

	sMainWindow = SDL_CreateWindow(
		Game->Name,				// [9/7/2013] Window name is based on the name given by Game ~hogsy
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		Video.iWidth,
		Video.iHeight,
		iFlags);
	if(!sMainWindow)
		Sys_Error("Failed to create window!\n%s\n",SDL_GetError());
	
	// [6/2/2014] Set the icon for the window ~hogsy
	// [25/3/2014] Grab the icon from our game directory ~hogsy
	sIcon = SDL_LoadBMP(va("%s/icon.bmp",com_gamedir));
	if(sIcon)
	{
        // [25/3/2014] Set the transparency key... ~hogsy
        SDL_SetColorKey(sIcon,true,SDL_MapRGB(sIcon->format,0,0,0));
		SDL_SetWindowIcon(sMainWindow,sIcon);
		SDL_FreeSurface(sIcon);
	}
	else
		Con_Warning("Failed to load window icon! (%s)\n",SDL_GetError());

	sMainContext = SDL_GL_CreateContext(sMainWindow);
	if(!sMainContext)
		Sys_Error("Failed to create context!\n%s\n",SDL_GetError());

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&iSupportedUnits);
	if(iSupportedUnits < VIDEO_MAX_UNITS)
		Sys_Error("Your system doesn't support the required number of TMUs! (%i)",iSupportedUnits);

	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &Video.fMaxAnisotropy);

	SDL_GL_SetSwapInterval(0);

	// Get any information that will be presented later.
	Video.ccGLVendor		= (char*)glGetString(GL_VENDOR);
	Video.ccGLRenderer		= (char*)glGetString(GL_RENDERER);
	Video.ccGLVersion		= (char*)glGetString(GL_VERSION);
	Video.ccGLExtensions	= (char*)glGetString(GL_EXTENSIONS);

	// [3/6/2013] Added to fix a bug on some systems when calling wglGetExtensionString* ~hogsy
	GLeeInit();

	Con_DPrintf(" Checking for extensions...\n");

	// Check that the required capabilities are supported.
	if(!GLEE_ARB_multitexture)
		Sys_Error("Video hardware incapable of multi-texturing!\n");
	else if (!GLEE_ARB_texture_env_combine && !GLEE_EXT_texture_env_combine)
		Sys_Error("ARB/EXT_texture_env_combine isn't supported by your hardware!\n");
	else if (!GLEE_ARB_texture_env_add && !GLEE_EXT_texture_env_add)
		Sys_Error("ARB/EXT_texture_env_add isn't supported by your hardware!\n");
	else if (!GLEE_EXT_fog_coord)
		Sys_Error("EXT_fog_coord isn't supported by your hardware!\n");

	if (GLEE_ARB_vertex_buffer_object)
		Video.bVertexBufferObject = true;
	else
		Con_Warning("ARB_vertex_buffer_object isn't supported by your hardware!\n");

#ifdef KATANA_VIDEO_NEXT
	if(!GLEE_ARB_vertex_program || !GLEE_ARB_fragment_program)
		Sys_Error("Unsupported video hardware!\n");
#endif

	// Set the default states...

	Video_EnableCapabilities(VIDEO_TEXTURE_2D);

	Video_SetBlend(VIDEO_BLEND_TWO, VIDEO_DEPTH_IGNORE);

	glClearColor(0,0,0,0);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glAlphaFunc(GL_GREATER,0.5f);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	glDepthRange(0,1);
	glDepthFunc(GL_LEQUAL);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	Video_SelectTexture(VIDEO_TEXTURE_LIGHT);

	// Overbrights
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 4);

	Video_SelectTexture(0);

	vid.conwidth		= (scr_conwidth.value > 0)?(int)scr_conwidth.value:(scr_conscale.value > 0)?(int)(Video.iWidth/scr_conscale.value) : Video.iWidth;
	vid.conwidth		= Math_Clamp(320,vid.conwidth,Video.iWidth);
	vid.conwidth		&= 0xFFFFFFF8;
	vid.conheight		= vid.conwidth*Video.iHeight/Video.iWidth;
	Video.bVerticalSync	= cvVerticalSync.bValue;
}

void Video_UpdateWindow(void)
{
	if(!Video.bActive)
		return;
	else if(!Video.bUnlocked)
	{
		Cvar_SetValue(cvFullscreen.name,(float)Video.bFullscreen);
		Cvar_SetValue(cvWidth.name,(float)Video.iWidth);
		Cvar_SetValue(cvHeight.name,(float)Video.iHeight);
		Cvar_SetValue(cvVerticalSync.name,(float)Video.bVerticalSync);

		Video.bUnlocked = true;
		return;
	}

	Video.iWidth	= cvWidth.iValue;
	Video.iHeight	= cvHeight.iValue;

	SDL_SetWindowSize(sMainWindow,Video.iWidth,Video.iHeight);

	if(Video.bVerticalSync != cvVerticalSync.bValue)
	{
		SDL_GL_SetSwapInterval(cvVerticalSync.iValue);

		Video.bVerticalSync = cvVerticalSync.bValue;
	}

	// [16/7/2013] There's gotta be a cleaner way of doing this... Ugh ~hogsy
	if(Video.bFullscreen != cvFullscreen.bValue)
	{
		if(SDL_SetWindowFullscreen(sMainWindow,(SDL_bool)cvFullscreen.bValue) < 0)
		{
			Con_Warning("Failed to set window mode!\n%s",SDL_GetError());

			// [16/7/2013] Reset the variable to the current value ~hogsy
			Cvar_SetValue(cvFullscreen.name,(float)Video.bFullscreen);
		}
		else
			Video.bFullscreen = cvFullscreen.bValue;
	}

#if 0
	if (Video.uiMSAASamples != cvMultisampleSamples.iValue)
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, cvMultisampleSamples.iValue);

		Video.uiMSAASamples = cvMultisampleSamples.iValue;
	}
#endif

	if(!cvFullscreen.value)
		// [15/7/2013] Center the window ~hogsy
		SDL_SetWindowPosition(sMainWindow,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED);

	// [15/7/2013] Update console size ~hogsy
	vid.conwidth	= Video.iWidth & 0xFFFFFFF8;
	vid.conheight	= vid.conwidth*Video.iHeight/Video.iWidth;
}

/*
	Coordinate Generation
*/

void Video_GenerateSphereCoordinates(void)
{
	// OpenGL makes this pretty easy for us (though this should probably be more abstract)...
	glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
	glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
}

/**/

/*	Bind our current texture.
*/
void Video_SetTexture(gltexture_t *gTexture)
{
	if(!gTexture)
		gTexture = notexture;
	// [29/8/2012] Same as the last binded texture? ~hogsy
	else if(gTexture->texnum == Video.iCurrentTexture)
		return;

	Video.iCurrentTexture = gTexture->texnum;

	gTexture->visframe = r_framecount;

	glBindTexture(GL_TEXTURE_2D,gTexture->texnum);

	if(bVideoDebug)
		Console_WriteToLog(cvVideoDebugLog.string,"Video: Bound texture (%s) (%i)\n",gTexture->name,Video.uiActiveUnit);
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

	if(bVideoDebug)
		Console_WriteToLog(cvVideoDebugLog.string, "Video: Setting blend mode (%i) (%i)\n", voBlendMode, vdDepthMode);
}

/*
    Multitexturing Management
*/

/*	Conversion between our TMU selection and OpenGL.
*/
unsigned int Video_GetGLUnit(unsigned int uiTarget)
{
	unsigned int uiUnit = 0;

	if (bVideoDebug)
		Console_WriteToLog(cvVideoDebugLog.string, "Video: Attempting to get TMU target %i\n", uiTarget);

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

	return uiUnit;
}

void Video_SelectTexture(unsigned int uiTarget)
{
	if(uiTarget == Video.uiActiveUnit)
        return;

	if (uiTarget > VIDEO_TEXTURE_MAX)
		Sys_Error("Invalid texture unit! (%i)\n",uiTarget);

	glActiveTexture(Video_GetGLUnit(uiTarget));

	Video.uiActiveUnit = uiTarget;

	if(bVideoDebug)
		Console_WriteToLog(cvVideoDebugLog.string,"Video: Texture Unit %i\n",Video.uiActiveUnit);
}

/*
	Object Management
*/

MathVector4_t	mvVideoGlobalColour;

void Video_ObjectTexture(VideoObject_t *voObject, unsigned int uiTextureUnit, float S, float T)
{
	voObject->vTextureCoord[uiTextureUnit][0] = S;
	voObject->vTextureCoord[uiTextureUnit][1] = T;
}

void Video_ObjectVertex(VideoObject_t *voObject, float X, float Y, float Z)
{
	voObject->vVertex[0] = X;
	voObject->vVertex[1] = Y;
	voObject->vVertex[2] = Z;
}

void Video_ObjectNormal(VideoObject_t *voObject, float X, float Y, float Z)
{
	voObject->vNormal[0] = X;
	voObject->vNormal[1] = Y;
	voObject->vNormal[2] = Z;
}

void Video_ObjectColour(VideoObject_t *voObject, float R,float G,float B,float A)
{
	voObject->vColour[pRED] = R;
	voObject->vColour[pGREEN] = G;
	voObject->vColour[pBLUE] = B;
	voObject->vColour[pALPHA] = A;
}

/*	Used to override any colour given to a video object.
	Cleared by Video_ResetCapabilities.
*/
void Video_SetColour(float R,float G,float B,float A)
{
	mvVideoGlobalColour[pRED] = R;
	mvVideoGlobalColour[pGREEN] = G;
	mvVideoGlobalColour[pBLUE] = B;
	mvVideoGlobalColour[pALPHA] = A;

	Video.bColourOverride = true;
}

/*
    Drawing
*/

/*	Reallocates video arrays.
*/
void Video_AllocateArrays(int iSize)
{
	int i;

	if (bVideoDebug)
		Console_WriteToLog(cvVideoDebugLog.string, "Video: Allocating arrays...\n");

	// Check that each of these have been initialized before freeing them.
	if (vVideoVertexArray)
		free(vVideoVertexArray);
	if (vVideoColourArray)
		free(vVideoColourArray);
	if (vVideoTextureArray)
		free(vVideoTextureArray);

	vVideoTextureArray = (MathVector2_t**)Hunk_AllocName(VIDEO_MAX_UNITS*sizeof(MathVector2_t), "video_texturearray");
	for (i = 0; i < VIDEO_MAX_UNITS; i++)
		vVideoTextureArray[i] = (MathVector2_t*)Hunk_Alloc(iSize*sizeof(MathVector2_t));

	vVideoVertexArray = (MathVector3_t*)Hunk_AllocName(iSize*sizeof(MathVector3_t), "video_vertexarray");
	vVideoColourArray = (MathVector4_t*)Hunk_AllocName(iSize*sizeof(MathVector4_t), "video_colourarray");

	if(!vVideoColourArray || !vVideoTextureArray || !vVideoVertexArray)
		Sys_Error("Failed to allocate video arrays!\n");
	
	// Keep this up to date.
	uiVideoArraySize = iSize;
}

/*	Draw terrain.
	Unfinished
*/
void Video_DrawTerrain(VideoObject_t *voTerrain)
{
	if(!voTerrain)
		Sys_Error("Invalid video object!\n");
}

extern cvar_t gl_fullbrights;

/*	Called before the object is drawn.
*/
void Video_DrawMaterial(
	Material_t *mMaterial, int iSkin,
	VideoObject_t *voObject, VideoPrimitive_t vpPrimitiveType, unsigned int uiSize,
	bool bPost)
{
	unsigned int	i;
	MaterialSkin_t	*msCurrentSkin;

	// If we're drawing flat, then don't apply textures.
	if (r_lightmap_cheatsafe || r_drawflat_cheatsafe || r_showtris.bValue || !cvVideoDrawMaterials.bValue)
		return;

	if (mMaterial->iFlags & MATERIAL_FLAG_ANIMATED)
		msCurrentSkin = Material_GetAnimatedSkin(mMaterial);
	else
		msCurrentSkin = Material_GetSkin(mMaterial, iSkin);
	if (!msCurrentSkin)
		Sys_Error("Failed to get valid skin!\n");

	for (i = 0; i < msCurrentSkin->uiTextures; i++)
	{
		Video_SelectTexture(0);

		// Check our specific type.
		switch (msCurrentSkin->mtTexture[i].mttType)
		{
		case MATERIAL_TEXTURE_DIFFUSE:
			// Select the given texture.
			Video_SelectTexture(VIDEO_TEXTURE_DIFFUSE);

			if (!bPost)
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

			if (msCurrentSkin->uiFlags & MATERIAL_FLAG_ALPHA)
			{
				if (!bPost)
					glEnable(GL_ALPHA_TEST);
				else
				{
					glDisable(GL_ALPHA_TEST);

					if (cvVideoAlphaTrick.bValue && (uiSize > 0))
					{
						Video_SetBlend(VIDEO_BLEND_IGNORE, VIDEO_DEPTH_FALSE);

						glEnable(GL_BLEND);

						// Draw the object again (don't bother passing material).
						Video_DrawObject(voObject, vpPrimitiveType, uiSize, NULL, 0);

						glDisable(GL_BLEND);

						Video_SetBlend(VIDEO_BLEND_IGNORE, VIDEO_DEPTH_TRUE);
					}
				}
			}
			else if (msCurrentSkin->uiFlags & MATERIAL_FLAG_BLEND)
			{
				if (!bPost)
					glEnable(GL_BLEND);
				else
					glDisable(GL_BLEND);
			}
			break;
		case MATERIAL_TEXTURE_DETAIL:
			if (!cvVideoDrawDetail.bValue)
				break;

			// Select the given texture.
			Video_SelectTexture(VIDEO_TEXTURE_DETAIL);

			if (!bPost)
			{
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
				glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 2);

				// Check if we've been given a video object to use...
				if (voObject)
				{
					unsigned int j;

					// Go through the whole object.
					for (j = 0; j < uiSize; j++)
					{
						// Copy over original texture coords.
						Video_ObjectTexture(&voObject[j], VIDEO_TEXTURE_DETAIL,
							// Use base texture coordinates as a reference.
							voObject[j].vTextureCoord[0][0] * cvVideoDetailScale.value,
							voObject[j].vTextureCoord[0][1] * cvVideoDetailScale.value);

						// TODO: Modify them to the appropriate scale.

					}
				}
			}
			break;
		case MATERIAL_TEXTURE_FULLBRIGHT:
			if (!gl_fullbrights.bValue)
				break;

			// Select the given texture.
			Video_SelectTexture(VIDEO_TEXTURE_FULLBRIGHT);

			if (!bPost)
			{
				Video_EnableCapabilities(VIDEO_BLEND);

				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);

				if (voObject)
				{
					unsigned int j;

					for (j = 0; j < uiSize; j++)
					{
						// Texture coordinates remain the same for fullbright layers.
						Video_ObjectTexture(&voObject[j], VIDEO_TEXTURE_FULLBRIGHT,
							// Use base texture coordinates as a reference.
							voObject[j].vTextureCoord[0][0],
							voObject[j].vTextureCoord[0][1]);
					}
				}
			}
			else
				Video_DisableCapabilities(VIDEO_BLEND);
			break;
		case MATERIAL_TEXTURE_SPHERE:
			// Select the given texture.
			Video_SelectTexture(VIDEO_TEXTURE_SPHERE);

			if (!bPost)
			{
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

				Video_GenerateSphereCoordinates();
				Video_EnableCapabilities(VIDEO_BLEND | VIDEO_TEXTURE_GEN_S | VIDEO_TEXTURE_GEN_T);
			}
			else
				Video_DisableCapabilities(VIDEO_BLEND | VIDEO_TEXTURE_GEN_S | VIDEO_TEXTURE_GEN_T);
			break;
		default:
			Sys_Error("Invalid texture type for material! (%s) (%i)\n", mMaterial->cPath, msCurrentSkin->mtTexture[i].mttType);
		}

		if (!bPost)
		{
			// Enable it.
			Video_EnableCapabilities(VIDEO_TEXTURE_2D);

			// Bind it.
			Video_SetTexture(msCurrentSkin->mtTexture[i].gMap);

			// Allow us to manipulate the texture.
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			if ((msCurrentSkin->mtTexture[i].vScroll[0] > 0) || (msCurrentSkin->mtTexture[i].vScroll[0] < 0) ||
				(msCurrentSkin->mtTexture[i].vScroll[1] > 0) || (msCurrentSkin->mtTexture[i].vScroll[1] < 0))
				glTranslatef(
				msCurrentSkin->mtTexture[i].vScroll[0] * cl.time,
				msCurrentSkin->mtTexture[i].vScroll[1] * cl.time,
				0);
			if ((msCurrentSkin->mtTexture[i].fRotate > 0) || (msCurrentSkin->mtTexture[i].fRotate < 0))
				glRotatef(msCurrentSkin->mtTexture[i].fRotate*cl.time, 0, 0, 1.0f);
			glMatrixMode(GL_MODELVIEW);
		}
		else
		{
			// Reset any manipulation within the matrix.
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glTranslatef(0, 0, 0);
			glRotatef(0, 0, 0, 0);
			glMatrixMode(GL_MODELVIEW);

			// Reset texture env changes...
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 1);

			// Disable the texture.
			Video_DisableCapabilities(VIDEO_TEXTURE_2D);
		}
	}
}

/*  Draw a simple rectangle.
*/
void Video_DrawFill(VideoObject_t *voFill,Material_t *mMaterial)
{
	Video_DrawObject(voFill,VIDEO_PRIMITIVE_TRIANGLE_FAN,4,mMaterial,0);
}

/*	Surfaces
*/
void Video_DrawSurface(msurface_t *mSurface,float fAlpha, Material_t *mMaterial, unsigned int uiSkin)
{
	VideoObject_t	*voSurface;
	float			*fVert;
	int				i;

	voSurface = (VideoObject_t*)Hunk_TempAlloc(mSurface->polys->numverts*sizeof(VideoObject_t));
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
	VideoObject_t *voObject, VideoPrimitive_t vpPrimitiveType, unsigned int	uiVerts,
	Material_t *mMaterial, int iSkin)
{
	unsigned int	i, j;
	GLenum			gPrimitive = 0;

	if (!cvVideoDraw.bValue)
		return;
	else if (!voObject)
	{
		Sys_Error("Invalid video object!\n");
		return;
	}
	else if (!uiVerts)
	{
		Sys_Error("Invalid number of vertices for video object! (%i)\n", uiVerts);
		return;
	}

	if (bVideoDebug)
		Console_WriteToLog(cvVideoDebugLog.string, "Video: Drawing object (%i) (%i)\n", uiVerts, vpPrimitiveType);

	if (mMaterial)
		Video_DrawMaterial(mMaterial, iSkin, voObject, vpPrimitiveType, uiVerts, false);

	bVideoIgnoreCapabilities = true;

	// Vertices count is too high for this object, bump up array sizes to manage it.
	if (uiVerts > uiVideoArraySize)
		// Double the array size to cope.
		Video_AllocateArrays(uiVerts * 2);

	// Copy everything over...
	for (i = 0; i < uiVerts; i++)
	{
		if (!r_showtris.value)
		{
			// Allow us to override the colour if we want/need to.
			if (Video.bColourOverride)
				Math_Vector4Copy(mvVideoGlobalColour, vVideoColourArray[i]);
			else
				Math_Vector4Copy(voObject[i].vColour, vVideoColourArray[i]);

			//Math_Vector2Copy(voObject[i].vTextureCoord[VIDEO_TEXTURE_DIFFUSE], vVideoTextureArray[VIDEO_TEXTURE_DIFFUSE][i]);

			// Copy over coords for each active TMU.
			for (j = 0; j < VIDEO_MAX_UNITS; j++)
				if (iSavedCapabilites[j][VIDEO_STATE_ENABLE] & VIDEO_TEXTURE_2D)
					Math_Vector2Copy(voObject[i].vTextureCoord[j], vVideoTextureArray[j][i]);
		}
		else
			Math_Vector4Set(1.0f, vVideoColourArray[i]);

		Math_VectorCopy(voObject[i].vVertex, vVideoVertexArray[i]);
	}

	// Handle different primitive types...
	switch (vpPrimitiveType)
	{
	case VIDEO_PRIMITIVE_TRIANGLES:
		if (r_showtris.bValue)
			gPrimitive = GL_LINES;
		else
			gPrimitive = GL_TRIANGLES;
		break;
	case VIDEO_PRIMITIVE_TRIANGLE_FAN:
		if (r_showtris.bValue)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		gPrimitive = GL_TRIANGLE_FAN;
		break;
	default:
		// [16/3/2014] Anything else and give us an error ~hogsy
		Sys_Error("Unknown object primitive type! (%i)\n", vpPrimitiveType);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vVideoVertexArray);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, vVideoColourArray);

	if (!r_showtris.bValue)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		//glClientActiveTexture(Video_GetGLUnit(VIDEO_TEXTURE_DIFFUSE));
		//glTexCoordPointer(2, GL_FLOAT, 0, vVideoTextureArray[VIDEO_TEXTURE_DIFFUSE]);

		for (i = 0; i < VIDEO_MAX_UNITS; i++)
			if (Video.bUnitState[i])
			{
				glClientActiveTexture(Video_GetGLUnit(i));
				glTexCoordPointer(2, GL_FLOAT, 0, vVideoTextureArray[i]);
			}
	}

	glDrawArrays(gPrimitive,0,uiVerts);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	if (!r_showtris.bValue)
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if(r_showtris.bValue)
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	if (mMaterial)
		Video_DrawMaterial(mMaterial, iSkin, voObject, vpPrimitiveType, uiVerts, true);

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
	{	VIDEO_ALPHA_TEST,		GL_ALPHA_TEST,      "ALPHA_TEST"    },
	{	VIDEO_BLEND,			GL_BLEND,           "BLEND"			},
	{	VIDEO_DEPTH_TEST,		GL_DEPTH_TEST,      "DEPTH_TEST"	},
	{	VIDEO_TEXTURE_2D,		GL_TEXTURE_2D,      "TEXTURE_2D"	},
	{	VIDEO_TEXTURE_GEN_S,	GL_TEXTURE_GEN_S,   "TEXTURE_GEN_S"	},
	{	VIDEO_TEXTURE_GEN_T,	GL_TEXTURE_GEN_T,   "TEXTURE_GEN_T"	},
	{	VIDEO_CULL_FACE,		GL_CULL_FACE,       "CULL_FACE"		},
	{	VIDEO_STENCIL_TEST,		GL_STENCIL_TEST,    "STENCIL_TEST"	},
	{	VIDEO_NORMALIZE,		GL_NORMALIZE,		"NORMALIZE"     },

	{   0   }
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
            if(bVideoDebug)
				Console_WriteToLog(cvVideoDebugLog.string, "Video: Enabling %s (%i)\n", vcCapabilityList[i].ccIdentifier, Video.uiActiveUnit);

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
            if(bVideoDebug)
				Console_WriteToLog(cvVideoDebugLog.string, "Video: Disabling %s (%i)\n", vcCapabilityList[i].ccIdentifier, Video.uiActiveUnit);

            if(!bVideoIgnoreCapabilities)
                // [24/2/2014] Collect up a list of the new capabilities we disabled ~hogsy
                iSavedCapabilites[Video.uiActiveUnit][VIDEO_STATE_DISABLE] |= vcCapabilityList[i].uiFirst;

			glDisable(vcCapabilityList[i].uiSecond);
		}
    }
}

/*	Resets our capabilities.
	Give an argument of true to only clear the list, not the capabilities.
	Also resets active blending mode.
*/
void Video_ResetCapabilities(bool bClearActive)
{
	int i;

    if(bVideoDebug)
        Console_WriteToLog(cvVideoDebugLog.string,"Video: Resetting capabilities...\n");

	Video_SelectTexture(VIDEO_TEXTURE_DIFFUSE);

	if(bClearActive)
	{
        if(bVideoDebug)
            Console_WriteToLog(cvVideoDebugLog.string,"Video: Clearing active capabilities...\n");

		bVideoIgnoreCapabilities = true;

		// Set this back too...
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		Video_DisableCapabilities(iSavedCapabilites[VIDEO_TEXTURE_DIFFUSE][VIDEO_STATE_ENABLE]);
		Video_EnableCapabilities(iSavedCapabilites[VIDEO_TEXTURE_DIFFUSE][VIDEO_STATE_DISABLE]);

		Video_SetBlend(VIDEO_BLEND_TWO, VIDEO_DEPTH_TRUE);

		if (Video.bColourOverride)
			Video.bColourOverride = false;

		bVideoIgnoreCapabilities = false;

		if(bVideoDebug)
            Console_WriteToLog(cvVideoDebugLog.string,"Video: Finished clearing capabilities.\n");
	}

	// [7/5/2014] Clear out capability list ~hogsy
	for(i = 0; i < VIDEO_MAX_UNITS; i++)
		iSavedCapabilites[i][0] =
		iSavedCapabilites[i][1] = 0;
}

/*
    Shader Management
    Unfinished!
*/

/*  Simple whipped up function to demo shader processing
    then simple error management. Replace this...
*/
void Video_ProcessShader(int iType)
{
    int             iState;
    unsigned int    uiShader;

    if(iType == VIDEO_SHADER_FRAGMENT)
        uiShader = glCreateShader(GL_VERTEX_SHADER);
    else
        uiShader = glCreateShader(GL_FRAGMENT_SHADER);

//    glShaderSource(uiShader,1,&uiShader,NULL);
    glCompileShader(uiShader);

    glGetShaderiv(uiShader,GL_COMPILE_STATUS,&iState);
    if(!iState)
    {
        char cLog[512];

        glGetShaderInfoLog(uiShader,512,NULL,cLog);

        // [12/3/2014] Spit a log out to the console ~hogsy
        Con_Warning("Failed to compile shader! (%s)\n",cLog);
        return;
    }

//    glShaderSource()
}

/**/

/*	Main rendering loop.
	Unfinished
*/
void Video_Frame(void)
{
    if(bVideoDebug)
        Console_WriteToLog(cvVideoDebugLog.string,"Video: Start of frame\n");

	SCR_UpdateScreen();

	Video_DrawDepthBuffer();

	GL_EndRendering();

	if (cvVideoFinish.bValue)
		glFinish();

    if(bVideoDebug)
    {
        Console_WriteToLog(cvVideoDebugLog.string,"Video: End of frame\n");

		// TODO: End of frame states (number of calls to video_draw etc)

		bVideoDebug = false;
    }
}

void Video_Shutdown(void)
{
	if(!Video.bInitialized)
		return;

	// Let us know that we're shutting down the video sub-system...
	Con_Printf("Shutting down video...\n");

	// [6/12/2012] Delete our context ~hogsy
	if(sMainContext)
		SDL_GL_DeleteContext(sMainContext);

	// [6/12/2012] Destroy our window ~hogsy
	if(sMainWindow)
		SDL_DestroyWindow(sMainWindow);

	SDL_QuitSubSystem(SDL_INIT_VIDEO);

	Video.bInitialized = false;
}
