/*	Copyright (C) 2011-2015 OldTimes Software
*/

#include "quakedef.h"
#include "video.h"

typedef struct
{
	MathVector3_t	mvPosition;	// Vertex position.
	MathVector3_t	mvNormal;	// Vertex normal.

	MathVector2_t	mvST[VIDEO_MAX_UNITS];	// Vertex texture coord, per unit.

	MathVector4_t	mvColour;	// Vertex RGBA.
} XObjectVertex_t;

typedef struct
{
	XObjectVertex_t *ovVertices;	// Array of vertices for the object.

	int iVertices;	// Number of vertices.

	VideoPrimitive_t	vpPrimitiveType;
} XObject_t;

XObject_t *Object_Create(void)
{
	return NULL;
}

/*
	Traditional style interface
*/

void Object_Begin(XObject_t *oObject)
{}

void Object_End(XObject_t *oObject)
{}

void Object_Vertex(XObject_t *oObject, float x, float y, float z)
{
	oObject->iVertices++;

	// TODO: Add new vertex to list.

	oObject->ovVertices[oObject->iVertices].mvPosition[0] = x;
	oObject->ovVertices[oObject->iVertices].mvPosition[1] = y;
	oObject->ovVertices[oObject->iVertices].mvPosition[2] = z;
}

void Object_VertexVector(XObject_t *oObject, MathVector3_t mvVertex)
{
	Object_Vertex(oObject, mvVertex[0], mvVertex[1], mvVertex[2]);
}

void Object_Normal(XObject_t *oObject, float x, float y, float z)
{
	oObject->ovVertices[oObject->iVertices].mvNormal[0] = x;
	oObject->ovVertices[oObject->iVertices].mvNormal[1] = y;
	oObject->ovVertices[oObject->iVertices].mvNormal[2] = z;
}

void Object_Colour(XObject_t *oObject, float r, float g, float b, float a)
{
	oObject->ovVertices[oObject->iVertices].mvColour[0] = r;
	oObject->ovVertices[oObject->iVertices].mvColour[1] = g;
	oObject->ovVertices[oObject->iVertices].mvColour[2] = b;
	oObject->ovVertices[oObject->iVertices].mvColour[3] = a;
}

void Object_ColourVector(XObject_t *oObject, MathVector4_t mvColour)
{
	Object_Colour(oObject, mvColour[0], mvColour[1], mvColour[2], mvColour[3]);
}

/*
	Clipping
*/

void Object_Clip(XObject_t *voObject, MathVector4_t mvClipDimensions)
{
}