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
	C Interface
*/

void VideoObject_Setup(VLdraw *object)
{
	// Create the vertex array object.
	vlGenerateVertexArray(&object->object_vertexarrays);
	vlBindVertexArray(object->object_vertexarrays);

	// Create each of the buffers.
	vlGenerateVertexBuffer(&object->buffer_vertex);
	vlGenerateVertexBuffer(&object->buffer_colour);
	vlGenerateVertexBuffer(&object->buffer_texture);
}

void VideoObject_Destroy(VLdraw *object)
{
	vlDeleteVertexBuffer(&object->buffer_colour);
	vlDeleteVertexBuffer(&object->buffer_vertex);
	vlDeleteVertexBuffer(&object->buffer_texture);

#ifdef VL_MODE_GLIDE
#else
	glDeleteVertexArrays(1, &object->object_vertexarrays);
#endif
}

void VideoObject_Begin(VLdraw *object, VideoPrimitive_t primitive)
{
	object->primitive = primitive;
}

void VideoObject_AddVertex(VLdraw *object)
{

}

void VideoObject_Vertex3f(VLdraw *object, float x, float y, float z)
{
	VIDEO_FUNCTION_START
	object->numverts++;

	// TODO: Add new vertex to list.

	object->vertices[object->numverts].position[0] = x;
	object->vertices[object->numverts].position[1] = y;
	object->vertices[object->numverts].position[2] = z;
	VIDEO_FUNCTION_END
}

void VideoObject_Normal(VLdraw *object, float x, float y, float z)
{
	VIDEO_FUNCTION_START
	object->vertices[object->numverts].normal[0] = x;
	object->vertices[object->numverts].normal[1] = y;
	object->vertices[object->numverts].normal[2] = z;
	VIDEO_FUNCTION_END
}

void VideoObject_Colour4f(VLdraw *voObject, float r, float g, float b, float a)
{
	VIDEO_FUNCTION_START
	voObject->vertices[voObject->numverts].colour[0] = r;
	voObject->vertices[voObject->numverts].colour[1] = g;
	voObject->vertices[voObject->numverts].colour[2] = b;
	voObject->vertices[voObject->numverts].colour[3] = a;
	VIDEO_FUNCTION_END
}

void VideoObject_Colour4fv(VLdraw *voObject, MathVector4f_t mvColour)
{
	VIDEO_FUNCTION_START
	VideoObject_Colour4f(voObject, mvColour[0], mvColour[1], mvColour[2], mvColour[3]);
	VIDEO_FUNCTION_END
}

/*
	Lighting
*/

void voDrawVertexNormals(VLdraw *object)
{
	if (!cv_video_shownormals.bValue || (object->primitive == VIDEO_PRIMITIVE_LINES))
		return;

	for (unsigned int i = 0; i < object->numverts; i++)
	{
		MathVector3f_t endpos;
		plVectorClear3fv(endpos);
		plVectorScale3fv(object->vertices[i].normal, 2.0f, endpos);
		plVectorAdd3fv(endpos, object->vertices[i].position, endpos);

		Draw_Line(object->vertices[i].position, endpos);
	}
}

/*
	Rendering
*/

void VideoObject_Draw(VLdraw *object)
{
#ifndef VL_MODE_GLIDE
	glEnableVertexAttribArray(0);

	vlBindBuffer(GL_ARRAY_BUFFER, object->buffer_vertex);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
	vlDrawArrays(object->primitive, 0, object->numtriangles);

	glDisableVertexAttribArray(0);
#endif
}
