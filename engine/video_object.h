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

#ifndef VIDEO_OBJECT_H
#define VIDEO_OBJECT_H

#ifdef __cplusplus
class VideoVertex
{
public:
	VideoVertex(float x, float y, float z);
protected:
private:
	MathVector3f_t
		mvPosition,
		mvNormal;

	MathVector2f_t mvST[VIDEO_MAX_UNITS];

	Colour_t cColour;
};

class DrawObject
{
public:
	virtual void Begin() = 0;
	virtual void End() = 0;
	virtual void Draw() = 0;

protected:
	VideoPrimitive_t primitive;

	unsigned int vertices;
	unsigned int triangles;
};

class DrawDynamicObject : public DrawObject
{
public:
	DrawDynamicObject(VideoPrimitive_t primitive = VIDEO_PRIMITIVE_TRIANGLE_FAN);
	~DrawDynamicObject();

	virtual void Begin();
	virtual void End();
	virtual void Draw();

protected:
private:
	std::vector<VideoVertex> vertices;
};

class VideoDrawStatic
{
public:
	VideoDrawStatic(VideoPrimitive_t pPrimitiveType = VIDEO_PRIMITIVE_TRIANGLE_FAN);
	~VideoDrawStatic();

	void Begin();
	void AddVertex(float x, float y, float z);
	void Colour(float r, float g, float b, float a = 1.0f);
	void End();

	virtual void Draw();

protected:
	VideoPrimitive_t pPrimitiveType;

private:
	unsigned int uiVertexBuffer;
	unsigned int uiColourBuffer;
	unsigned int uiTextureBuffer;

	VideoVertex *VertexList;

	unsigned int uiVertices;
};
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

#endif // !VIDEO_OBJECT_H