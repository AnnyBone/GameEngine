/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#pragma once

#include "platform.h"
#include "platform_math.h"

#define		VL_MODE_OPENGL
//			VL_MODE_OPENGL_CORE
//			VL_MODE_OPENGL_ES
//#define	VL_MODE_GLIDE
//#define	VL_MODE_DIRECT3D
//			VL_MODE_VULKAN

#if defined (VL_MODE_OPENGL)
#	include <GL/glew.h>

#	ifdef _WIN32
#		include <GL/wglew.h>
#	endif
#elif defined (VL_MODE_OPENGL_CORE)
#	include <GL/glcorearb.h>
#elif defined (VL_MODE_GLIDE)
#	ifdef _MSC_VER
#		define __MSC__
#	endif

#	include <glide.h>
#elif defined (VL_MODE_DIRECT3D)
#	include <d3d11.h>

#	pragma comment (lib, "d3d11.lib")
#	pragma comment (lib, "d3dx11.lib")
#	pragma comment (lib, "d3dx10.lib")
#elif defined (VL_MODE_VULKAN)
#endif

typedef PLuint VLVertexArray;
typedef PLuint VLRenderBuffer;
typedef PLuint VLFrameBuffer;
typedef PLuint VLTexture;

typedef enum VLDataFormat
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	VL_UNSIGNED_BYTE				= GL_UNSIGNED_BYTE,
	VL_UNSIGNED_INT_8_8_8_8_REV		= GL_UNSIGNED_INT_8_8_8_8_REV,
#endif
} VLDataFormat;

typedef enum VLString
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	VL_STRING_RENDERER		= GL_RENDERER,
	VL_STRING_VERSION		= GL_VERSION,
	VL_STRING_VENDOR		= GL_VENDOR,
	VL_STRING_EXTENSIONS	= GL_EXTENSIONS,
#elif defined VL_MODE_GLIDE
	VL_STRING_RENDERER		= GR_RENDERER,
	VL_STRING_VERSION		= GR_VERSION,
	VL_STRING_VENDOR		= GR_VENDOR,
	VL_STRING_EXTENSIONS	= GR_EXTENSION,
#else
	VL_STRING_RENDERER		= 0,
	VL_STRING_VERSION		= 1,
	VL_STRING_VENDOR		= 2,
	VL_STRING_EXTENSIONS	= 3,
#endif
} VLString;

typedef enum VLMask
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	VL_MASK_COLOUR		= GL_COLOR_BUFFER_BIT,
	VL_MASK_DEPTH		= GL_DEPTH_BUFFER_BIT,
	VL_MASK_ACCUM		= GL_ACCUM_BUFFER_BIT,
	VL_MASK_STENCIL		= GL_STENCIL_BUFFER_BIT,
#else
	VL_MASK_COLOUR		= (1 << 0),
	VL_MASK_DEPTH		= (1 << 1),
	VL_MASK_ACCUM		= (1 << 2),
	VL_MASK_STENCIL		= (1 << 3),
#endif
} VLMask;

typedef enum VLColourFormat
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	VL_COLOURFORMAT_ARGB,
	VL_COLOURFORMAT_ABGR,
	VL_COLOURFORMAT_RGB		= GL_RGB,
	VL_COLOURFORMAT_BGR		= GL_BGR,
	VL_COLOURFORMAT_RGBA	= GL_RGBA,
	VL_COLOURFORMAT_BGRA	= GL_BGRA,
#elif defined (VL_MODE_GLIDE)
	VL_COLOURFORMAT_ARGB	= GR_COLORFORMAT_ARGB,
	VL_COLOURFORMAT_ABGR	= GR_COLORFORMAT_ABGR,
	VL_COLOURFORMAT_RGBA	= GR_COLORFORMAT_RGBA,
	VL_COLOURFORMAT_BGRA	= GR_COLORFORMAT_BGRA,
#else
	VL_COLOURFORMAT_ARGB	= 0,
	VL_COLOURFORMAT_ABGR	= 1,
	VL_COLOURFORMAT_RGBA	= 2,
	VL_COLOURFORMAT_BGRA	= 3,
