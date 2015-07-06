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

#ifndef __ENGINEVIDEOOBJECT__
#define __ENGINEVIDEOOBJECT__

void VideoObject_Begin(VideoObject_t *voObject, VideoPrimitive_t vpPrimitive);
void VideoObject_Vertex(VideoObject_t *voObject, float x, float y, float z);
void VideoObject_VertexVector(VideoObject_t *oObject, MathVector3_t mvVertex);
void VideoObject_Normal(VideoObject_t *voObject, float x, float y, float z);
void VideoObject_Colour(VideoObject_t *voObject, float r, float g, float b, float a);
void VideoObject_ColourVector(VideoObject_t *voObject, MathVector4_t mvColour);
void VideoObject_End(VideoObject_t *voObject);
void VideoObject_Clip(VideoObject_t *voObject, MathVector4_t mvClipDimensions);
void VideoObject_Draw(VideoObject_t *voObject);

#endif