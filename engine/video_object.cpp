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

void VideoObject_Colour4f(vlDraw_t *voObject, float r, float g, float b, float a)
{
	VIDEO_FUNCTION_START
	voObject->vertices[voObject->numverts].colour[0] = r;
	voObject->vertices[voObject->numverts].colour[1] = g;
	voObject->vertices[voObject->numverts].colour[2] = b;
	voObject->vertices[voObject->numverts].colour[3] = a;
	VIDEO_FUNCTION_END
}

void VideoObject_Colour4fv(vlDraw_t *voObject, MathVector4f_t mvColour)
{
	VIDEO_FUNCTION_START
	VideoObject_Colour4f(voObject, mvColour[0], mvColour[1], mvColour[2], mvColour[3]);
	VIDEO_FUNCTION_END
}