#endif
} VLColourFormat;

typedef enum VLCullMode
{
	VL_CULL_START = -1,

	VL_CULL_POSTIVE,
	VL_CULL_NEGATIVE,

	VL_CULL_END
} VLCullMode;

typedef enum VLCapability
{
	VL_CAPABILITY_START = -1,

	VL_CAPABILITY_FOG				= (1 << 0),	// Fog.
	VL_CAPABILITY_ALPHA_TEST		= (1 << 1),	// Alpha-testing.
	VL_CAPABILITY_BLEND				= (1 << 2), // Blending.
	VL_CAPABILITY_TEXTURE_2D		= (1 << 3),	// Enables/disables textures.
	VL_CAPABILITY_TEXTURE_GEN_S		= (1 << 4),	// Generate S coordinate.
	VL_CAPABILITY_TEXTURE_GEN_T		= (1 << 5), // Generate T coordinate.
	VL_CAPABILITY_DEPTH_TEST		= (1 << 6),	// Depth-testing.
	VL_CAPABILITY_STENCIL_TEST		= (1 << 7),	// Stencil-testing.
	VL_CAPABILITY_MULTISAMPLE		= (1 << 8), // Multisampling.
	VL_CAPABILITY_CULL_FACE			= (1 << 9),	// Automatically cull faces.
	VL_CAPABILITY_SCISSOR_TEST		= (1 << 10), // Scissor test for buffer clear.

	// Texture Generation
	VL_CAPABILITY_GENERATEMIPMAP	= (1 << 20),

	VL_CAPABILITY_END
} VLCapability;

// Blending Modes
typedef enum VLBlend
{
#if defined (VL_MODE_OPENGL)
	VL_BLEND_ZERO					= GL_ZERO,
	VL_BLEND_ONE					= GL_ONE,
	VL_BLEND_SRC_COLOR				= GL_SRC_COLOR,
	VL_BLEND_ONE_MINUS_SRC_COLOR	= GL_ONE_MINUS_SRC_COLOR,
	VL_BLEND_SRC_ALPHA				= GL_SRC_ALPHA,
	VL_BLEND_ONE_MINUS_SRC_ALPHA	= GL_ONE_MINUS_SRC_ALPHA,
	VL_BLEND_DST_ALPHA				= GL_DST_ALPHA,
	VL_BLEND_ONE_MINUS_DST_ALPHA	= GL_ONE_MINUS_DST_ALPHA,
	VL_BLEND_DST_COLOR				= GL_DST_COLOR,
	VL_BLEND_ONE_MINUS_DST_COLOR	= GL_ONE_MINUS_DST_COLOR,
	VL_BLEND_SRC_ALPHA_SATURATE		= GL_SRC_ALPHA_SATURATE,
#elif defined (VL_MODE_GLIDE)
	VL_BLEND_ZERO					= GR_BLEND_ZERO,
	VL_BLEND_ONE					= GR_BLEND_ONE,
	VL_BLEND_SRC_COLOR				= GR_BLEND_SRC_COLOR,
	VL_BLEND_ONE_MINUS_SRC_COLOR	= GR_BLEND_ONE_MINUS_SRC_COLOR,
	VL_BLEND_SRC_ALPHA				= GR_BLEND_SRC_ALPHA,
	VL_BLEND_ONE_MINUS_SRC_ALPHA	= GR_BLEND_ONE_MINUS_SRC_ALPHA,
	VL_BLEND_DST_ALPHA				= GR_BLEND_DST_ALPHA,
	VL_BLEND_ONE_MINUS_DST_ALPHA	= GR_BLEND_ONE_MINUS_DST_ALPHA,
	VL_BLEND_DST_COLOR				= GR_BLEND_DST_COLOR,
	VL_BLEND_ONE_MINUS_DST_COLOR	= GR_BLEND_ONE_MINUS_DST_COLOR,
	VL_BLEND_SRC_ALPHA_SATURATE		= GR_BLEND_ALPHA_SATURATE,
#else
	VL_BLEND_ZERO,
	VL_BLEND_ONE,
	VL_BLEND_SRC_COLOR,
	VL_BLEND_ONE_MINUS_SRC_COLOR,
	VL_BLEND_SRC_ALPHA,
	VL_BLEND_ONE_MINUS_SRC_ALPHA,
	VL_BLEND_DST_ALPHA,
	VL_BLEND_ONE_MINUS_DST_ALPHA,
	VL_BLEND_DST_COLOR,
	VL_BLEND_ONE_MINUS_DST_COLOR,
	VL_BLEND_SRC_ALPHA_SATURATE,
#endif
} VLBlend;

