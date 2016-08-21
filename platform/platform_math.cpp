/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others
Copyright (C) 2011-2016 OldTimes Software

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

#include "platform_math.h"

/*	Math Library	*/

plVector2f_t pl_origin2f = { 0, 0 };
plVector3f_t pl_origin3f = { 0, 0, 0 };
plVector4f_t pl_origin4f	= { 0, 0, 0, 0 };

// Colours
plColour_t pl_white		= { 1, 1, 1, 1 };
plColour_t pl_red		= { 1, 0, 0, 1 };
plColour_t pl_green		= { 0, 1, 0, 1 };
plColour_t pl_blue		= { 0, 0, 1, 1 };
plColour_t pl_black		= { 0, 0, 0, 1 };

MathVector_t plVectorToAngles(plVector3f_t vValue)
{
	float			fForward, fYaw, fPitch;
	MathVector_t	mvResult;

	if (vValue[1] == 0 && vValue[0] == 0)
	{
		fYaw = 0;
		if (vValue[2] > 0)
			fPitch = 90.0f;
		else
			fPitch = 270.0f;
	}
	else
	{
		fYaw = (float)(atan2(vValue[1], vValue[0]) * 180 / PL_PI);
		if (fYaw < 0)
			fYaw += 360.0f;

		fForward = (float)sqrt(vValue[0] * vValue[0] + vValue[1] * vValue[1]);
		fPitch = (float)(atan2(vValue[2], fForward) * 180 / PL_PI);
		if (fPitch < 0)
			fPitch += 360.0f;
	}

	mvResult.vX = fPitch;
	mvResult.vY = fYaw;
	mvResult.vZ = 0;

	return mvResult;
}

float Math_DotProduct(plVector3f_t a, plVector3f_t b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void Math_VectorMake(plVector3f_t veca, float scale, plVector3f_t vecb, plVector3f_t vecc)
{
	vecc[0] = veca[0] + scale*vecb[0];
	vecc[1] = veca[1] + scale*vecb[1];
	vecc[2] = veca[2] + scale*vecb[2];
}

/*
	Utility
*/

char *plVectorToString(plVector3f_t vector)
{
	static char s[32];
	memset(s, 0, 32 * sizeof(char));
	sprintf(s, "%i %i %i", (int)vector[0], (int)vector[1], (int)vector[2]);
	return s;
}

////////////////////////////////////////////////////////////////////////

static float pl_identity[16] =
{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

