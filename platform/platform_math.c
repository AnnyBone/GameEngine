/*	Copyright (C) 1996-2001 Id Software, Inc.
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

#include "platform.h"

/*
	Math Library
*/

plVector2f_t pl_origin2f = { 0, 0 };
plVector3f_t pl_origin3f = { 0, 0, 0 };
plVector4f_t pl_origin4f	= { 0, 0, 0, 0 };

// Colours
plColour_t pl_white		= { 1, 1, 1, 1 };
plColour_t pl_red		= { 1, 0, 0, 1 };
plColour_t pl_green		= { 0, 1, 0, 1 };
plColour_t pl_blue		= { 0, 0, 1, 1 };

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

float plVectorToYaw(float *vec)
{
	float result;
	if (vec[1] == 0 && vec[0] == 0)
		result = 0;
	else
	{
		result = (float)(atan2(vec[1], vec[0])*180.0f / PL_PI);
		if (result < 0)
			result += 360.0f;
	}

	return result;
}

float Math_AngleMod(float a)
{
	a = (360.0f / 65536)*((int)(a*(65536 / 360.0f)) & 65535);

	return a;
}

float plLengthf(plVector3f_t a)
{
	float l = 0;
	for (int i = 0; i<3; i++)
		l += a[i] * a[i];
	l = sqrtf(l);

	return l;
}

double plLengthd(plVector3d_t a)
{
	double l = 0;
	for (int i = 0; i<3; i++)
		l += a[i] * a[i];
	l = sqrt(l);

	return l;
}

void plVectorNormalizeFast(plVector3f_t vVector)
{
	float y, fNumber;

	fNumber = Math_DotProduct(vVector, vVector);
	if (fNumber != 0.0f)
	{
		*((long*)&y) = 0x5f3759df - ((*(long*)&fNumber) >> 1);
		y = y*(1.5f - (fNumber*0.5f*y*y));

		Math_VectorScale(vVector, y, vVector);
	}
}

void plAngleVectors(plVector3f_t angles, plVector3f_t forward, plVector3f_t right, plVector3f_t up)
{
	float	angle, sr, sp, sy, cr, cp, cy;

	angle = angles[PL_YAW] * ((float)PL_PI * 2 / 360);
	sy = (float)sin(angle);
	cy = (float)cos(angle);
	angle = angles[PL_PITCH] * ((float)PL_PI * 2 / 360);
	sp = (float)sin(angle);
	cp = (float)cos(angle);
	angle = angles[PL_ROLL] * ((float)PL_PI * 2 / 360);
	sr = (float)sin(angle);
	cr = (float)cos(angle);

	if (forward)
	{
		forward[0] = cp*cy;
		forward[1] = cp*sy;
		forward[2] = -sp;
	}
	if (right)
	{
		right[0] = (-1 * sr*sp*cy + -1 * cr*-sy);
		right[1] = (-1 * sr*sp*sy + -1 * cr*cy);
		right[2] = -1 * sr*cp;
	}
	if (up)
	{
		up[0] = (cr*sp*cy + -sr*-sy);
		up[1] = (cr*sp*sy + -sr*cy);
		up[2] = cr*cp;
	}
}

