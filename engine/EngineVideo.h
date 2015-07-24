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

#ifndef __ENGINEVIDEO__
#define __ENGINEVIDEO__

extern ConsoleVariable_t	
	cvVideoLegacy,
	cvVideoDrawModels,		// Should we draw models?
	cvWidth,				// The width of our window (not reliable).
	cvHeight,				// The height of our window (not reliable).
	cvFullscreen,			// Should we be fullscreen?
	cvMultisampleSamples,	// Number of samples we're using.
	cvVideoAlphaTrick,
	cvVideoMirror,			// Toggles mirrors.
	cvVideoPlayerShadow,	// Render players shadow.
	cvVerticalSync,
	cvVideoOverbright,		// Enable overbrights?
	cvLitParticles;			// Should particles be lit or not?

extern bool	bVideoIgnoreCapabilities;

extern struct gltexture_s *gEffectTexture[MAX_EFFECTS];

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

#include "shared_video.h"

typedef struct
{
	// OpenGL Information
	char	*cGLVendor,
			*cGLRenderer,
			*cGLVersion,
			*cGLExtensions;

	float			fMaxAnisotropy,	// Max anisotropy amount allowed by the hardware.
					fBitsPerPixel;

    // Texture Management
	unsigned	int	iCurrentTexture,	// Current/last binded texture.
                    uiActiveUnit,		// The currently active unit.
                    uiSecondaryUnit;	// Current/last secondary texture.

	int iSupportedUnits;	// Max number of supported units.
	int iMSAASamples;

	unsigned	int	uiFrameBuffer[VIDEO_MAX_FRAMEBUFFFERS],
					iWidth,iHeight;

	bool
		bInitialized,					// Is the video system started?
		bFullscreen,					// Is the window fullscreen or not?
		bVerticalSync,					// Sync the swap interval to the refresh rate?
		bActive,						// Is the window active or not?
		bSkipUpdate,					// Skip screen update.
		bColourOverride,				// Override any applied colour for the object.
		bUnitState[VIDEO_MAX_UNITS],	// The state of each individual TMU.
		bDebugFrame,
		bUnlocked;						// Can we change the window settings or not?

	// OpenGL Extensions
	bool
		bGenerateMipMap,
		bVertexBufferObject;	// ARB_vertex_buffer_object
} Video_t;

extern Video_t Video;

void Video_Initialize(void);
void Video_UpdateWindow(void);
void Video_ClearBuffer(void);
void Video_GenerateSphereCoordinates(void);
void Video_SetTexture(gltexture_t *gTexture);
void Video_SetBlend(VideoBlend_t voBlendMode, VideoDepth_t vdDepthMode);
void Video_SetViewportSize(int iWidth, int iHeight);
void Video_SelectTexture(unsigned int uiTarget);
void Video_EnableCapabilities(unsigned int iCapabilities);
void Video_DisableCapabilities(unsigned int iCapabilities);
void Video_ResetCapabilities(bool bClearActive);
void Video_PreFrame(void);
void Video_PostFrame(void);
void Video_Frame(void);
void Video_ObjectTexture(VideoObjectVertex_t *voObject, unsigned int uiTextureUnit, float S, float T);
void Video_ObjectVertex(VideoObjectVertex_t *voObject, float X, float Y, float Z);
void Video_ObjectNormal(VideoObjectVertex_t *voObject, float X, float Y, float Z);
void Video_ObjectColour(VideoObjectVertex_t *voObject, float R, float G, float B, float A);
void Video_SetColour(float R, float G, float B, float A);
void Video_DrawArrays(VideoPrimitive_t vpPrimitiveType, unsigned int uiSize, bool bWireframe);
void Video_DrawFill(VideoObjectVertex_t *voFill, Material_t *mMaterial, int iSkin);
void Video_DrawSurface(msurface_t *mSurface,float fAlpha,Material_t *mMaterial, unsigned int uiSkin);
void Video_DrawObject(VideoObjectVertex_t *voObject, VideoPrimitive_t vpPrimitiveType, unsigned int uiVerts, Material_t *mMaterial, int iSkin);
void Video_DrawMaterial(Material_t *mMaterial, int iSkin, VideoObjectVertex_t *voObject, VideoPrimitive_t vpPrimitiveType, unsigned int uiSize, bool bPost);
void Video_Shutdown(void);

unsigned int Video_GetTextureUnit(unsigned int uiTarget);

bool Video_GetCapability(unsigned int iCapability);

#define	VIDEO_FUNCTION_START(a) \
{ \
	static int iFUNCTIONCALLS = -1; \
	iFUNCTIONCALLS++; \
	if(Video.bDebugFrame) \
        pLog_Write(va("log_video_%s", __FILE__), "Function start: %s (%i)", pFUNCTION, iFUNCTIONCALLS); \
} \
{
#define	VIDEO_FUNCTION_END \
}

// Legacy
void Video_ShowBoundingBoxes(void);

unsigned int VideoLayer_GenerateVertexBuffer(void);

void VideoLayer_DeleteBuffer(unsigned int uiBuffer);

#include "EngineVideoAlias.h"

void R_SetupGenericView(void);
void R_SetupScene(void);

/*
    Sprite
*/

void Sprite_Draw(ClientEntity_t *eEntity);

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

MathVector_t Light_GetSample(MathVector3f_t vPoint);

DynamicLight_t *Light_GetDynamic(MathVector3f_t vPoint, bool bCheap);

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

void Brush_Draw(ClientEntity_t *e);

void DrawGLPoly(glpoly_t *p);

/*
	Warp
*/

void Warp_DrawWaterPoly(glpoly_t *p);

void R_EmitWirePoint(MathVector3f_t origin);

bool R_CullBox(MathVector3f_t emins, MathVector3f_t emaxs);

#endif