// Blending
#define	VL_BLEND_ADDITIVE	VL_BLEND_SRC_ALPHA, VL_BLEND_ONE
#define	VL_BLEND_DEFAULT	VL_BLEND_SRC_ALPHA, VL_BLEND_ONE_MINUS_SRC_ALPHA

//-----------------
// Textures

typedef enum VLTextureTarget
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	VL_TEXTURE_1D	= GL_TEXTURE_1D,
	VL_TEXTURE_2D	= GL_TEXTURE_2D,
	VL_TEXTURE_3D	= GL_TEXTURE_3D,
#else
	VL_TEXTURE_1D,
	VL_TEXTURE_2D,
	VL_TEXTURE_3D,
#endif
} VLTextureTarget;

typedef enum VLTextureFilter
{
	VL_TEXTUREFILTER_MIPMAP_NEAREST,	// GL_NEAREST_MIPMAP_NEAREST
	VL_TEXTUREFILTER_MIPMAP_LINEAR,		// GL_LINEAR_MIPMAP_LINEAR

	VL_TEXTUREFILTER_MIPMAP_LINEAR_NEAREST,	// GL_LINEAR_MIPMAP_NEAREST
	VL_TEXTUREFILTER_MIPMAP_NEAREST_LINEAR,	// GL_NEAREST_MIPMAP_LINEAR

#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	VL_TEXTUREFILTER_NEAREST	= GL_NEAREST,	// Nearest filtering
	VL_TEXTUREFILTER_LINEAR		= GL_LINEAR		// Linear filtering
#elif defined (VL_MODE_GLIDE)
	VL_TEXTUREFILTER_NEAREST	= GR_TEXTUREFILTER_POINT_SAMPLED,	// Nearest filtering
	VL_TEXTUREFILTER_LINEAR		= GR_TEXTUREFILTER_BILINEAR			// Linear filtering
#else
	VL_TEXTUREFILTER_NEAREST,	// Nearest filtering
	VL_TEXTUREFILTER_LINEAR		// Linear filtering
#endif
} VLTextureFilter;

typedef enum VLTextureFormat
{
#if defined (VL_MODE_OPENGL)
	VL_TEXTUREFORMAT_RGB8	= GL_RGB8,
	VL_TEXTUREFORMAT_RGBA8	= GL_RGBA8,

	VL_TEXTUREFORMAT_RGBA_DXT1	= GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
	VL_TEXTUREFORMAT_RGB_DXT1	= GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
	VL_TEXTUREFORMAT_RGBA_DXT3	= GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
	VL_TEXTUREFORMAT_RGBA_DXT5	= GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,

	VL_TEXTUREFORMAT_RGB_FXT1	= GL_COMPRESSED_RGB_FXT1_3DFX,
#else
	VL_TEXTUREFORMAT_RGB,
	VL_TEXTUREFORMAT_RGBA,
	VL_TEXTUREFORMAT_BGR,
	VL_TEXTUREFORMAT_BGRA,
#endif
} VLTextureFormat;

