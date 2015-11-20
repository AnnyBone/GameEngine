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

#include "EngineBase.h"

#include "video.h"

/*
	Vertex
*/

VideoVertex::VideoVertex(float x, float y, float z)
{
	mvPosition[0] = x; 
	mvPosition[1] = y; 
	mvPosition[2] = z;

	Math_VectorClear(mvNormal);

	Math_Vector4Set(1.0f, cColour);
}

/*
	Object
*/

VideoVertexObject::VideoVertexObject(VideoPrimitive_t pPrimitiveType)
{
	VIDEO_FUNCTION_START
	this->pPrimitiveType = pPrimitiveType;
	
	uiVertexBuffer = 0;
	uiColourBuffer = 0;
	uiTextureBuffer = 0;

	VertexList = NULL;

	uiVertices = 0;

	VideoLayer_GenerateVertexBuffer(&uiVertexBuffer);
	VideoLayer_GenerateVertexBuffer(&uiColourBuffer);
	VideoLayer_GenerateVertexBuffer(&uiTextureBuffer);
	VIDEO_FUNCTION_END
}

VideoVertexObject::~VideoVertexObject()
{
	VIDEO_FUNCTION_START
	VideoLayer_DeleteVertexBuffer(&uiVertexBuffer);
	VideoLayer_DeleteVertexBuffer(&uiColourBuffer);
	VideoLayer_DeleteVertexBuffer(&uiTextureBuffer);
	VIDEO_FUNCTION_END
}

void VideoVertexObject::Begin()
{
}

void VideoVertexObject::AddVertex(float x, float y, float z)
{
}

void VideoVertexObject::Colour(float r, float g, float b, float a)
{
}

void VideoVertexObject::End()
{
	VIDEO_FUNCTION_START
	glBindBuffer(GL_ARRAY_BUFFER, uiVertexBuffer);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uiColourBuffer);
	glColorPointer(4, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uiTextureBuffer);
	
	int i;
	for (i = 0; i < VIDEO_MAX_UNITS; i++)
		if (Video.bUnitState[i])
		{
			glClientActiveTexture(Video_GetTextureUnit(i));

			glTexCoordPointer(2, GL_FLOAT, 0, 0);
		}
	VIDEO_FUNCTION_END
}

void VideoVertexObject::Draw()
{
	VIDEO_FUNCTION_START
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, uiVertexBuffer);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glEnableClientState(GL_COLOR_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, uiColourBuffer);
	glColorPointer(4, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uiTextureBuffer);

	int i;
	for (i = 0; i < VIDEO_MAX_UNITS; i++)
		if (Video.bUnitState[i])
		{
			glClientActiveTexture(Video_GetTextureUnit(i));
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, 0);
		}

	VideoLayer_DrawArrays(pPrimitiveType, uiVertices /*replace me*/, r_showtris.bValue);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	VIDEO_FUNCTION_END
}

/*
	C Wrapper
*/

/*
	Traditional style interface
*/

void VideoObject_Begin(VideoObject_t *voObject, VideoPrimitive_t vpPrimitive)
{
	VIDEO_FUNCTION_START
	VIDEO_FUNCTION_END
}

void VideoObject_Vertex(VideoObject_t *voObject, float x, float y, float z)
{
	VIDEO_FUNCTION_START
	voObject->iVertices++;

	// TODO: Add new vertex to list.

	voObject->ovVertices[voObject->iVertices].mvPosition[0] = x;
	voObject->ovVertices[voObject->iVertices].mvPosition[1] = y;
	voObject->ovVertices[voObject->iVertices].mvPosition[2] = z;
	VIDEO_FUNCTION_END
}

void VideoObject_VertexVector(VideoObject_t *oObject, MathVector3f_t mvVertex)
{
	VIDEO_FUNCTION_START
	VideoObject_Vertex(oObject, mvVertex[0], mvVertex[1], mvVertex[2]);
	VIDEO_FUNCTION_END
}

void VideoObject_Normal(VideoObject_t *voObject, float x, float y, float z)
{
	VIDEO_FUNCTION_START
	voObject->ovVertices[voObject->iVertices].mvNormal[0] = x;
	voObject->ovVertices[voObject->iVertices].mvNormal[1] = y;
	voObject->ovVertices[voObject->iVertices].mvNormal[2] = z;
	VIDEO_FUNCTION_END
}

void VideoObject_Colour(VideoObject_t *voObject, float r, float g, float b, float a)
{
	VIDEO_FUNCTION_START
	voObject->ovVertices[voObject->iVertices].mvColour[0] = r;
	voObject->ovVertices[voObject->iVertices].mvColour[1] = g;
	voObject->ovVertices[voObject->iVertices].mvColour[2] = b;
	voObject->ovVertices[voObject->iVertices].mvColour[3] = a;
	VIDEO_FUNCTION_END
}

void VideoObject_ColourVector(VideoObject_t *voObject, MathVector4f_t mvColour)
{
	VIDEO_FUNCTION_START
	VideoObject_Colour(voObject, mvColour[0], mvColour[1], mvColour[2], mvColour[3]);
	VIDEO_FUNCTION_END
}

void VideoObject_End(VideoObject_t *voObject)
{
	VIDEO_FUNCTION_START
	VIDEO_FUNCTION_END
}

/*
	Clipping
*/

void VideoObject_Clip(VideoObject_t *voObject, MathVector4f_t mvClipDimensions)
{
	VIDEO_FUNCTION_START
	VIDEO_FUNCTION_END
}

/*
	Rendering
*/

void VideoObject_Draw(VideoObject_t *voObject)
{
	VIDEO_FUNCTION_START
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, voObject->uiVertexBuffer);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glEnableClientState(GL_COLOR_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, voObject->uiColourBuffer);
	glColorPointer(4, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, voObject->uiTextureBuffer);

	int i;
	for (i = 0; i < VIDEO_MAX_UNITS; i++)
		if (Video.bUnitState[i])
		{
			glClientActiveTexture(Video_GetTextureUnit(i));
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, 0);
		}

	VideoLayer_DrawArrays(voObject->vpPrimitiveType, voObject->iVertices, r_showtris.bValue);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	VIDEO_FUNCTION_END
}
