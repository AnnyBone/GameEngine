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

extern "C" {
#include "EngineBase.h"

#include "EngineVideo.h"
#include "EngineVideoObject.h"
}

class CVideoVertex
{
public:
	CVideoVertex(float x, float y, float z);
protected:
private:
	float x, y, z;
};

CVideoVertex::CVideoVertex(float x, float y, float z)
{
	this->x = x; 
	this->y = y; 
	this->z = z;
}

//

class CVideoObject
{
public:
	CVideoObject(VideoPrimitive_t primitiveType);
	~CVideoObject();

	void Begin();
	void Vertex(float x, float y, float z);
	void Colour(float r, float g, float b, float a);
	void End();
	void Draw();

protected:
	VideoPrimitive_t primitiveType;

private:
	unsigned int vertexBuffer;
	unsigned int colourBuffer;
	unsigned int textureBuffer;
};

CVideoObject::CVideoObject(VideoPrimitive_t primitiveType)
{
	VIDEO_FUNCTION_START(CVideoObject)

		this->primitiveType = primitiveType;

		vertexBuffer = VideoLayer_GenerateVertexBuffer();
		colourBuffer = VideoLayer_GenerateVertexBuffer();
		textureBuffer = VideoLayer_GenerateVertexBuffer();

	VIDEO_FUNCTION_END
}

CVideoObject::~CVideoObject()
{
	VIDEO_FUNCTION_START(~CVideoObject)

		VideoLayer_DeleteBuffer(vertexBuffer);
		VideoLayer_DeleteBuffer(colourBuffer);
		VideoLayer_DeleteBuffer(textureBuffer);

	VIDEO_FUNCTION_END
}

void CVideoObject::Begin()
{
}

void CVideoObject::Vertex(float x, float y, float z)
{
}

void CVideoObject::Colour(float r, float g, float b, float a)
{
}

void CVideoObject::End()
{
}

void CVideoObject::Draw()
{
	VIDEO_FUNCTION_START(VideoObject_Draw)

		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glEnableClientState(GL_COLOR_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
		glColorPointer(4, GL_FLOAT, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);

		int i;
		for (i = 0; i < VIDEO_MAX_UNITS; i++)
			if (Video.bUnitState[i])
			{
				glClientActiveTexture(Video_GetTextureUnit(i));
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2, GL_FLOAT, 0, 0);
			}

		Video_DrawArrays(primitiveType, 0 /*replace me*/, r_showtris.bValue);

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
	VIDEO_FUNCTION_START(VideoObject_Begin)
	VIDEO_FUNCTION_END
}

void VideoObject_Vertex(VideoObject_t *voObject, float x, float y, float z)
{
	VIDEO_FUNCTION_START(VideoObject_Vertex)

		voObject->iVertices++;

		// TODO: Add new vertex to list.

		voObject->ovVertices[voObject->iVertices].mvPosition[0] = x;
		voObject->ovVertices[voObject->iVertices].mvPosition[1] = y;
		voObject->ovVertices[voObject->iVertices].mvPosition[2] = z;

	VIDEO_FUNCTION_END
}

void VideoObject_VertexVector(VideoObject_t *oObject, MathVector3_t mvVertex)
{
	VIDEO_FUNCTION_START(VideoObject_VertexVector)

		VideoObject_Vertex(oObject, mvVertex[0], mvVertex[1], mvVertex[2]);

	VIDEO_FUNCTION_END
}

void VideoObject_Normal(VideoObject_t *voObject, float x, float y, float z)
{
	VIDEO_FUNCTION_START(VideoObject_Normal)

		voObject->ovVertices[voObject->iVertices].mvNormal[0] = x;
		voObject->ovVertices[voObject->iVertices].mvNormal[1] = y;
		voObject->ovVertices[voObject->iVertices].mvNormal[2] = z;

	VIDEO_FUNCTION_END
}

void VideoObject_Colour(VideoObject_t *voObject, float r, float g, float b, float a)
{
	VIDEO_FUNCTION_START(VideoObject_Colour)

		voObject->ovVertices[voObject->iVertices].mvColour[0] = r;
		voObject->ovVertices[voObject->iVertices].mvColour[1] = g;
		voObject->ovVertices[voObject->iVertices].mvColour[2] = b;
		voObject->ovVertices[voObject->iVertices].mvColour[3] = a;

	VIDEO_FUNCTION_END
}

void VideoObject_ColourVector(VideoObject_t *voObject, MathVector4_t mvColour)
{
	VIDEO_FUNCTION_START(VideoObject_ColourVector)

		VideoObject_Colour(voObject, mvColour[0], mvColour[1], mvColour[2], mvColour[3]);

	VIDEO_FUNCTION_END
}

void VideoObject_End(VideoObject_t *voObject)
{
	VIDEO_FUNCTION_START(VideoObject_End)
	VIDEO_FUNCTION_END
}

/*
	Clipping
*/

void VideoObject_Clip(VideoObject_t *voObject, MathVector4_t mvClipDimensions)
{
	VIDEO_FUNCTION_START(VideoObject_Clip)
	VIDEO_FUNCTION_END
}

/*
	Rendering
*/

void VideoObject_Draw(VideoObject_t *voObject)
{
	VIDEO_FUNCTION_START(VideoObject_Draw)

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

		Video_DrawArrays(voObject->vpPrimitiveType, voObject->iVertices, r_showtris.bValue);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	VIDEO_FUNCTION_END
}
