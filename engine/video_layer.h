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

#define	VL_MODE_OPENGL
//		VL_MODE_OPENGL_CORE
//		VL_MODE_GLIDE
//		VL_MODE_DIRECTX
//		VL_MODE_VULKAN

#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
typedef unsigned int vlVertexArray_t;
typedef unsigned int vlRenderBuffer_t;
typedef unsigned int vlFrameBuffer_t;
#endif

typedef enum
{
	VL_SHADER_FRAGMENT,
	VL_SHADER_VERTEX
} vlShaderType_t;

#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
#	define VL_MASK_COLOUR	GL_COLOR_BUFFER_BIT
#	define VL_MASK_DEPTH	GL_DEPTH_BUFFER_BIT
#	define VL_MASK_ACCUM	GL_ACCUM_BUFFER_BIT
#	define VL_MASK_STENCIL	GL_STENCIL_BUFFER_BIT
#else
#	define VL_MASK_COLOUR	(1 << 1)
#	define VL_MASK_DEPTH	(1 << 2)
#	define VL_MASK_ACCUM	(1 << 3)
#	define VL_MASK_STENCIL	(1 << 4)
#endif

typedef enum
{
#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
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
#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
	// Set these directly to avoid translation.
	VL_TEXTURE_FILTER_NEAREST	= GL_NEAREST,	// Nearest filtering
	VL_TEXTURE_FILTER_LINEAR	= GL_LINEAR		// Linear filtering
#else
	VL_TEXTURE_FILTER_NEAREST,					// Nearest filtering
	VL_TEXTURE_FILTER_LINEAR					// Linear filtering
#endif
} vlTextureFilter_t;

typedef enum
{
#ifdef VL_MODE_OPENGL
	VL_TEXTURE_FORMAT_RGB			= GL_RGB,
	VL_TEXTURE_FORMAT_RGBA			= GL_RGBA,
	VL_TEXTURE_FORMAT_BGR			= GL_BGR,
	VL_TEXTURE_FORMAT_BGRA			= GL_BGRA,
#else
	VL_TEXTURE_FORMAT_RGB,
	VL_TEXTURE_FORMAT_RGBA,
	VL_TEXTURE_FORMAT_BGR,
	VL_TEXTURE_FORMAT_BGRA,
#endif
} vlTextureFormat_t;

typedef enum
{
#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
	VL_TEXTURE_2D	= GL_TEXTURE_2D
#else
	VL_TEXTURE_2D
#endif
} vlTextureTarget_t;

typedef enum
{
#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
	VL_STRING_RENDERER		= GL_RENDERER,
	VL_STRING_VERSION		= GL_VERSION,
	VL_STRING_VENDOR		= GL_VENDOR,
	VL_STRING_EXTENSIONS	= GL_EXTENSIONS
#endif
} vlString_t;

plEXTERN_C_START

char *vlGetErrorString(unsigned int er);

const char *vlGetRenderer(void);
const char *vlGetVersion(void);
const char *vlGetExtensions(void);
const char *vlGetVendor(void);
const char *vlGetString(vlString_t string);

void vlGetMaxTextureImageUnits(int *params);
void vlGetMaxTextureAnistropy(float *params);

unsigned int vlGetTextureUnit(unsigned int target);

void vlPushMatrix(void);
void vlPopMatrix(void);

void vlActiveTexture(unsigned int texunit);
void vlTexImage2D(vlTextureTarget_t target, vlTextureFormat_t internal_format, vlTextureFormat_t format, int width, int height, const void *data);
void vlSetTextureFilter(vlTextureFilter_t FilterMode);
void vlSetTextureEnvironmentMode(VideoTextureEnvironmentMode_t TextureEnvironmentMode);

void vlEnable(unsigned int uiCapabilities);
void vlDisable(unsigned int uiCapabilities);

void vlBlendFunc(VideoBlend_t modea, VideoBlend_t modeb);
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