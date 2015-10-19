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

#include "EngineBase.h"

/*
	This document acts as a layer between the video sub-system
	and OpenGL. All OpenGL functionality should be here.
*/

#include "EngineVideo.h"

/*===========================
	OPENGL ERROR HANDLING
===========================*/

/*	Checks glGetError and returns a string
	describing the fault.
*/
char *VideoLayer_GetErrorMessage(unsigned int uiGLError)
{
	VIDEO_FUNCTION_START
	switch (uiGLError)
	{
	case GL_NO_ERROR:
		return "No error has been recorded.";
	case GL_INVALID_ENUM:
		return "An unacceptable value is specified for an enumerated argument.";
	case GL_INVALID_VALUE:
		return "A numeric argument is out of range.";
	case GL_INVALID_OPERATION:
		return "The specified operation is not allowed in the current state.";
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return "The framebuffer object is not complete.";
	case GL_OUT_OF_MEMORY:
		return "There is not enough memory left to execute the command.";
	case GL_STACK_UNDERFLOW:
		return "An attempt has been made to perform an operation that would	cause an internal stack to underflow.";
	case GL_STACK_OVERFLOW:
		return "An attempt has been made to perform an operation that would	cause an internal stack to overflow.";
	default:
		return "An unknown error occured.";
	}
	VIDEO_FUNCTION_END
}

/*===========================
	OPENGL MATRICES
===========================*/

static bool bVLMatrixPushed = false;

void VideoLayer_PushMatrix(void)
{
	VIDEO_FUNCTION_START
	if (bVLMatrixPushed)
		return;

	glPushMatrix();
	bVLMatrixPushed = true;
	VIDEO_FUNCTION_END
}

void VideoLayer_PopMatrix(void)
{
	VIDEO_FUNCTION_START
	if (!bVLMatrixPushed)
		return;
	
	glPopMatrix();
	bVLMatrixPushed = false;
	VIDEO_FUNCTION_END
}

/*===========================
	OPENGL TEXTURES
===========================*/

unsigned int VideoLayer_TranslateFormat(VideoTextureFormat_t Format)
{
	switch (Format)
	{
	case VIDEO_TEXTURE_FORMAT_BGR:
		return GL_BGR;
	case VIDEO_TEXTURE_FORMAT_BGRA:
		return GL_BGRA;
	case VIDEO_TEXTURE_FORMAT_LUMINANCE:
		return GL_LUMINANCE;
	case VIDEO_TEXTURE_FORMAT_RGB:
		return GL_RGB;
	case VIDEO_TEXTURE_FORMAT_RGBA:
		return GL_RGBA;
	default:
		Sys_Error("Unknown texture format! (%i)\n", Format);
	}

	// Won't be hit but meh, compiler will complain otherwise.
	return 0;
}

void VideoLayer_SetupTexture(VideoTextureFormat_t InternalFormat, VideoTextureFormat_t Format, unsigned int Width, unsigned int Height)
{
	VIDEO_FUNCTION_START
	glTexImage2D(GL_TEXTURE_2D, 0, 
		VideoLayer_TranslateFormat(InternalFormat),
		Width, Height, 0, 
		VideoLayer_TranslateFormat(Format), 
		GL_UNSIGNED_BYTE, NULL);
	VIDEO_FUNCTION_END
}

/*	TODO:
		Modify this so it works as a replacement for TexMgr_SetFilterModes.
*/
void VideoLayer_SetTextureFilter(VideoTextureFilter_t FilterMode)
{
	VIDEO_FUNCTION_START
	unsigned int SetFilter = 0;

	switch (FilterMode)
	{
	case VIDEO_TEXTURE_FILTER_LINEAR:
		SetFilter = GL_LINEAR;
		break;
	case VIDEO_TEXTURE_FILTER_NEAREST:
		SetFilter = GL_NEAREST;
		break;
	default:
		Sys_Error("Unknown texture filter type! (%i)\n", FilterMode);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, SetFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, SetFilter);
	VIDEO_FUNCTION_END
}

int VideoLayer_TranslateTextureEnvironmentMode(VideoTextureEnvironmentMode_t TextureEnvironmentMode)
{
	VIDEO_FUNCTION_START
	switch (TextureEnvironmentMode)
	{
	case VIDEO_TEXTURE_MODE_ADD:
		return GL_ADD;
	case VIDEO_TEXTURE_MODE_MODULATE:
		return GL_MODULATE;
	case VIDEO_TEXTURE_MODE_DECAL:
		return GL_DECAL;
	case VIDEO_TEXTURE_MODE_BLEND:
		return GL_BLEND;
	case VIDEO_TEXTURE_MODE_REPLACE:
		return GL_REPLACE;
	case VIDEO_TEXTURE_MODE_COMBINE:
		return GL_COMBINE;
	default:
		Sys_Error("Unknown texture environment mode! (%i)\n", TextureEnvironmentMode);
	}

	// Won't be hit but meh, compiler will complain otherwise.
	return 0;
	VIDEO_FUNCTION_END
}

void VideoLayer_SetTextureEnvironmentMode(VideoTextureEnvironmentMode_t TextureEnvironmentMode)
{
	VIDEO_FUNCTION_START
	// Ensure there's actually been a change.
	if (Video.TextureUnits[Video.uiActiveUnit].CurrentTexEnvMode == TextureEnvironmentMode)
		return;

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, 
		VideoLayer_TranslateTextureEnvironmentMode(TextureEnvironmentMode));

	Video.TextureUnits[Video.uiActiveUnit].CurrentTexEnvMode = TextureEnvironmentMode;
	VIDEO_FUNCTION_END
}

