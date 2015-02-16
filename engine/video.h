/*  Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef __ENGINEVIDEO__
#define __ENGINEVIDEO__

#include <SDL.h>
#include <SDL_syswm.h>

#include "shared_video.h"

//#define	VIDEO_ENABLE_SHADERS

extern cvar_t	cvVideoDrawModels,		// Should we draw models?
				cvWidth,				// The width of our window (not reliable).
				cvHeight,				// The height of our window (not reliable).
				cvFullscreen,			// Should we be fullscreen?
				cvVideoAlphaTrick,
				cvVideoMirror,			// Toggles mirrors.
				cvVideoPlayerShadow,	// Render players shadow.
				gl_overbright,			// Enable overbrights?
				cvLitParticles;			// Should particles be lit or not?

extern bool	bVideoIgnoreCapabilities;

typedef enum VideoUnits_e
{
	VIDEO_TEXTURE_MIN = -1,

	VIDEO_TEXTURE_DIFFUSE,
	VIDEO_TEXTURE_LIGHT,
	VIDEO_TEXTURE_DETAIL,
	VIDEO_TEXTURE_FULLBRIGHT,
	VIDEO_TEXTURE_SPHERE,

	VIDEO_TEXTURE_MAX
} VideoUnits_t;

#define	VIDEO_MAX_FRAMEBUFFFERS	1

typedef struct
{
	// OpenGL Information
	char	*cGLVendor,
			*cGLRenderer,
			*cGLVersion,
			*cGLExtensions;

	float			fMaxAnisotropy,		        // Max anisotropy amount allowed by the hardware.
					fBitsPerPixel;

    // Texture Management
	unsigned	int	iCurrentTexture,					// Current/last binded texture.
                    uiActiveUnit,
                    uiSecondaryUnit;					// Current/last secondary texture.

	unsigned	int	uiMSAASamples,	// Number of AA samples.
					uiFrameBuffer[VIDEO_MAX_FRAMEBUFFFERS],
					iWidth,iHeight;

	bool			bInitialized,					// Is the video system started?
					bFullscreen,					// Is the window fullscreen or not?
					bVerticalSync,					// Sync the swap interval to the refresh rate?
					bActive,						// Is the window active or not?
					bSkipUpdate,					// Skip screen update.
					bColourOverride,				// Override any applied colour for the object.
					bUnitState[VIDEO_MAX_UNITS],	// The state of each individual TMU.
					bUnlocked;						// Can we change the window settings or not?

	// OpenGL Extensions
	bool	bVertexBufferObject;		// ARB_vertex_buffer_object

	SDL_SysWMinfo	sSystemInfo;
} Video_t;

// Replacement Video Object
typedef struct
{
	MathVector_t	*vVertex;

	unsigned	int	uiIndeces;
} VideoObject2_t;

Video_t	Video;

extern SDL_Window	*sMainWindow;

// Video Capabilities
#define	VIDEO_ALPHA_TEST	1	// Alpha-testing
#define	VIDEO_BLEND			2	// Blending
#define	VIDEO_TEXTURE_2D	4	// Enables/disables textures.
#define	VIDEO_DEPTH_TEST	8	// Depth-testing
#define	VIDEO_TEXTURE_GEN_T	16	// Generate T coordinate.
#define	VIDEO_TEXTURE_GEN_S	32	// Generate S coordinate.
#define	VIDEO_CULL_FACE		64	// Automatically cull faces.
#define	VIDEO_STENCIL_TEST	128	// Stencil-testing
#define	VIDEO_NORMALIZE		256	// Normalization for scaled models that are lit.

// Primitive Types
typedef enum
{
    VIDEO_PRIMITIVE_TRIANGLES,
    VIDEO_PRIMITIVE_TRIANGLE_FAN
} VideoPrimitive_t;

// Blending Modes
typedef enum VideoBlend_e
{
    VIDEO_BLEND_IGNORE, // Don't bother changing blend mode.

    VIDEO_BLEND_ONE,    // ONE			ONE
    VIDEO_BLEND_TWO,    // SRC_ALPHA	ONE_MINUS_SRC_ALPHA
    VIDEO_BLEND_THREE,  // DST_COLOR	SRC_COLOR
    VIDEO_BLEND_FOUR    // ZERO			ZERO
} VideoBlend_t;

// Depth Modes
typedef enum VideoDepth_e
{
	VIDEO_DEPTH_IGNORE = -1,	// Don't bother changing depth mode.
	VIDEO_DEPTH_FALSE,			// Don't enable depth mask.
	VIDEO_DEPTH_TRUE			// Enable depth mask.
} VideoDepth_t;

// Shader Types
typedef enum VideoShaderType_e
{
	VIDEO_SHADER_VERTEX,
	VIDEO_SHADER_FRAGMENT
} VideoShaderType_t;

void Video_Initialize(void);
void Video_CreateWindow(void);
void Video_UpdateWindow(void);
void Video_ClearBuffer(void);
void Video_GenerateSphereCoordinates(void);
void Video_SetTexture(gltexture_t *gTexture);
void Video_SetBlend(VideoBlend_t voBlendMode, VideoDepth_t vdDepthMode);
void Video_SelectTexture(unsigned int uiTarget);
void Video_EnableCapabilities(unsigned int iCapabilities);
void Video_DisableCapabilities(unsigned int iCapabilities);
void Video_ResetCapabilities(bool bClearActive);
void Video_Frame(void);
void Video_ObjectTexture(VideoObject_t *voObject, unsigned int uiTextureUnit, float S, float T);
void Video_ObjectVertex(VideoObject_t *voObject, float X, float Y, float Z);
void Video_ObjectNormal(VideoObject_t *voObject, float X, float Y, float Z);
void Video_ObjectColour(VideoObject_t *voObject, float R, float G, float B, float A);
void Video_SetColour(float R, float G, float B, float A);
void Video_DrawFill(VideoObject_t *voFill,Material_t *mMaterial);
void Video_DrawSurface(msurface_t *mSurface,float fAlpha,Material_t *mMaterial, unsigned int uiSkin);
void Video_DrawObject(VideoObject_t *voObject, VideoPrimitive_t vpPrimitiveType, unsigned int uiVerts, Material_t *mMaterial, int iSkin);
void Video_DrawMaterial(Material_t *mMaterial, int iSkin, VideoObject_t *voObject, VideoPrimitive_t vpPrimitiveType, unsigned int uiSize, bool bPost);
void Video_Shutdown(void);

bool Video_GetCapability(unsigned int iCapability);

/*
	Draw
*/

void Draw_ResetCanvas(void);

/*
    Sprite
*/

void Sprite_Draw(entity_t *eEntity);

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

MathVector_t Light_GetSample(vec3_t vPoint);

DynamicLight_t *Light_GetDynamic(vec3_t vPoint);

void R_RenderDynamicLightmaps(msurface_t *fa);
void R_UploadLightmap(int lmap);

/*
	World
*/

void World_Draw(void);
void World_DrawWaterTextureChains(void);

/*
	Brush
*/

void Brush_Draw(entity_t *e);

void DrawGLPoly(glpoly_t *p);

/*
	Warp
*/

void Warp_DrawWaterPoly(glpoly_t *p);

#endif
