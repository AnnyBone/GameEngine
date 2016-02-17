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

typedef enum
{
	VIDEO_FBO_DEFAULT,

	VIDEO_FBO_DRAW,
	VIDEO_FBO_READ
} VideoFBOTarget_t;

typedef enum
{
	VIDEO_TEXTURE_FILTER_NEAREST,
	VIDEO_TEXTURE_FILTER_LINEAR
} VideoTextureFilter_t;

typedef enum
{
	VIDEO_TEXTURE_FORMAT_RGB,
	VIDEO_TEXTURE_FORMAT_RGBA,
	VIDEO_TEXTURE_FORMAT_BGR,
	VIDEO_TEXTURE_FORMAT_BGRA,
	VIDEO_TEXTURE_FORMAT_LUMINANCE,
} VideoTextureFormat_t;

plEXTERN_C_START

char *vlGetErrorString(unsigned int er);

const char *vlGetString(unsigned int name);
const char *vlGetVendor(void);
const char *vlGetRenderer(void);
const char *vlGetVersion(void);
const char *vlGetExtensions(void);

void vlClear(void);

void vlGetMaxTextureImageUnits(int *params);
void vlGetMaxTextureAnistropy(float *params);

void vlPushMatrix(void);
void vlPopMatrix(void);

void vlSetupTexture(VideoTextureFormat_t InternalFormat, VideoTextureFormat_t Format, unsigned int Width, unsigned int Height);
void vlSetTextureFilter(VideoTextureFilter_t FilterMode);
void vlSetTextureEnvironmentMode(VideoTextureEnvironmentMode_t TextureEnvironmentMode);

void vlEnable(unsigned int uiCapabilities);
void vlDisable(unsigned int uiCapabilities);

void vlBlendFunc(VideoBlend_t modea, VideoBlend_t modeb);
void vlDepthMask(bool mode);

// Shaders
void VideoLayer_UseProgram(unsigned int program);

// Drawing
void vlDrawArrays(VideoPrimitive_t mode, unsigned int first, unsigned int count);
void vlDrawElements(VideoPrimitive_t mode, unsigned int count, unsigned int type, const void *indices);

// Vertex Array
void vlGenerateVertexArray(unsigned int *arrays);
void vlBindVertexArray(unsigned int array);

// Vertex Buffer
void vlGenerateVertexBuffer(unsigned int *uiBuffer);
void vlGenerateVertexBuffers(int num, unsigned int *buffers);
void vlBindBuffer(unsigned int target, unsigned int buffer);
void vlDeleteVertexBuffer(unsigned int *uiBuffer);
	
// Frame Buffer
void vlClearStencilBuffer(void);
void vlGenerateFrameBuffer(unsigned int *buffer);
void vlCheckFrameBufferStatus();
void vlBindFrameBuffer(VideoFBOTarget_t target, unsigned int buffer);
void vlAttachFrameBufferRenderBuffer(unsigned int attachment, unsigned int buffer);
void vlAttachFrameBufferTexture(gltexture_t *buffer);
void vlDeleteFrameBuffer(unsigned int *uiBuffer);

// Render Buffer
void vlGenerateRenderBuffer(unsigned int *buffer);
void vlBindRenderBuffer(unsigned int buffer);
void vlRenderBufferStorage(int format, int samples, unsigned int width, unsigned int height);
void vlDeleteRenderBuffer(unsigned int *buffer);

plEXTERN_C_END