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

//#define	VIDEO_SUPPORT_FRAMEBUFFERS
#define	VIDEO_SUPPORT_SHADERS
#define	VIDEO_LIGHTMAP_HACKS

plEXTERN_C_START

extern ConsoleVariable_t
	cv_video_shaders,
	cv_video_drawmodels,			// Should we draw models?
	cv_video_width,					// The width of our window (not reliable).
	cv_video_height,				// The height of our window (not reliable).
	cv_video_fullscreen,			// Should we be fullscreen?
	cv_video_msaasamples,			// Number of samples we're using.
	cv_video_alphatrick,			// TODO: move into material codebase
	cv_video_drawmirrors,			// Toggles mirrors.
	cv_video_drawplayershadow,		// Render players shadow.
	cv_video_verticlesync,
	cv_video_lightmapoversample,	// Enable overbrights?
	cv_video_drawbrushes,			// Draw brush entities?

	cv_video_shownormals,			// Show vertex normals?

	cvLitParticles;					// Should particles be lit or not?

extern	ConsoleVariable_t	r_showtris;
extern  ConsoleVariable_t	r_drawentities;
extern	ConsoleVariable_t	r_drawworld;
extern	ConsoleVariable_t	r_drawviewmodel;
extern	ConsoleVariable_t	r_speeds;
extern	ConsoleVariable_t	r_waterwarp;
extern	ConsoleVariable_t	r_fullbright;
extern	ConsoleVariable_t	r_lightmap;
extern	ConsoleVariable_t	r_shadows;
extern	ConsoleVariable_t	r_dynamic;
extern	ConsoleVariable_t	r_novis;
extern	ConsoleVariable_t	r_nocull;
extern	ConsoleVariable_t	gl_cull;
extern	ConsoleVariable_t	gl_polyblend;
extern	ConsoleVariable_t	gl_flashblend;
extern	ConsoleVariable_t	gl_max_size;

extern ConsoleVariable_t cv_video_drawshadowmap;
extern ConsoleVariable_t cv_video_drawshadowblob;
extern ConsoleVariable_t cv_video_drawdetail;	// TODO: Move into EngineMaterial ?
extern ConsoleVariable_t cv_video_drawsky;
extern ConsoleVariable_t cv_video_detailscale; // TODO: Move into EngineMaterial ?

extern bool	bVideoIgnoreCapabilities;

extern struct gltexture_s *gEffectTexture[MAX_EFFECTS];

plEXTERN_C_END

#define	VIDEO_TEXTURE_DIFFUSE	0
#define	VIDEO_TEXTURE_LIGHT		1

#define	VIDEO_MAX_FRAMEBUFFFERS	1

#define	VIDEO_LOG	"video"

#include "shared_video.h"

typedef struct
{
	bool isactive;

	unsigned int current_texture;
	unsigned int capabilities[2];

	VideoTextureEnvironmentMode_t current_envmode;
} VideoTextureMU_t;

typedef struct
{
	bool	fog_coord;
	bool	vertex_buffer_object;
	bool	shadow;
	bool	depth_texture;
	bool	generate_mipmap;
} VideoExtensions;

typedef struct
{
	// OpenGL Information
	const char	
		*gl_vendor,
		*gl_renderer,
		*gl_version,
		*gl_extensions;

	float	
		fMaxAnisotropy,	// Max anisotropy amount allowed by the hardware.
		bpp;			// Bits per-pixel.

	// Texture Management
	VideoTextureMU_t	*textureunits;
	int					num_textureunits;					// Max number of supported units.
	unsigned int		current_textureunit;				// Current TMU.
	unsigned int		current_texture[VIDEO_MAX_UNITS];	// Current/last binded texture.
	bool				textureunit_state[VIDEO_MAX_UNITS];	// The state of each individual TMU.

	unsigned int current_program;

	int msaa_samples;

	unsigned int iWidth,iHeight;

	int iFrameCount;	// An alternative to r_framecount, which is slightly more reliable.

	bool
		bInitialized,					// Is the video system started?
		fullscreen,						// Is the window fullscreen or not?
		vertical_sync,					// Sync the swap interval to the refresh rate?
		bActive,						// Is the window active or not?
		bSkipUpdate,					// Skip screen update.
		debug_frame,
		unlocked;						// Can we change the window settings or not?

	// OpenGL Extensions
	VideoExtensions	extensions;
} Video_t;