// Texture Environment Modes
typedef enum VLTextureEnvironmentMode
{
	VL_TEXTUREMODE_START = -1,

	VIDEO_TEXTUREMODE_ADD,
	VIDEO_TEXTUREMODE_MODULATE,
	VIDEO_TEXTUREMODE_DECAL,
	VIDEO_TEXTUREMODE_BLEND,
	VIDEO_TEXTUREMODE_REPLACE,
	VIDEO_TEXTUREMODE_COMBINE,

	VL_TEXTUREMODE_END
} VLTextureEnvironmentMode;

typedef struct VLTextureInfo
{
	PLbyte *data;

	PLuint x, y;
	PLuint width, height;
	PLuint size;
	PLuint levels;

	VLColourFormat	pixel_format;
	VLTextureFormat format;

	PLbool initial;

	PLuint storage_type;

	PLuint flags;
} VLTextureInfo;

//-----------------
// Drawing

typedef enum VLDrawMode
{
#if defined (VL_MODE_OPENGL)
	VL_DRAWMODE_STATIC		= GL_STATIC_DRAW,
	VL_DRAWMODE_DYNAMIC		= GL_DYNAMIC_DRAW,
#else
	VL_DRAWMODE_STATIC,
	VL_DRAWMODE_DYNAMIC,
#endif
} VLDrawMode;

// Primitive Types
typedef enum VLPrimitive
{
	VL_PRIMITIVE_IGNORE = -1,

	VL_PRIMITIVE_LINES,
	VL_PRIMITIVE_LINE_STRIP,
	VL_PRIMITIVE_POINTS,
	VL_PRIMITIVE_TRIANGLES,
	VL_PRIMITIVE_TRIANGLE_STRIP,
	VL_PRIMITIVE_TRIANGLE_FAN,
	VL_PRIMITIVE_TRIANGLE_FAN_LINE,
	VL_PRIMITIVE_QUADS,				// Advised to avoid this.

	VL_PRIMITIVE_END
} VLPrimitive;

typedef struct vlVertex_s
{
	plVector3f_t position;
	plVector3f_t normal;

	plVector2f_t ST[16];

	plColour_t colour;
} vlVertex_t;

typedef struct vlDraw_s
{
	vlVertex_t *vertices;							// Array of vertices for the object.

	unsigned int numverts;							// Number of vertices.
	unsigned int numtriangles;						// Number of triangles.

	uint8_t	*indices;								// List of indeces.

	VLPrimitive primitive, primitive_restore;		// Type of primitive, and primitive to restore to.

	unsigned int _gl_vbo[16];						// Vertex buffer object.
} vlDraw_t;

//-----------------
// Shaders

typedef unsigned int vlShaderProgram_t;
typedef unsigned int vlShader_t;

typedef enum VLUniformType
{
	VL_UNIFORM_START = -1,

	VL_UNIFORM_FLOAT,
	VL_UNIFORM_INT,
	VL_UNIFORM_UINT,
	VL_UNIFORM_BOOL,
	VL_UNIFORM_DOUBLE,

	// Textures
	VL_UNIFORM_TEXTURE1D,
	VL_UNIFORM_TEXTURE2D,
	VL_UNIFORM_TEXTURE3D,
	VL_UNIFORM_TEXTURECUBE,
	VL_UNIFORM_TEXTUREBUFFER,

	// Vectors
	VL_UNIFORM_VEC2,
	VL_UNIFORM_VEC3,
	VL_UNIFORM_VEC4,

	// Matrices
	VL_UNIFORM_MAT3,

	VL_UNIFORM_END
} VLUniformType;

#if 0
typedef int VLUniform;
#else
typedef struct VLUniform
{
	int location;	// Location within the shader.

	VLUniformType type;	// Data type.

	char def[16];	// Default value.
} VLUniform;
#endif

typedef int vlAttribute_t;

typedef enum VLShaderType
{
	VL_SHADER_START = -1,

	VL_SHADER_FRAGMENT,
	VL_SHADER_VERTEX,
	VL_SHADER_GEOMETRY,

	VL_SHADER_END
} VLShaderType;

//-----------------
// Lighting

typedef struct vlLight_s
{
	plVector3f_t	position;
	plColour_t		colour;
	float			radius;
} vlLight_t;

//-----------------
