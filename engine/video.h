/*  Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef __ENGINEVIDEO__
#define __ENGINEVIDEO__

#include <SDL.h>
#include <SDL_syswm.h>

#define	VIDEO_MAX_UNITS	16

//#define	VIDEO_ENABLE_SHADERS

extern cvar_t	cvVideoDrawModels,		// Should we draw models?
				cvWidth,				// The width of our window (not reliable).
				cvHeight,				// The height of our window (not reliable).
				cvFullscreen,			// Should we be fullscreen?
				cvVideoAlphaTrick,
				cvVideoMirror,			// Toggles mirrors.
				cvVideoPlayerShadow,	// Render players shadow.
				cvVerticalSync,
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
	unsigned	int	iCurrentTexture,	// Current/last binded texture.
                    uiActiveUnit,		// The currently active unit.
                    uiSecondaryUnit;	// Current/last secondary texture.

	int iSupportedUnits;	// Max number of supported units.

	unsigned	int	uiMSAASamples,	// Number of AA samples.
					uiFrameBuffer[VIDEO_MAX_FRAMEBUFFFERS],
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
		bVertexBufferObject;		// ARB_vertex_buffer_object

	SDL_SysWMinfo	sSystemInfo;
} Video_t;

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
	VIDEO_PRIMITIVE_LINE,
	VIDEO_PRIMITIVE_QUAD_STRIP,
    VIDEO_PRIMITIVE_TRIANGLES,
    VIDEO_PRIMITIVE_TRIANGLE_FAN,
	VIDEO_PRIMITIVE_TRIANGLE_FAN_LINE
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

// Vertex
typedef struct
{
	MathVector3_t mvPosition;				// Vertex position.
	MathVector3_t mvNormal;					// Vertex normal.

	MathVector2_t mvST[VIDEO_MAX_UNITS];	// Vertex texture coord, per unit.

	MathVector4_t mvColour;					// Vertex RGBA.
} VideoObjectVertex_t;

// Object
typedef struct
{
	VideoObjectVertex_t *ovVertices;	// Array of vertices for the object.

	int iVertices;						// Number of vertices.

	VideoPrimitive_t vpPrimitiveType;

	bool bWireframeOverride;			// If wireframe view is active, override it for this object.

	unsigned int uiVertexBuffer;
	unsigned int uiColourBuffer;
	unsigned int uiTextureBuffer;
} VideoObjectX_t;

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
void Video_DrawFill(VideoObjectVertex_t *voFill, Material_t *mMaterial);
void Video_DrawSurface(msurface_t *mSurface,float fAlpha,Material_t *mMaterial, unsigned int uiSkin);
void Video_DrawObject(VideoObjectVertex_t *voObject, VideoPrimitive_t vpPrimitiveType, unsigned int uiVerts, Material_t *mMaterial, int iSkin);
void Video_DrawMaterial(Material_t *mMaterial, int iSkin, VideoObjectVertex_t *voObject, VideoPrimitive_t vpPrimitiveType, unsigned int uiSize, bool bPost);
void Video_Shutdown(void);



unsigned int Video_GetTextureUnit(unsigned int uiTarget);

bool Video_GetCapability(unsigned int iCapability);

#define	VIDEO_FUNCTION_START(a) \
{ \
	static int a = -1; \
	a++; \
	if(Video.bDebugFrame) Console_WriteToLog(va("log_video_"__FILE__), "Function start: "pFUNCTION" (%i)",a); \
} \
{
#define	VIDEO_FUNCTION_END \
}

/*
	Layer
*/

unsigned int VideoLayer_GenerateVertexBuffer(void);

void VideoLayer_DeleteBuffer(unsigned int uiBuffer);

/*
	Screen
*/

void Screen_DrawFPS(void);
void Screen_DrawConsole(void);
void Screen_SetUpToDrawConsole(void);
void Screen_UpdateSize(void);

/*
	Legacy
*/

void R_SetupGenericView(void);
void R_SetupScene(void);

/*
	Draw
*/

void Draw_ResetCanvas(void);
void Draw_Line(MathVector3f_t mvStart, MathVector3f_t mvEnd);
void Draw_Grid(MathVector3f_t mvPosition, int iGridSize);

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

DynamicLight_t *Light_GetDynamic(vec3_t vPoint,bool bCheap);

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

void R_EmitWirePoint(vec3_t origin);

bool R_CullBox(vec3_t emins, vec3_t emaxs);

#endif
