/*	Copyright (C) 2011-2016 OldTimes Software

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

#include "engine_base.h"

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

VideoDrawStatic::VideoDrawStatic(VideoPrimitive_t pPrimitiveType)
{
	VIDEO_FUNCTION_START
	this->pPrimitiveType = pPrimitiveType;
	
	uiVertexBuffer = 0;
	uiColourBuffer = 0;
	uiTextureBuffer = 0;

	VertexList = NULL;

	uiVertices = 0;
	VIDEO_FUNCTION_END
}

VideoDrawStatic::~VideoDrawStatic()
{
	VIDEO_FUNCTION_START
	VideoLayer_DeleteVertexBuffer(&uiVertexBuffer);
	VideoLayer_DeleteVertexBuffer(&uiColourBuffer);
	VideoLayer_DeleteVertexBuffer(&uiTextureBuffer);
	VIDEO_FUNCTION_END
}

void VideoDrawStatic::Begin()
{
	VIDEO_FUNCTION_START
	VideoLayer_GenerateVertexBuffer(&uiVertexBuffer);
	VideoLayer_GenerateVertexBuffer(&uiColourBuffer);
	VideoLayer_GenerateVertexBuffer(&uiTextureBuffer);
	VIDEO_FUNCTION_END
}

void VideoDrawStatic::AddVertex(float x, float y, float z)
{
}

void VideoDrawStatic::Colour(float r, float g, float b, float a)
{
}

void VideoDrawStatic::End()
{
	VIDEO_FUNCTION_START
	glBindBuffer(GL_ARRAY_BUFFER, uiVertexBuffer);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uiColourBuffer);
	glColorPointer(4, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uiTextureBuffer);
	
	int i;
	for (i = 0; i < VIDEO_MAX_UNITS; i++)
		if (Video.textureunit_state[i])
		{
			glClientActiveTexture(Video_GetTextureUnit(i));

			glTexCoordPointer(2, GL_FLOAT, 0, 0);
		}
	VIDEO_FUNCTION_END
}

void VideoDrawStatic::Draw()
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
		if (Video.textureunit_state[i])
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

VideoObject_t *VideoObject_Create(void)
{
	VideoObject_t *object = new VideoObject_t;
	return object;
}

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
	voObject->numverts++;

	// TODO: Add new vertex to list.

	voObject->vertices[voObject->numverts].mvPosition[0] = x;
	voObject->vertices[voObject->numverts].mvPosition[1] = y;
	voObject->vertices[voObject->numverts].mvPosition[2] = z;
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
	voObject->vertices[voObject->numverts].mvNormal[0] = x;
	voObject->vertices[voObject->numverts].mvNormal[1] = y;
	voObject->vertices[voObject->numverts].mvNormal[2] = z;
	VIDEO_FUNCTION_END
}

void VideoObject_Colour(VideoObject_t *voObject, float r, float g, float b, float a)
{
	VIDEO_FUNCTION_START
	voObject->vertices[voObject->numverts].mvColour[0] = r;
	voObject->vertices[voObject->numverts].mvColour[1] = g;
	voObject->vertices[voObject->numverts].mvColour[2] = b;
	voObject->vertices[voObject->numverts].mvColour[3] = a;
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

void VideoObject_EnableDrawState(void)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
}

void VideoObject_DisableDrawState(void)
{
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	for (int i = 0; i < VIDEO_MAX_UNITS; i++)
		if (Video.textureunit_state[i])
		{
			glClientActiveTexture(Video_GetTextureUnit(i));
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
}

void VideoObject_SetupPointers(VideoVertex_t *vobject)
{
	glVertexPointer(3, GL_FLOAT, sizeof(VideoVertex_t), vobject->mvPosition);
	glColorPointer(4, GL_FLOAT, sizeof(VideoVertex_t), vobject->mvColour);
	glNormalPointer(GL_FLOAT, sizeof(VideoVertex_t), vobject->mvNormal);

	for (int i = 0; i < VIDEO_MAX_UNITS; i++)
		if (Video.textureunit_state[i])
		{
			glClientActiveTexture(Video_GetTextureUnit(i));
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, sizeof(VideoVertex_t), vobject->mvST[i]);
		}
}

void VideoObject_Draw(VideoObject_t *object)
{
	VIDEO_FUNCTION_START

	VideoObject_EnableDrawState();

	glBindBuffer(GL_ARRAY_BUFFER, object->buffer_vertex);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, object->buffer_colour);
	glColorPointer(4, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, object->buffer_texture);

	for (int i = 0; i < VIDEO_MAX_UNITS; i++)
		if (Video.textureunit_state[i])
		{
			glClientActiveTexture(Video_GetTextureUnit(i));
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, 0);
		}

	VideoLayer_DrawArrays(object->primitive, object->numverts, r_showtris.bValue);

	VideoObject_DisableDrawState();

	VIDEO_FUNCTION_END
}
