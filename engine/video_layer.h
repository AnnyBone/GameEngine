/*	Copyright (C) 2011-2015 OldTimes Software

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

#ifndef VIDEO_LAYER_H
#define	VIDEO_LAYER_H

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

#if __cplusplus
extern "C" {
#endif

	char *VideoLayer_GetErrorMessage(unsigned int uiGLError);

	void VideoLayer_PushMatrix(void);
	void VideoLayer_PopMatrix(void);

	void VideoLayer_SetupTexture(VideoTextureFormat_t InternalFormat, VideoTextureFormat_t Format, unsigned int Width, unsigned int Height);
	void VideoLayer_SetTextureFilter(VideoTextureFilter_t FilterMode);
	void VideoLayer_SetTextureEnvironmentMode(VideoTextureEnvironmentMode_t TextureEnvironmentMode);

	void VideoLayer_Enable(unsigned int uiCapabilities);
	void VideoLayer_Disable(unsigned int uiCapabilities);
	void VideoLayer_SetBlend(VideoBlend_t mode);

	// Drawing
	void VideoLayer_DrawArrays(const VideoPrimitive_t vpPrimitiveType, unsigned int uiSize, bool bWireframe);

	// Vertex Buffer
	void VideoLayer_GenerateVertexBuffer(unsigned int *uiBuffer);
	void VideoLayer_DeleteVertexBuffer(unsigned int *uiBuffer);
	
	// Frame Buffer
	void VideoLayer_GenerateFrameBuffer(unsigned int *uiBuffer);
	void VideoLayer_CheckFrameBufferStatus();
	void VideoLayer_BindFrameBuffer(VideoFBOTarget_t vtTarget, unsigned int uiBuffer);
	void VideoLayer_AttachFrameBufferRenderBuffer(unsigned int attachment, unsigned int buffer);
	void VideoLayer_AttachFrameBufferTexture(gltexture_t *buffer);
	void VideoLayer_DeleteFrameBuffer(unsigned int *uiBuffer);

	// Render Buffer
	void VideoLayer_GenerateRenderBuffer(unsigned int *buffer);
	void VideoLayer_BindRenderBuffer(unsigned int buffer);
	void VideoLayer_RenderBufferStorage(int format, int samples, unsigned int width, unsigned int height);
	void VideoLayer_DeleteRenderBuffer(unsigned int *buffer);

#if __cplusplus
};
#endif

#endif