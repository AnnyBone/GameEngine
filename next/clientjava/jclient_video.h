/*  Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef __ENGINEVIDEO__
#define __ENGINEVIDEO__

//#define	VIDEO_ENABLE_SHADERS

#define	VIDEO_MAX_UNITS	16

// Video Object
typedef struct
{
	MathVector3_t	vVertex;

	MathVector2_t	vTextureCoord[VIDEO_MAX_UNITS];		// Texture coordinates by texture unit.

	MathVector4_t	vColour;							// RGBA

	MathVector3_t	vNormal;							// Vertex normals.
} VideoObject_t;

typedef struct
{
	char	*cName;

	int	iID;
} VideoTexture_t;

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

typedef enum
{
	VIDEO_MODE_DEFAULT,		// Textured and lit.
	VIDEO_MODE_WIREFRAME,	// Wireframe display mode.

	VIDEO_MODE_NONE
} VideoRenderMode_t;

typedef struct
{
	// OpenGL Information
	char	*cGLVendor,
			*cGLRenderer,
			*cGLVersion,
			*cGLExtensions;

	float	
		fGamma,			// Current gamma level.
		fMaxAnisotropy,	// Max anisotropy amount allowed by the hardware.
		fBitsPerPixel;

	// Texture Management
	unsigned	int	iCurrentTexture,					// Current/last binded texture.
		uiActiveUnit,
		uiSecondaryUnit;					// Current/last secondary texture.

	unsigned	int	uiMSAASamples,	// Number of AA samples.
		uiFrameBuffer[VIDEO_MAX_FRAMEBUFFFERS],
		iWidth, iHeight;

	bool			
		bInitialized,					// Is the video system started?
		bFullscreen,					// Is the window fullscreen or not?
		bVerticalSync,					// Sync the swap interval to the refresh rate?
		bActive,						// Is the window active or not?
		bSkipUpdate,					// Skip screen update.
		bColourOverride,				// Override any applied colour for the object.
		bUnitState[VIDEO_MAX_UNITS],	// The state of each individual TMU.
		bUnlocked;						// Can we change the window settings or not?

	VideoRenderMode_t	vmrActiveMode;	// Active rendering mode.

	// OpenGL Extensions
	bool	bVertexBufferObject;		// ARB_vertex_buffer_object
} Video_t;

// Replacement Video Object
typedef struct
{
	MathVector_t	*vVertex;

	unsigned	int	uiIndeces;
} VideoObject2_t;

Video_t	Video;

extern GLFWwindow	*gMainWindow;

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
	VIDEO_PRIMITIVE_TRIANGLE_FAN,
	VIDEO_PRIMITIVE_TRIANGLE_FAN_WIRE
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

bool JVideo_Initialize(void);	// Returns true on success, returns false on failure.

void Video_CreateWindow(void);
void Video_UpdateWindow(void);
void Video_ClearBuffer(void);
void Video_GenerateSphereCoordinates(void);
void Video_SetTexture(VideoTexture_t *vtTexture);
void Video_SetBlend(VideoBlend_t voBlendMode, VideoDepth_t vdDepthMode);
void Video_SelectTexture(unsigned int uiTarget);
void Video_EnableCapabilities(unsigned int iCapabilities);
void Video_DisableCapabilities(unsigned int iCapabilities);
void Video_ResetCapabilities(bool bClearActive);
void JVideo_Frame(void);
void Video_ObjectTexture(VideoObject_t *voObject, unsigned int uiTextureUnit, float S, float T);
void Video_ObjectVertex(VideoObject_t *voObject, float X, float Y, float Z);
void Video_ObjectNormal(VideoObject_t *voObject, float X, float Y, float Z);
void Video_ObjectColour(VideoObject_t *voObject, float R, float G, float B, float A);
void Video_SetColour(float R, float G, float B, float A);
void Video_DrawFill(VideoObject_t *voFill);
void Video_DrawObject(VideoObject_t *voObject, VideoPrimitive_t vpPrimitiveType, unsigned int uiVerts);
void JVideo_Shutdown(void);

bool Video_GetCapability(unsigned int iCapability);

#endif
