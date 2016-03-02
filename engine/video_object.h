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

#pragma once

plEXTERN_C_START

void VideoObject_Begin(VideoObject_t *object, VideoPrimitive_t primitive);
void VideoObject_Vertex3f(VideoObject_t *object, float x, float y, float z);
void VideoObject_Normal(VideoObject_t *object, float x, float y, float z);
void VideoObject_Colour4f(VideoObject_t *object, float r, float g, float b, float a);
void VideoObject_Colour4fv(VideoObject_t *object, MathVector4f_t colour);
void VideoObject_End(VideoObject_t *object);
void VideoObject_Clip(VideoObject_t *object, MathVector4f_t mvClipDimensions);

void VideoObject_Draw(VideoObject_t *object);
void VideoObject_DrawImmediate(VideoObject_t *object);

void VideoObject_CalculateLighting(VideoObject_t *object, DynamicLight_t *light, MathVector3f_t position);

plEXTERN_C_END