/*===========================
	OPENGL CAPABILITIES
===========================*/

typedef struct
{
	unsigned int uiFirst, uiSecond;

	const char *ccIdentifier;
} VideoLayerCapabilities_t;

VideoLayerCapabilities_t vlcCapabilityList[] =
{
	{ VIDEO_ALPHA_TEST, GL_ALPHA_TEST, "ALPHA_TEST" },
	{ VIDEO_BLEND, GL_BLEND, "BLEND" },
	{ VIDEO_DEPTH_TEST, GL_DEPTH_TEST, "DEPTH_TEST" },
	{ VIDEO_TEXTURE_2D, GL_TEXTURE_2D, "TEXTURE_2D" },
	{ VIDEO_TEXTURE_GEN_S, GL_TEXTURE_GEN_S, "TEXTURE_GEN_S" },
	{ VIDEO_TEXTURE_GEN_T, GL_TEXTURE_GEN_T, "TEXTURE_GEN_T" },
	{ VIDEO_CULL_FACE, GL_CULL_FACE, "CULL_FACE" },
	{ VIDEO_STENCIL_TEST, GL_STENCIL_TEST, "STENCIL_TEST" },
	{ VIDEO_NORMALIZE, GL_NORMALIZE, "NORMALIZE" },
	{ VIDEO_MULTISAMPLE, GL_MULTISAMPLE, "MULTISAMPLE" },

	{ 0 }
};

/*	Enables video capabilities.
*/
void VideoLayer_Enable(unsigned int uiCapabilities)
{
	VIDEO_FUNCTION_START
	int i;
	for (i = 0; i < sizeof(vlcCapabilityList); i++)
	{
		// Check if we reached the end of the list yet.
		if (!vlcCapabilityList[i].uiFirst)
			break;

		if (uiCapabilities & VIDEO_TEXTURE_2D)
			Video.bUnitState[Video.uiActiveUnit] = true;

		if (uiCapabilities & vlcCapabilityList[i].uiFirst)
		{
			// TODO: Implement debugging support.

			glEnable(vlcCapabilityList[i].uiSecond);
		}
	}
	VIDEO_FUNCTION_END
}

void VideoLayer_Disable(unsigned int uiCapabilities)
{
	VIDEO_FUNCTION_START
	int i;
	for (i = 0; i < sizeof(vlcCapabilityList); i++)
	{
		// Check if we reached the end of the list yet.
		if (!vlcCapabilityList[i].uiFirst)
			break;

		if (uiCapabilities & VIDEO_TEXTURE_2D)
			Video.bUnitState[Video.uiActiveUnit] = false;

		if (uiCapabilities & vlcCapabilityList[i].uiFirst)
		{
			// TODO: Implement debugging support.

			glDisable(vlcCapabilityList[i].uiSecond);
		}
	}
	VIDEO_FUNCTION_END
}

/*===========================
	OPENGL BUFFERS
===========================*/

// VERTEX BUFFER OBJECTS

/*	Generates a single OpenGL buffer.
*/
void VideoLayer_GenerateVertexBuffer(unsigned int *uiBuffer) 
{
	VIDEO_FUNCTION_START
	glGenBuffers(1, uiBuffer);
	VIDEO_FUNCTION_END
}

/*	Deletes a single OpenGL buffer.
*/
void VideoLayer_DeleteVertexBuffer(unsigned int *uiBuffer)
{
	VIDEO_FUNCTION_START
	glDeleteBuffers(1, uiBuffer);
	VIDEO_FUNCTION_END
}

// FRAME BUFFER OBJECTS

void VideoLayer_GenerateFrameBuffer(unsigned int *uiBuffer) 
{
	VIDEO_FUNCTION_START
	glGenFramebuffers(1, uiBuffer);
	VIDEO_FUNCTION_END
}

void VideoLayer_BindFrameBuffer(VideoFBOTarget_t vtTarget, unsigned int uiBuffer)
{
	VIDEO_FUNCTION_START
	unsigned int uiOutTarget;

	// TODO: Get these named up so we can easily debug.
	switch (vtTarget)
	{
	case VIDEO_FBO_DRAW:
		uiOutTarget = GL_DRAW_BUFFER;
		break;
	case VIDEO_FBO_READ:
		uiOutTarget = GL_READ_BUFFER;
		break;
	default:
		uiOutTarget = GL_FRAMEBUFFER;
	}

	glBindFramebuffer(uiOutTarget, uiBuffer);

	// Ensure there weren't any issues.
	unsigned int uiGLError = glGetError();
	if (uiGLError != GL_NO_ERROR)
	{
		char *cErrorString = "";
		switch (uiGLError)
		{
		case GL_INVALID_ENUM:
			cErrorString = "Invalid framebuffer target!";
			break;
		case GL_INVALID_OPERATION:
			cErrorString = "Invalid framebuffer object!";
			break;
		default:
			// This should *NEVER* occur.
			break;
		}
		Sys_Error("%s\n%s", VideoLayer_GetErrorMessage(uiGLError), cErrorString);
	}
	VIDEO_FUNCTION_END
}

void VideoLayer_DeleteFrameBuffer(unsigned int *uiBuffer)
{
	VIDEO_FUNCTION_START
	glDeleteFramebuffers(1, uiBuffer);
	VIDEO_FUNCTION_END
}

void VideoLayer_AttachFrameBufferTexture(gltexture_t *Texture)
{
	VIDEO_FUNCTION_START
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture->texnum, 0);
	VIDEO_FUNCTION_END
}

