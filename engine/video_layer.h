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

//		VL_MODE_OPENGL
//		VL_MODE_OPENGL_CORE
#define	VL_MODE_GLIDE
//		VL_MODE_DIRECT3D
//		VL_MODE_VULKAN

#if defined (VL_MODE_OPENGL)
#	include "GLee.h"

#	include <GL/glu.h>
#elif defined (VL_MODE_OPENGL_CORE)
#	include <GL/glcorearb.h>
#elif defined (VL_MODE_GLIDE)
#	ifdef _MSC_VER
#		define __MSC__
#	endif

#	include <glide.h>
#elif defined (VL_MODE_DIRECT3D)
#elif defined (VL_MODE_VULKAN)
#endif

typedef unsigned int vlVertexArray_t;
typedef unsigned int vlRenderBuffer_t;
typedef unsigned int vlFrameBuffer_t;

typedef enum
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	VL_MASK_COLOUR		= GL_COLOR_BUFFER_BIT,
	VL_MASK_DEPTH		= GL_DEPTH_BUFFER_BIT,
	VL_MASK_ACCUM		= GL_ACCUM_BUFFER_BIT,
	VL_MASK_STENCIL		= GL_STENCIL_BUFFER_BIT
#else
	VL_MASK_COLOUR		= (1 << 0),
	VL_MASK_DEPTH		= (1 << 1),
	VL_MASK_ACCUM		= (1 << 2),
	VL_MASK_STENCIL		= (1 << 3)
#endif
} vlMask_t;

typedef enum
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	// Set these directly to avoid translation.
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
	// Set these directly to avoid translation.
	VL_TEXTURE_FILTER_NEAREST	= GL_NEAREST,	// Nearest filtering
	VL_TEXTURE_FILTER_LINEAR	= GL_LINEAR		// Linear filtering
#elif defined (VL_MODE_GLIDE)
	VL_TEXTURE_FILTER_NEAREST	= GR_TEXTUREFILTER_POINT_SAMPLED,	// Nearest filtering
	VL_TEXTURE_FILTER_LINEAR	= GR_TEXTUREFILTER_BILINEAR			// Linear filtering
#else
	VL_TEXTURE_FILTER_NEAREST,					// Nearest filtering
	VL_TEXTURE_FILTER_LINEAR					// Linear filtering
#endif
} vlTextureFilter_t;

typedef enum
{
#ifdef VL_MODE_OPENGL
	VL_TEXTURE_FORMAT_RGB	= GL_RGB,
	VL_TEXTURE_FORMAT_RGBA	= GL_RGBA,
	VL_TEXTURE_FORMAT_BGR	= GL_BGR,
	VL_TEXTURE_FORMAT_BGRA	= GL_BGRA,
#else
	VL_TEXTURE_FORMAT_RGB,
	VL_TEXTURE_FORMAT_RGBA,
	VL_TEXTURE_FORMAT_BGR,
	VL_TEXTURE_FORMAT_BGRA,
#endif
} vlTextureFormat_t;

typedef enum
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	VL_TEXTURE_2D	= GL_TEXTURE_2D
#else
	VL_TEXTURE_2D
#endif
} vlTextureTarget_t;

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
	VL_BLEND_SRC_ALPHA_SATURATE		= GL_SRC_ALPHA_SATURATE
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
	VL_BLEND_SRC_ALPHA_SATURATE		= GR_BLEND_ALPHA_SATURATE
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
	VL_BLEND_SRC_ALPHA_SATURATE
#endif
} vlBlend_t;

#define	VL_BLEND_ADDITIVE	VL_BLEND_SRC_ALPHA, VL_BLEND_ONE
#define	VL_BLEND_DEFAULT	VL_BLEND_SRC_ALPHA, VL_BLEND_ONE_MINUS_SRC_ALPHA

typedef enum
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	VL_STRING_RENDERER		= GL_RENDERER,
	VL_STRING_VERSION		= GL_VERSION,
	VL_STRING_VENDOR		= GL_VENDOR,
	VL_STRING_EXTENSIONS	= GL_EXTENSIONS
#elif defined VL_MODE_GLIDE
	VL_STRING_RENDERER		= GR_RENDERER,
	VL_STRING_VERSION		= GR_VERSION,
	VL_STRING_VENDOR		= GR_VENDOR,
	VL_STRING_EXTENSIONS	= GR_EXTENSION
#else
	VL_STRING_RENDERER,
	VL_STRING_VERSION,
	VL_STRING_VENDOR,
	VL_STRING_EXTENSIONS
#endif
} vlString_t;

plEXTERN_C_START

char *vlGetErrorString(unsigned int er);

const char *vlGetRenderer(void);
const char *vlGetVersion(void);
const char *vlGetExtensions(void);
const char *vlGetVendor(void);
const char *vlGetString(vlString_t string);

void vlGetMaxTextureAnistropy(float *params);

unsigned int vlGetMaxTextureImageUnits(void);
unsigned int vlGetTextureUnit(unsigned int target);

void vlPushMatrix(void);
void vlPopMatrix(void);

void vlActiveTexture(unsigned int texunit);
void vlTexImage2D(vlTextureTarget_t target, vlTextureFormat_t internal_format, vlTextureFormat_t format, int width, int height, const void *data);
void vlSetTextureFilter(vlTextureFilter_t FilterMode);
void vlSetTextureEnvironmentMode(VideoTextureEnvironmentMode_t TextureEnvironmentMode);

void vlEnable(unsigned int uiCapabilities);
void vlDisable(unsigned int uiCapabilities);

void vlBlendFunc(vlBlend_t modea, vlBlend_t modeb);
void vlDepthMask(bool mode);

// Shaders
void vlUseProgram(unsigned int program);

// Drawing
void vlDrawArrays(VideoPrimitive_t mode, unsigned int first, unsigned int count);
void vlDrawElements(VideoPrimitive_t mode, unsigned int count, unsigned int type, const void *indices);

// Vertex Array
void vlGenerateVertexArray(unsigned int *arrays);
void vlBindVertexArray(unsigned int array);

// Vertex Buffer
void vlGenerateVertexBuffer(unsigned int *buffer);
void vlGenerateVertexBuffers(int num, unsigned int *buffers);
void vlBindBuffer(unsigned int target, unsigned int buffer);
void vlDeleteVertexBuffer(unsigned int *uiBuffer);
	
// Frame Buffer
void vlClear(unsigned int mask);
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

plEXTERN_C_END