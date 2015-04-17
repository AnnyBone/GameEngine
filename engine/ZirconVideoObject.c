/*	Copyright (C) 2011-2015 OldTimes Software
	
	Zircon Game Framework (ZGF)
*/

#include "quakedef.h"

#include "video.h"

VideoObjectX_t *VideoObject_Create(void)
{
	VideoObjectX_t *voNewObject;

	voNewObject = (VideoObjectX_t*)malloc(sizeof(VideoObjectX_t));

	// TODO: Allocate...

	voNewObject->uiVertexBuffer = VideoLayer_GenerateBuffer();
	voNewObject->uiColourBuffer = VideoLayer_GenerateBuffer();
	voNewObject->uiTextureBuffer = VideoLayer_GenerateBuffer();

	return NULL;
}

void VideoObject_Delete(VideoObjectX_t *voObject)
{
	VideoLayer_DeleteBuffer(voObject->uiVertexBuffer);
	VideoLayer_DeleteBuffer(voObject->uiColourBuffer);
	VideoLayer_DeleteBuffer(voObject->uiTextureBuffer);
}

/*
	Traditional style interface
*/

void VideoObject_Begin(VideoObjectX_t *voObject, VideoPrimitive_t vpPrimitive)
{
}

void VideoObject_End(VideoObjectX_t *voObject)
{
}

void VideoObject_Vertex(VideoObjectX_t *voObject, float x, float y, float z)
{
	voObject->iVertices++;

	// TODO: Add new vertex to list.

	voObject->ovVertices[voObject->iVertices].mvPosition[0] = x;
	voObject->ovVertices[voObject->iVertices].mvPosition[1] = y;
	voObject->ovVertices[voObject->iVertices].mvPosition[2] = z;
}

void VideoObject_VertexVector(VideoObjectX_t *oObject, MathVector3_t mvVertex)
{
	VideoObject_Vertex(oObject, mvVertex[0], mvVertex[1], mvVertex[2]);
}

void VideoObject_Normal(VideoObjectX_t *voObject, float x, float y, float z)
{
	voObject->ovVertices[voObject->iVertices].mvNormal[0] = x;
	voObject->ovVertices[voObject->iVertices].mvNormal[1] = y;
	voObject->ovVertices[voObject->iVertices].mvNormal[2] = z;
}

void VideoObject_Colour(VideoObjectX_t *voObject, float r, float g, float b, float a)
{
	voObject->ovVertices[voObject->iVertices].mvColour[0] = r;
	voObject->ovVertices[voObject->iVertices].mvColour[1] = g;
	voObject->ovVertices[voObject->iVertices].mvColour[2] = b;
	voObject->ovVertices[voObject->iVertices].mvColour[3] = a;
}

void VideoObject_ColourVector(VideoObjectX_t *voObject, MathVector4_t mvColour)
{
	VideoObject_Colour(voObject, mvColour[0], mvColour[1], mvColour[2], mvColour[3]);
}

/*
	Clipping
*/

void VideoObject_Clip(VideoObjectX_t *voObject, MathVector4_t mvClipDimensions)
{
}

/*
	Rendering
*/

void VideoObject_Draw(VideoObjectX_t *voObject)
{
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

	Video_DrawArrays(voObject->vpPrimitiveType, voObject->iVertices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}