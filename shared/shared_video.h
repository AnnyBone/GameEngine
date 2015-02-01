/*	Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef __SHAREDVIDEO__
#define	__SHAREDVIDEO__

#define	VIDEO_MAX_UNITS	16

// Video Object
typedef struct
{
	MathVector3_t	vVertex;

	MathVector2_t	vTextureCoord[VIDEO_MAX_UNITS];		// Texture coordinates by texture unit.

	MathVector4_t	vColour;							// RGBA

	MathVector3_t	vNormal;							// Vertex normals.
} VideoObject_t;

#endif