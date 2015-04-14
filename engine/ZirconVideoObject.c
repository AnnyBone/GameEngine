/*	Copyright (C) 2011-2015 OldTimes Software
	
	Zircon Game Framework (ZGF)
*/

#include "quakedef.h"
#include "video.h"

typedef struct
{
	MathVector3_t mvPosition;				// Vertex position.
	MathVector3_t mvNormal;					// Vertex normal.

	MathVector2_t mvST[VIDEO_MAX_UNITS];	// Vertex texture coord, per unit.

	MathVector4_t mvColour;					// Vertex RGBA.
} VideoObjectVertex_t;

typedef struct
{
	VideoObjectVertex_t *ovVertices;	// Array of vertices for the object.

	int iVertices;						// Number of vertices.

	GLenum ePrimitiveType;

	int iBuffer;
} ZVideoObject_t;

ZVideoObject_t *VideoObject_Create(void)
{
	return NULL;
}

/*
	Traditional style interface
*/

void VideoObject_Begin(ZVideoObject_t *voObject)
{}

void VideoObject_End(ZVideoObject_t *voObject)
{}

void VideoObject_Vertex(ZVideoObject_t *voObject, float x, float y, float z)
{
	voObject->iVertices++;

	// TODO: Add new vertex to list.

	voObject->ovVertices[voObject->iVertices].mvPosition[0] = x;
	voObject->ovVertices[voObject->iVertices].mvPosition[1] = y;
	voObject->ovVertices[voObject->iVertices].mvPosition[2] = z;
}

void VideoObject_VertexVector(ZVideoObject_t *oObject, MathVector3_t mvVertex)
{
	VideoObject_Vertex(oObject, mvVertex[0], mvVertex[1], mvVertex[2]);
}

void VideoObject_Normal(ZVideoObject_t *voObject, float x, float y, float z)
{
	voObject->ovVertices[voObject->iVertices].mvNormal[0] = x;
	voObject->ovVertices[voObject->iVertices].mvNormal[1] = y;
	voObject->ovVertices[voObject->iVertices].mvNormal[2] = z;
}

void VideoObject_Colour(ZVideoObject_t *voObject, float r, float g, float b, float a)
{
	voObject->ovVertices[voObject->iVertices].mvColour[0] = r;
	voObject->ovVertices[voObject->iVertices].mvColour[1] = g;
	voObject->ovVertices[voObject->iVertices].mvColour[2] = b;
	voObject->ovVertices[voObject->iVertices].mvColour[3] = a;
}

void VideoObject_ColourVector(ZVideoObject_t *voObject, MathVector4_t mvColour)
{
	VideoObject_Colour(voObject, mvColour[0], mvColour[1], mvColour[2], mvColour[3]);
}

/*
	Clipping
*/

void VideoObject_Clip(ZVideoObject_t *voObject, MathVector4_t mvClipDimensions)
{
}

/*
	Rendering
*/

void VideoObject_Draw(ZVideoObject_t *voObject)
{
	int i;
	GLenum ePrimitiveType;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, voObject->iBuffer);

	glVertexPointer(3, GL_FLOAT, 0, 0);
	glColorPointer(4, GL_FLOAT, 0, 0);

	for (i = 0; i < VIDEO_MAX_UNITS; i++)
		if (Video.bUnitState[i])
		{
			glClientActiveTexture(Video_GetGLUnit(i));
			glTexCoordPointer(2, GL_FLOAT, 0, 0);
		}

	ePrimitiveType = voObject->ePrimitiveType;

	// Handle wireframe view for different primitive types.
	if (r_showtris.bValue)
	{
		switch (ePrimitiveType)
		{
		case GL_TRIANGLES:
			ePrimitiveType = GL_LINES;
			break;
		case GL_TRIANGLE_FAN:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		}
	}

	glDrawArrays(ePrimitiveType, 0, voObject->iVertices);

	if (r_showtris.bValue && (ePrimitiveType == GL_TRIANGLE_FAN))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}