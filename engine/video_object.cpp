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

void VideoObject_Setup(VideoObject_t *object)
{
	// Create the vertex array object.
	vlGenerateVertexArray(&object->object_vertexarrays);
	vlBindVertexArray(object->object_vertexarrays);

	// Create each of the buffers.
	vlGenerateVertexBuffer(&object->buffer_vertex);
	vlGenerateVertexBuffer(&object->buffer_colour);
	vlGenerateVertexBuffer(&object->buffer_texture);
}

void VideoObject_Destroy(VideoObject_t *object)
{
	vlDeleteVertexBuffer(&object->buffer_colour);
	vlDeleteVertexBuffer(&object->buffer_vertex);
	vlDeleteVertexBuffer(&object->buffer_texture);

	glDeleteVertexArrays(1, &object->object_vertexarrays);
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

void VideoObject_End(VideoObject_t *object)
{
	VIDEO_FUNCTION_START
	vlBindBuffer(GL_ARRAY_BUFFER, object->buffer_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(object->vertices), object->vertices, GL_STATIC_DRAW);
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

void VideoObject_Draw(VideoObject_t *object)
{
	glEnableVertexAttribArray(0);

	vlBindBuffer(GL_ARRAY_BUFFER, object->buffer_vertex);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	vlDrawArrays(object->primitive, 0, object->numtriangles);

	glDisableVertexAttribArray(0);
}

/*	Draw object using immediate mode.
*/
void VideoObject_DrawImmediate(VideoObject_t *object)
{
	VIDEO_FUNCTION_START
	if (object->numverts == 0)
		return;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	VideoVertex_t *vert = &object->vertices[0];
	glVertexPointer(3, GL_FLOAT, sizeof(VideoVertex_t), vert->mvPosition);
	glColorPointer(4, GL_FLOAT, sizeof(VideoVertex_t), vert->mvColour);
	glNormalPointer(GL_FLOAT, sizeof(VideoVertex_t), vert->mvNormal);
	for (int i = 0; i < VIDEO_MAX_UNITS; i++)
		if (Video.textureunit_state[i])
		{
			glClientActiveTexture(Video_GetTextureUnit(i));
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, sizeof(VideoVertex_t), vert->mvST[i]);
		}

	if (object->primitive == VIDEO_PRIMITIVE_TRIANGLES)
		vlDrawElements(
			object->primitive,
			object->numtriangles * 3,
			GL_UNSIGNED_BYTE,
			object->indices
		);
	else
		vlDrawArrays(object->primitive, 0, object->numverts);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	for (int i = 0; i < VIDEO_MAX_UNITS; i++)
		if (Video.textureunit_state[i])
		{
			glClientActiveTexture(Video_GetTextureUnit(i));
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	VIDEO_FUNCTION_END
}
