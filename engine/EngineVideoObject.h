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

#ifndef __ENGINEVIDEOOBJECT_H__
#define __ENGINEVIDEOOBJECT_H__

#ifdef __cplusplus
namespace VideoSys // temp name...
{

	class CVideoVertex
	{
	public:
		CVideoVertex(float x, float y, float z);
	protected:
	private:
		MathVector3f_t
			mvPosition,
			mvNormal;

		MathVector2f_t mvST[VIDEO_MAX_UNITS];

		Colour_t cColour;
	};

	class CVideoObject
	{
	public:
		CVideoObject(VideoPrimitive_t pPrimitiveType = VIDEO_PRIMITIVE_TRIANGLE_FAN);
		~CVideoObject();

		void Begin();
		void Vertex(float x, float y, float z);
		void Colour(float r, float g, float b, float a);
		void End();

		virtual void Draw();

	protected:
		VideoPrimitive_t pPrimitiveType;

	private:
		unsigned int uiVertexBuffer;
		unsigned int uiColourBuffer;
		unsigned int uiTextureBuffer;

		CVideoVertex *VertexList;

		unsigned int uiVertices;
	};

}
#endif

void VideoObject_Begin(VideoObject_t *voObject, VideoPrimitive_t vpPrimitive);
void VideoObject_Vertex(VideoObject_t *voObject, float x, float y, float z);
void VideoObject_VertexVector(VideoObject_t *oObject, MathVector3f_t mvVertex);
void VideoObject_Normal(VideoObject_t *voObject, float x, float y, float z);
void VideoObject_Colour(VideoObject_t *voObject, float r, float g, float b, float a);
void VideoObject_ColourVector(VideoObject_t *voObject, MathVector4f_t mvColour);
void VideoObject_End(VideoObject_t *voObject);
void VideoObject_Clip(VideoObject_t *voObject, MathVector4f_t mvClipDimensions);
void VideoObject_Draw(VideoObject_t *voObject);

#endif