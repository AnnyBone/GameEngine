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
char *VideoLayer_GetErrorMessage(void)
{
	switch (glGetError())
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
	VIDEO_FUNCTION_START(VideoLayer_Enable)
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
	VIDEO_FUNCTION_START(VideoLayer_Disable)
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
	VIDEO_FUNCTION_START(VideoLayer_GenerateVertexBuffer)
		glGenBuffers(1, uiBuffer);
	VIDEO_FUNCTION_END
}

/*	Deletes a single OpenGL buffer.
*/
void VideoLayer_DeleteVertexBuffer(unsigned int *uiBuffer)
{
	VIDEO_FUNCTION_START(VideoLayer_DeleteVertexBuffer)
		glDeleteBuffers(1, uiBuffer);
	VIDEO_FUNCTION_END
}

// FRAME BUFFER OBJECTS

void VideoLayer_GenerateFrameBuffer(unsigned int *uiBuffer) 
{
	VIDEO_FUNCTION_START(VideoLayer_GenerateFrameBuffer)
		glGenFramebuffers(1, uiBuffer);
	VIDEO_FUNCTION_END
}

void VideoLayer_BindFrameBuffer(VideoFBOTarget_t vtTarget, unsigned int uiBuffer)
{
	VIDEO_FUNCTION_START(VideoLayer_BindFrameBuffer)
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
	VIDEO_FUNCTION_END
}

void VideoLayer_DeleteFrameBuffer(unsigned int *uiBuffer)
{
	VIDEO_FUNCTION_START(VideoLayer_DeleteFrameBuffer)
		glDeleteFramebuffers(1, uiBuffer);
	VIDEO_FUNCTION_END
}

