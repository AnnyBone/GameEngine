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

plEXTERN_C_START

void VideoObject_Vertex(VideoObject_t *object, float x, float y, float z);
void VideoObject_Normal(VideoObject_t *object, float x, float y, float z);
void VideoObject_Colour(VideoObject_t *object, float r, float g, float b, float a);
void VideoObject_ColourVector(VideoObject_t *object, MathVector4f_t mvColour);
void VideoObject_End(VideoObject_t *object);
void VideoObject_Clip(VideoObject_t *object, MathVector4f_t mvClipDimensions);

void VideoObject_Draw(VideoObject_t *object);
void VideoObject_DrawImmediate(VideoObject_t *object);

plEXTERN_C_END

#endif // !VIDEO_OBJECT_H