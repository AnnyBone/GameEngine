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

#include "platform_video_layer.h"

#if defined (VL_MODE_OPENGL)
#	include <GL/glew.h>
#	ifdef _WIN32
#		include <GL/wglew.h>
#	endif

#	include <GL/glu.h>
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

typedef enum
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
} VLMask_t;

typedef enum XColourFormat
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	VL_COLOURFORMAT_ARGB,
	VL_COLOURFORMAT_ABGR,
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

	VL_COLOURFORMAT_END
} XColourFormat;

typedef enum
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

	VL_STRING_END
} vlString_t;

typedef enum
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	VL_FRAMEBUFFER_DEFAULT	= GL_FRAMEBUFFER,
	VL_FRAMEBUFFER_DRAW		= GL_DRAW_FRAMEBUFFER,
	VL_FRAMEBUFFER_READ		= GL_READ_FRAMEBUFFER
#else
	VL_FRAMEBUFFER_DEFAULT,
	VL_FRAMEBUFFER_DRAW,
	VL_FRAMEBUFFER_READ
#endif
} vlFBOTarget_t;

typedef enum
{
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
} vlTextureFilter_t;

typedef enum
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	VL_TEXTURECLAMP_CLAMP	= GL_CLAMP_TO_EDGE,
	VL_TEXTURECLAMP_WRAP	= GL_REPEAT,
#elif defined (VL_MODE_GLIDE)
	VL_TEXTURECLAMP_CLAMP	= GR_TEXTURECLAMP_CLAMP,
	VL_TEXTURECLAMP_WRAP	= GR_TEXTURECLAMP_WRAP,
#else
	VL_TEXTURECLAMP_CLAMP,
	VL_TEXTURECLAMP_WRAP,
#endif
} vlTextureClamp_t;

typedef enum
{
#ifdef VL_MODE_OPENGL
	VL_TEXTUREFORMAT_RGB	= GL_RGB,
	VL_TEXTUREFORMAT_RGBA	= GL_RGBA,
	VL_TEXTUREFORMAT_BGR	= GL_BGR,
	VL_TEXTUREFORMAT_BGRA	= GL_BGRA,
#else
	VL_TEXTUREFORMAT_RGB,
	VL_TEXTUREFORMAT_RGBA,
	VL_TEXTUREFORMAT_BGR,
	VL_TEXTUREFORMAT_BGRA,
#endif
} vlTextureFormat_t;

// Blending Modes
typedef enum
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

	VL_BLEND_END
} vlBlend_t;

plEXTERN_C_START

void vlInit(void);

char *vlGetErrorString(unsigned int er);

const char *vlGetExtensions(void);
const char *vlGetString(vlString_t string);

void vlPushMatrix(void);
void vlPopMatrix(void);

void vlEnable(unsigned int cap);
void vlDisable(unsigned int cap);

void vlBlendFunc(vlBlend_t modea, vlBlend_t modeb);
void vlDepthMask(bool mode);

//-----------------
// Textures

typedef enum
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

	VL_TEXTURE_END
} vlTextureTarget_t;

void vlGetMaxTextureAnistropy(float *params);
void vlGetMaxTextureImageUnits(int *param);

int vlGetMaxTextureSize(void);

unsigned int vlGetTextureUnit(unsigned int target);

void vlActiveTexture(unsigned int texunit);
void vlTexImage2D(vlTextureTarget_t target, vlTextureFormat_t internal_format, vlTextureFormat_t format, int width, int height, const void *data);

void vlSetTextureFilter(vlTextureFilter_t filter);
void vlSetTextureEnvironmentMode(vlTextureEnvironmentMode_t TextureEnvironmentMode);

void vlBindTexture(vlTextureTarget_t target, vlTexture_t texture);

//-----------------
// Shaders

vlShaderProgram_t vlCreateShaderProgram(void);
vlShaderProgram_t vlGetCurrentShaderProgram(void);

void vlUseShaderProgram(vlShaderProgram_t program);
void vlDeleteShaderProgram(vlShaderProgram_t *program);
void vlLinkShaderProgram(vlShaderProgram_t *program);
void vlAttachShader(vlShaderProgram_t program, vlShader_t shader);
void vlDeleteShader(vlShader_t *shader);

vlAttribute_t vlGetAttributeLocation(vlShaderProgram_t *program, const char *name);

//-----------------
// Drawing

typedef enum vlDrawMode_s
{
#if defined (VL_MODE_OPENGL)
	VL_DRAWMODE_STATIC		= GL_STATIC_DRAW,
	VL_DRAWMODE_DYNAMIC		= GL_DYNAMIC_DRAW,
#else
	VL_DRAWMODE_STATIC,
	VL_DRAWMODE_DYNAMIC,
#endif
} vlDrawMode_t;

vlDraw_t *vlCreateDraw(vlPrimitive_t primitive, uint32_t num_tris, uint32_t num_verts);
void vlDeleteDraw(vlDraw_t *draw);

void vlBeginDraw(vlDraw_t *draw);
void vlEndDraw(vlDraw_t *draw);
void vlDrawVertex3f(float x, float y, float z);
void vlDrawVertex3fv(plVector3f_t position);
void vlDrawColour4f(float r, float g, float b, float a);
void vlDrawColour4fv(plColour_t rgba);
void vlDrawNormal3fv(plVector3f_t position);
void vlDrawTexCoord2f(unsigned int target, float s, float t);

void vlDraw(vlDraw_t *draw);
void vlDrawVertexNormals(vlDraw_t *draw);

//-----------------
// Lighting

void vlApplyLighting(vlDraw_t *object, vlLight_t *light, plVector3f_t position);

//-----------------

// Frame Buffer
void vlGenerateFrameBuffer(unsigned int *buffer);
void vlCheckFrameBufferStatus();
void vlBindFrameBuffer(vlFBOTarget_t target, unsigned int buffer);
void vlAttachFrameBufferRenderBuffer(unsigned int attachment, unsigned int buffer);
void vlAttachFrameBufferTexture(gltexture_t *buffer);
void vlDeleteFrameBuffer(unsigned int *buffer);

// Render Buffer
void vlGenerateRenderBuffer(unsigned int *buffer);
void vlBindRenderBuffer(unsigned int buffer);
void vlRenderBufferStorage(int format, int samples, unsigned int width, unsigned int height);
void vlDeleteRenderBuffer(unsigned int *buffer);

void vlViewport(int x, int y, unsigned int width, unsigned int height);

void vlSwapBuffers(void);
void vlSetClearColour4f(float r, float g, float b, float a);
void vlSetClearColour4fv(plColour_t rgba);
void vlClearBuffers(unsigned int mask);
void vlScissor(int x, int y, unsigned int width, unsigned int height);
void vlColourMask(bool red, bool green, bool blue, bool alpha);
void vlSetCullMode(vlCullMode_t mode);
void vlFinish(void);

plEXTERN_C_END