plEXTERN_C_START

extern Video_t Video;

void Video_Initialize(void);
void Video_UpdateWindow(void);
void Video_ClearBuffer(void);
void Video_GenerateSphereCoordinates(void);
void Video_SetTexture(gltexture_t *gTexture);
void Video_SetViewportSize(int w, int h);
void Video_EnableCapabilities(unsigned int iCapabilities);
void Video_DisableCapabilities(unsigned int iCapabilities);
void Video_PreFrame(void);
void Video_PostFrame(void);
void Video_Frame(void);
void Video_ObjectTexture(VideoVertex_t *voObject, unsigned int uiTextureUnit, float S, float T);
void Video_ObjectVertex(VideoVertex_t *voObject, float X, float Y, float Z);
void Video_ObjectNormal(VideoVertex_t *voObject, float X, float Y, float Z);
void Video_ObjectColour(VideoVertex_t *voObject, float R, float G, float B, float A);
void Video_DrawFill(VideoVertex_t *voFill, Material_t *mMaterial, int iSkin);
void Video_DrawSurface(msurface_t *mSurface, float fAlpha, Material_t *mMaterial, unsigned int uiSkin);
void Video_DrawObject(VideoVertex_t *voObject, VideoPrimitive_t vpPrimitiveType, unsigned int uiVerts, Material_t *mMaterial, int iSkin);
void Video_ShowBoundingBoxes(void);
void Video_Shutdown(void);

bool Video_GetCapability(unsigned int iCapability);

// Temporary
void DEBUG_FrameBufferInitialization();
void DEBUG_FrameBufferBind();
void DEBUG_FrameBufferDraw();

// Legacy
void R_EmitWireBox(MathVector3f_t mins, MathVector3f_t maxs, float r, float g, float b);
bool R_CullBox(MathVector3f_t emins, MathVector3f_t emaxs);

// Brush
void GL_BuildLightmaps();
void R_RebuildAllLightmaps();

MathVector_t Light_GetSample(MathVector3f_t vPoint);

// Legacy
extern float r_world_matrix[16], r_base_world_matrix[16];
void R_SetupGenericView(void);
void R_SetupScene(void);

plEXTERN_C_END

// TODO: Reintroduce tracking functionality.
#define	VIDEO_FUNCTION_START \
{ \
	static unsigned int callnum = 0; \
	if(Video.debug_frame) \
	{ \
		callnum++; \
	} \
}
#define	VIDEO_FUNCTION_END \

#include "video_layer.h"
#include "video_object.h"

// Legacy
#include "EngineVideoAlias.h"

plEXTERN_C_START

/*
	Sky
*/

void Sky_Draw(void);

/*
	Light
*/

void Light_Draw(void);
void Light_Animate(void);
void Light_MarkLights(DynamicLight_t *light,int bit,mnode_t *node);

DynamicLight_t *Light_GetDynamic(MathVector3f_t vPoint, bool bCheap);

void R_RenderDynamicLightmaps(msurface_t *fa);
void R_UploadLightmap(int lmap);

/*
	World
*/

void World_Draw(void);
void World_DrawWater(void);

/*
	Brush
*/

void Brush_Draw(ClientEntity_t *e);

void DrawGLPoly(glpoly_t *p);

/*
	Warp
*/

void Surface_DrawWater(glpoly_t *p, Material_t *mCurrent);

plEXTERN_C_END