void Math_VectorAngles(const plVector3f_t forward, plVector3f_t angles)
{
	plVector3f_t temp;

	temp[0] = forward[0];
	temp[1] = forward[1];
	temp[2] = 0;

	angles[PITCH] = -atan2f(forward[2], plLengthf(temp)) / (float)PL_PI;
	angles[YAW] = atan2f(forward[1], forward[0]) / (float)PL_PI;
	angles[ROLL] = 0;
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
	Colour
*/

void plColourSetf(plColour_t in, float r, float g, float b, float a)
{
	in[0] = r; in[1] = g; in[2] = b; in[3] = a;
}

/*	Stolen from Quake 2.
*/
float plColourNormalize(plVector3f_t in, plVector3f_t out)
{
	float	max, scale;
	max = in[0];
	if (in[1] > max)
		max = in[1];
	else if (in[2] > max)
		max = in[2];
	if (!max)
		return 0;

	scale = 1.0f / max;
	Math_VectorScale(in, scale, out);
	return max;
}

/*
	Interpolation
	This is borrowed from http://probesys.blogspot.co.uk/
*/

float plLinear(float x)
{
	if (x < 0)
		return 0;
	if (x > 1.0f)
		return 1.0f;

	return x;
}

float plInPow(float x, float p)
{
	if (x < 0)
		return 0;
	if (x > 1.0f)
		return 1.0f;

	return powf(x, p);
}

float plOutPow(float x, float p)
{
	int	iSign;

	if (x < 0)
		return 0;
	if (x > 1.0f)
		return 1.0f;

	iSign = (int)p % 2 == 0 ? -1 : 1;
	return (iSign * (powf(x - 1.0f, p) + iSign));
}

float Math_InOutPow(float x, float p)
{
	int iSign;

	if (x < 0)
		return 0;
	if (x > 1.0f)
		return 1.0f;

	iSign = (int)p % 2 == 0 ? -1 : 1;
	return (iSign / 2.0f * (powf(x - 2.0f, p) + iSign * 2.0f));
}

float Math_InSin(float x)
{
	if (x < 0)
		return 0;
	if (x > 1.0f)
		return 1.0f;

	return -cosf(x * ((float)PL_PI / 2.0f)) + 1.0f;
}

float Math_OutSin(float x)
{
	if (x < 0)
		return 0;
	if (x > 1.0f)
		return 1.0f;

	return sinf(x * ((float)PL_PI / 2.0f));
}

float Math_InOutSin(float x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return -0.5f * (cosf((float)PL_PI * x) - 1.0f);
}

float plInExp(float x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return powf(2.0f, 10.0f * (x - 1.0f));
}

float plOutExp(float x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return -powf(2.0f, -1.0f * x) + 1.0f;
}

float plInOutExp(float x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return x < 0.5f ? 0.5f * powf(2.0f, 10.0f * (2.0f*x - 1.0f)) : 
		0.5f * (-powf(2.0f, 10.0f * (-2.0f*x + 1.0f)) + 1.0f);
}

float plInCirc(float x)
{
	if (x < 0)
		return 0;
	if (x > 1.0f)
		return 1.0f;

	return -(sqrtf(1.0f - x *x) - 1.0f);
}

float Math_OutCirc(float x)
{
	if (x < 0)
		return 0;
	if (x > 1.0f)
		return 1.0f;

	return sqrtf(1.0f - (x - 1.0f)*(x - 1.0f));
}

float Math_InOutCirc(float x)
{
	if (x < 0)
		return 0;
	if (x > 1.0f)
		return 1.0f;

	return x < 1.0f ? -0.5f * (sqrtf(1.0f - x*x) - 1.0f) : 
		0.5f * (sqrtf(1.0f - ((1.0f * x) - 2.0f) * ((2.0f * x) - 2.0f)) + 1.0f);
}

float Math_Rebound(float x)
{
	if (x < 0)
		return 0;
	if (x > 1.0f)
		return 1.0f;

	if (x < (1.0f / 2.75f)) return 1.0f - 7.5625f * x * x;
	else if (x < (2.0f / 2.75f)) return 1.0f - (7.5625f * (x - 1.5f / 2.75f) *
		(x - 1.5f / 2.75f) + 0.75f);
	else if (x < (2.5f / 2.75f)) return 1.0f - (7.5625f * (x - 2.25f / 2.75f) *
		(x - 2.25f / 2.75f) + 0.9375f);
	else return 1.0f - (7.5625f * (x - 2.625f / 2.75f) * (x - 2.625f / 2.75f) +
		0.984375f);
}

float Math_InBack(float x)
{
	if (x < 0)
		return 0;
	if (x > 1.0f)
		return 1.0f;

	return x * x * ((1.70158f + 1.0f) * x - 1.70158f);
}

float plOutBack(float x)
{
	if (x < 0)
		return 0;
	if (x > 1.0f)
		return 1.0f;

	return (x - 1.0f) * (x - 1.0f) * ((1.70158f + 1.0f) * (x - 1.0f) + 1.70158f) + 1.0f;
}

float Math_InOutBack(float x)
{
	if (x < 0)
		return 0;
	if (x > 1.0f)
		return 1.0f;

	return x < 0.5f ? 0.5f * (4.0f * x * x * ((2.5949f + 1.0f) * 2.0f * x - 2.5949f)) :
		0.5f * ((2.0f * x - 2.0f) * (2.0f * x - 2.0f) * ((2.5949f + 1.0f) * (2.0f * x - 2.0f) +
		2.5949f) + 2.0f);
}

/*	The variable, k, controls the stretching of the function.
*/
float plImpulse(float x, float k)
{
	float h = k*x;
	return h*expf(1.0f - h);
}

float Math_ExpPulse(float x, float k, float n)
{
	return expf(-k*powf(x, n));
}

/*
	Utility
*/

void plSphereFromBounds(plVector3f_t mins, plVector3f_t maxs, plVector3f_t origin, float *radius)
{
	plVectorAdd3fv(mins, maxs, origin);
	plVectorScalef(origin, 0.5f, origin);

	plVector3f_t temp;
	plVectorSubtract3fv(maxs, origin, temp);

	*radius = plLengthf(temp);
}

/*	turn forward towards side on the plane defined by forward and side
	if angle = 90, the result will be equal to side
	assumes side and forward are perpendicular, and normalized
	to turn away from side, use a negative angle
*/
void plTurnVector(plVector3f_t out, const plVector3f_t forward, const plVector3f_t side, float angle)
{
	float scale_forward, scale_side;

	scale_forward = cosf((float)PL_DEG2RAD(angle));
	scale_side = sinf((float)PL_DEG2RAD(angle));

	out[0] = scale_forward*forward[0] + scale_side*side[0];
	out[1] = scale_forward*forward[1] + scale_side*side[1];
	out[2] = scale_forward*forward[2] + scale_side*side[2];
}

char *plVectorToString(plVector3f_t vector)
{
	static char s[32];
	memset(s, 0, 32 * sizeof(char));
	sprintf(s, "%i %i %i", (int)vector[0], (int)vector[1], (int)vector[2]);
	return s;
}

/*	Check to see if an area is intersecting another area.
*/
bool plIsIntersecting(
	MathVector3f_t mvFirstMins, MathVector3f_t mvFirstMaxs,
	MathVector3f_t mvSecondMins, MathVector3f_t mvSecondMaxs)
{
	if (mvFirstMins[0] > mvSecondMaxs[0] ||
		mvFirstMins[1] > mvSecondMaxs[1] ||
		mvFirstMins[2] > mvSecondMaxs[2] ||
		mvFirstMaxs[0] < mvSecondMins[0] ||
		mvFirstMaxs[1] < mvSecondMins[1] ||
		mvFirstMaxs[2] < mvSecondMins[2])
		return false;

	return true;
}

void plNormalizeAngles(plVector3f_t angles)
{
	while (angles[0] > 360)
		angles[0] -= 360;
	while (angles[0] < 0)
		angles[0] += 360;
	while (angles[1] > 360)
		angles[1] -= 360;
	while (angles[1] < 0)
		angles[1] += 360;
}

float plSnapToEights(float x)
{
	x *= 8.0f;
	if (x > 0) x += 0.5f;
	else x -= 0.5f;
	return 0.125f * (int)x;
}

/*
*/

static float pl_identity[16] =
{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

