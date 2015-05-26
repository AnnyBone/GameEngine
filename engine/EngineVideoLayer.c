/*	Copyright (C) 2011-2015 OldTimes Software
*/

#include "EngineBase.h"

/*
	This document acts as a layer between the video sub-system
	and OpenGL. All OpenGL functionality should be here.
*/

#include "EngineVideo.h"

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
unsigned int VideoLayer_GenerateVertexBuffer(void) 
{
	VIDEO_FUNCTION_START(VideoLayer_GenerateVertexBuffer)
		unsigned int uiBuffer;
		glGenBuffers(1, &uiBuffer);
		return uiBuffer;
	VIDEO_FUNCTION_END
}

// FRAME BUFFER OBJECTS

unsigned int VideoLayer_GenerateFrameBuffer(void) 
{
	VIDEO_FUNCTION_START(VideoLayer_GenerateFrameBuffer)
		unsigned int uiBuffer;
		glGenFramebuffers(1, &uiBuffer);
		return uiBuffer;
	VIDEO_FUNCTION_END
}

/**/

/*	Deletes a single OpenGL buffer.
*/
void VideoLayer_DeleteBuffer(unsigned int uiBuffer) 
{
	VIDEO_FUNCTION_START(VideoLayer_DeleteBuffer)
		glDeleteBuffers(1, &uiBuffer);
	VIDEO_FUNCTION_END
}