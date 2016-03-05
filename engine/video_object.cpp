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

#if 0

using namespace video;

DrawObject::DrawObject()
{
	indices		= nullptr;
	buffers		= nullptr;
	primitive	= VIDEO_PRIMITIVE_IGNORE;

	num_triangles = 0;

	current_vertex = nullptr;
}

void DrawObject::Begin(VideoPrimitive_t mode)
{
	if ((mode <= VIDEO_PRIMITIVE_IGNORE) || (mode >= VIDEO_PRIMITIVE_END))
		Sys_Error("Invalid primitive mode for object!\n");

	// Clear the vectors, shrink so we can free mem.
	Clear();

	// and then reserve.
	vertices.reserve(4);
}

void DrawObject::Vertex(float x, float y, float z)
{
	VideoVertex_t *vertex = new VideoVertex_t;
	current_vertex = vertex;
	vertices.push_back(vertex);
}

void DrawObject::Normal(float x, float y, float z)
{
	if (!current_vertex)
		Sys_Error("Invalid vertex for video object!\n");
	current_vertex->mvNormal[0] = x;
	current_vertex->mvNormal[0] = y;
	current_vertex->mvNormal[0] = z;
}

void DrawObject::TexCoord(float s, float t)
{
	if (!current_vertex)
		Sys_Error("Invalid vertex for video object!\n");
	current_vertex->mvST[0][0] = s;
	current_vertex->mvST[0][1] = t;
}

void DrawObject::End()
{
	// TODO: Upload it to the GPU
}

void DrawObject::Clear()
{
	vertices.clear();
	vertices.shrink_to_fit();
}

#endif

/*
	C Interface
*/

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

#ifdef KATANA_CORE_GLIDE
#else
	glDeleteVertexArrays(1, &object->object_vertexarrays);
#endif
}

void VideoObject_Begin(VideoObject_t *object, VideoPrimitive_t primitive)
{
	object->primitive = primitive;
}

void VideoObject_AddVertex(VideoObject_t *object)
{

}

void VideoObject_Vertex3f(VideoObject_t *object, float x, float y, float z)
{
	VIDEO_FUNCTION_START
	object->numverts++;

	// TODO: Add new vertex to list.

	object->vertices[object->numverts].mvPosition[0] = x;
	object->vertices[object->numverts].mvPosition[1] = y;
	object->vertices[object->numverts].mvPosition[2] = z;
	VIDEO_FUNCTION_END
}

void VideoObject_Normal(VideoObject_t *object, float x, float y, float z)
{
	VIDEO_FUNCTION_START
	object->vertices[object->numverts].mvNormal[0] = x;
	object->vertices[object->numverts].mvNormal[1] = y;
	object->vertices[object->numverts].mvNormal[2] = z;
	VIDEO_FUNCTION_END
}

void VideoObject_Colour4f(VideoObject_t *voObject, float r, float g, float b, float a)
{
	VIDEO_FUNCTION_START
	voObject->vertices[voObject->numverts].mvColour[0] = r;
	voObject->vertices[voObject->numverts].mvColour[1] = g;
	voObject->vertices[voObject->numverts].mvColour[2] = b;
	voObject->vertices[voObject->numverts].mvColour[3] = a;
	VIDEO_FUNCTION_END
}

void VideoObject_Colour4fv(VideoObject_t *voObject, MathVector4f_t mvColour)
{
	VIDEO_FUNCTION_START
	VideoObject_Colour4f(voObject, mvColour[0], mvColour[1], mvColour[2], mvColour[3]);
	VIDEO_FUNCTION_END
}

void VideoObject_End(VideoObject_t *object)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
#else
	vlBindBuffer(GL_ARRAY_BUFFER, object->buffer_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(object->vertices), object->vertices, GL_STATIC_DRAW);
#endif
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
	Lighting
*/

void voDrawVertexNormals(VideoObject_t *object)
{
	if (!cv_video_shownormals.bValue || (object->primitive == VIDEO_PRIMITIVE_LINES))
		return;

	for (unsigned int i = 0; i < object->numverts; i++)
	{
		MathVector3f_t endpos;
		plVectorClear3fv(endpos);
		plVectorScale3fv(object->vertices[i].mvNormal, 2.0f, endpos);
		plVectorAdd3fv(endpos, object->vertices[i].mvPosition, endpos);

		Draw_Line(object->vertices[i].mvPosition, endpos);
	}
}

void VideoObject_CalculateLighting(VideoObject_t *object, DynamicLight_t *light, MathVector3f_t position)
{
	// Calculate the distance.
	MathVector3f_t distvec;
	Math_VectorSubtract(position, light->origin, distvec);
	float distance = (light->radius - plLengthf(distvec)) / 100.0f;

	for (unsigned int i = 0; i < object->numverts; i++)
	{
		float x = object->vertices[i].mvNormal[0];
		float y = object->vertices[i].mvNormal[1];
		float z = object->vertices[i].mvNormal[2];

		float angle = (distance*((x * distvec[0]) + (y * distvec[1]) + (z * distvec[2])));
		if (angle < 0)
			plVectorClear3fv(object->vertices[i].mvColour);
		else
		{
			object->vertices[i].mvColour[pRED]		= light->color[pRED] * angle;
			object->vertices[i].mvColour[pGREEN]	= light->color[pGREEN] * angle;
			object->vertices[i].mvColour[pBLUE]		= light->color[pBLUE] * angle;
		}

		/*
		x = Object->Vertices_normalStat[count].x;
		y = Object->Vertices_normalStat[count].y;
		z = Object->Vertices_normalStat[count].z;

		angle = (LightDist*((x * Object->Spotlight.x) + (y * Object->Spotlight.y) + (z * Object->Spotlight.z) ));
		if (angle<0 )
		{	
			Object->Vertices_screen[count].r = 0;
			Object->Vertices_screen[count].b = 0;
			Object->Vertices_screen[count].g = 0;
		}
		else
		{	
			Object->Vertices_screen[count].r = Object->Vertices_local[count].r * angle;
			Object->Vertices_screen[count].b = Object->Vertices_local[count].b * angle;
			Object->Vertices_screen[count].g = Object->Vertices_local[count].g * angle;
		}
		*/
	}
}

/*
	Rendering
*/

void VideoObject_Draw(VideoObject_t *object)
{
#ifndef KATANA_CORE_GLIDE
	glEnableVertexAttribArray(0);

	vlBindBuffer(GL_ARRAY_BUFFER, object->buffer_vertex);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	vlDrawArrays(object->primitive, 0, object->numtriangles);

	glDisableVertexAttribArray(0);
#endif
}

/*	Draw object using immediate mode.
*/
void VideoObject_DrawImmediate(VideoObject_t *object)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
#else
	if (object->numverts == 0)
		return;

	voDrawVertexNormals(object);

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
			glClientActiveTexture(vlGetTextureUnit(i));
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
			glClientActiveTexture(vlGetTextureUnit(i));
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
#endif
	VIDEO_FUNCTION_END
}
