/*	Copyright (C) 2011-2015 OldTimes Software
*/

#include "quakedef.h"

#include "video.h"

VideoObjectX_t *VideoObject_Create(void)
{
	VIDEO_FUNCTION_START(VideoObject_Create)
		VideoObjectX_t *voNewObject;

		voNewObject = (VideoObjectX_t*)malloc(sizeof(VideoObjectX_t));

		// TODO: Allocate...

		voNewObject->uiVertexBuffer = VideoLayer_GenerateVertexBuffer();
		voNewObject->uiColourBuffer = VideoLayer_GenerateVertexBuffer();
		voNewObject->uiTextureBuffer = VideoLayer_GenerateVertexBuffer();

		return NULL;
	VIDEO_FUNCTION_END
}

void VideoObject_Delete(VideoObjectX_t *voObject)
{
	VIDEO_FUNCTION_START(VideoObject_Delete)
		VideoLayer_DeleteBuffer(voObject->uiVertexBuffer);
		VideoLayer_DeleteBuffer(voObject->uiColourBuffer);
		VideoLayer_DeleteBuffer(voObject->uiTextureBuffer);
	VIDEO_FUNCTION_END
}

/*
	Traditional style interface
*/

void VideoObject_Begin(VideoObjectX_t *voObject, VideoPrimitive_t vpPrimitive)
{
	VIDEO_FUNCTION_START(VideoObject_Begin)
	VIDEO_FUNCTION_END
}

void VideoObject_End(VideoObjectX_t *voObject)
{
	VIDEO_FUNCTION_START(VideoObject_End)
	VIDEO_FUNCTION_END
}

void VideoObject_Vertex(VideoObjectX_t *voObject, float x, float y, float z)
{
	VIDEO_FUNCTION_START(VideoObject_Vertex)
		voObject->iVertices++;

		// TODO: Add new vertex to list.

		voObject->ovVertices[voObject->iVertices].mvPosition[0] = x;
		voObject->ovVertices[voObject->iVertices].mvPosition[1] = y;
		voObject->ovVertices[voObject->iVertices].mvPosition[2] = z;
	VIDEO_FUNCTION_END
}

void VideoObject_VertexVector(VideoObjectX_t *oObject, MathVector3_t mvVertex)
{
	VIDEO_FUNCTION_START(VideoObject_VertexVector)
		VideoObject_Vertex(oObject, mvVertex[0], mvVertex[1], mvVertex[2]);
	VIDEO_FUNCTION_END
}

void VideoObject_Normal(VideoObjectX_t *voObject, float x, float y, float z)
{
	VIDEO_FUNCTION_START(VideoObject_Normal)
		voObject->ovVertices[voObject->iVertices].mvNormal[0] = x;
		voObject->ovVertices[voObject->iVertices].mvNormal[1] = y;
		voObject->ovVertices[voObject->iVertices].mvNormal[2] = z;
	VIDEO_FUNCTION_END
}

void VideoObject_Colour(VideoObjectX_t *voObject, float r, float g, float b, float a)
{
	VIDEO_FUNCTION_START(VideoObject_Colour)
		voObject->ovVertices[voObject->iVertices].mvColour[0] = r;
		voObject->ovVertices[voObject->iVertices].mvColour[1] = g;
		voObject->ovVertices[voObject->iVertices].mvColour[2] = b;
		voObject->ovVertices[voObject->iVertices].mvColour[3] = a;
	VIDEO_FUNCTION_END
}

void VideoObject_ColourVector(VideoObjectX_t *voObject, MathVector4_t mvColour)
{
	VIDEO_FUNCTION_START(VideoObject_ColourVector)
		VideoObject_Colour(voObject, mvColour[0], mvColour[1], mvColour[2], mvColour[3]);
	VIDEO_FUNCTION_END
}

/*
	Clipping
*/

void VideoObject_Clip(VideoObjectX_t *voObject, MathVector4_t mvClipDimensions)
{
	VIDEO_FUNCTION_START(VideoObject_Clip)
	VIDEO_FUNCTION_END
}

/*
	Rendering
*/

void VideoObject_Draw(VideoObjectX_t *voObject)
{
	VIDEO_FUNCTION_START(VideoObject_Draw)
		int i;

		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, voObject->uiVertexBuffer);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glEnableClientState(GL_COLOR_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, voObject->uiColourBuffer);
		glColorPointer(4, GL_FLOAT, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, voObject->uiTextureBuffer);

		for (i = 0; i < VIDEO_MAX_UNITS; i++)
			if (Video.bUnitState[i])
			{
				glClientActiveTexture(Video_GetTextureUnit(i));
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2, GL_FLOAT, 0, 0);
			}

		Video_DrawArrays(voObject->vpPrimitiveType, voObject->iVertices, r_showtris.bValue);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	VIDEO_FUNCTION_END
}