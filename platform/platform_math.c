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

#include "platform_math.h"

/*
	Math Library
*/

MathVector2f_t pl_origin2f	= { 0, 0 };
MathVector3f_t pl_origin3f	= { 0, 0, 0 };
MathVector4f_t pl_origin4f	= { 0, 0, 0, 0 };

// Colours
Colour_t pl_white		= { 1, 1, 1, 1 };
Colour_t pl_red			= { 1, 0, 0, 1 };
Colour_t pl_green		= { 0, 1, 0, 1 };
Colour_t pl_blue		= { 0, 0, 1, 1 };

MathVector_t plVectorToAngles(MathVector3f_t vValue)
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
		fYaw = (float)(atan2(vValue[1], vValue[0]) * 180 / pMath_PI);
		if (fYaw < 0)
			fYaw += 360.0f;

		fForward = (float)sqrt(vValue[0] * vValue[0] + vValue[1] * vValue[1]);
		fPitch = (float)(atan2(vValue[2], fForward) * 180 / pMath_PI);
		if (fPitch < 0)
			fPitch += 360.0f;
	}

	mvResult.vX = fPitch;
	mvResult.vY = fYaw;
	mvResult.vZ = 0;

	return mvResult;
}

float plVectorToYaw(MathVectorf_t *vec)
{
	float result;
	if (vec[1] == 0 && vec[0] == 0)
		result = 0;
	else
	{
		result = (float)(atan2(vec[1], vec[0])*180.0f / pMath_PI);
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

MathVectorf_t plLengthf(MathVector3f_t a)
{
	float l = 0;
	for (int i = 0; i<3; i++)
		l += a[i] * a[i];
	l = sqrtf(l);

	return l;
}

MathVectord_t plLengthd(MathVector3d_t a)
{
	double l = 0;
	for (int i = 0; i<3; i++)
		l += a[i] * a[i];
	l = sqrt(l);

	return l;
}

double plVectorLength(MathVector3f_t a)
{
	double length = 0;
	for (int i = 0; i< 3; i++)
		length += a[i] * a[i];
	length = sqrt(length);

	return length;
}

MathVectorf_t plVectorNormalize(MathVector3f_t a)
{
	MathVectorf_t i, l = (MathVectorf_t)plVectorLength(a);
	if (l)
	{
		i = 1.0f / l;
		Math_VectorScale(a, i, a);
	}

	return l;
}

void plVectorNormalizeFast(MathVector3f_t vVector)
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

bool plVectorCompare(MathVector3f_t a, MathVector3f_t b)
{
	for (int i = 0; i < 3; i++)
		if (a[i] != b[i])
			return false;

	return true;
}

void plAngleVectors(MathVector3f_t angles, MathVector3f_t forward, MathVector3f_t right, MathVector3f_t up)
{
	float	angle, sr, sp, sy, cr, cp, cy;

	angle = angles[pYAW] * ((float)pMath_PI * 2 / 360);
	sy = (float)sin(angle);
	cy = (float)cos(angle);
	angle = angles[pPITCH] * ((float)pMath_PI * 2 / 360);
	sp = (float)sin(angle);
	cp = (float)cos(angle);
	angle = angles[pROLL] * ((float)pMath_PI * 2 / 360);
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

void Math_VectorAngles(const MathVector3f_t forward, MathVector3f_t angles)
{
	MathVector3f_t temp;

	temp[0] = forward[0];
	temp[1] = forward[1];
	temp[2] = 0;

	angles[PITCH] = -atan2f(forward[2], plLengthf(temp)) / (float)pMath_PI_DIV180;
	angles[YAW] = atan2f(forward[1], forward[0]) / (float)pMath_PI_DIV180;
	angles[ROLL] = 0;
}

MathVectorf_t Math_DotProduct(MathVector3f_t a, MathVector3f_t b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void plVectorClear3fv(MathVector3f_t out)
{
	out[0] = out[1] = out[2] = 0;
}

void plVectorSet3f(MathVector3f_t out, float x, float y, float z)
{
	out[0] = x; out[1] = y; out[2] = z;
}

void plVectorCopy3fv(MathVector3f_t in, MathVector3f_t out)
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}

void plVectorScale3fv(MathVector3f_t in, MathVectorf_t scale, MathVector3f_t out)
{
	out[0] = in[0] * scale;
	out[1] = in[1] * scale;
	out[2] = in[2] * scale;
}

void plVectorAdd3fv(MathVector3f_t in, MathVector3f_t add, MathVector3f_t out)
{
	out[0] = in[0] + add[0];
	out[1] = in[1] + add[1];
	out[2] = in[2] + add[2];
}

void plVectorSubtract3fv(MathVector3f_t in, MathVector3f_t subtract, MathVector3f_t out)
{
	out[0] = in[0] - subtract[0];
	out[1] = in[1] - subtract[1];
	out[2] = in[2] - subtract[2];
}

void plVectorMultiply3fv(MathVector3f_t in, MathVector3f_t multi, MathVector3f_t out)
{
	out[0] = in[0] * multi[0];
	out[1] = in[1] * multi[1];
	out[2] = in[2] * multi[2];
}

// [23/2/2013] Added Math_VectorMake ~eukos
void Math_VectorMake(MathVector3f_t veca, float scale, MathVector3f_t vecb, MathVector3f_t vecc)
{
	vecc[0] = veca[0] + scale*vecb[0];
	vecc[1] = veca[1] + scale*vecb[1];
	vecc[2] = veca[2] + scale*vecb[2];
}

/*	Stolen from Quake 2.
*/
MathVectorf_t plColourNormalize(MathVector3f_t in, MathVector3f_t out)
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

	return -cosf(x * ((float)pMath_PI / 2.0f)) + 1.0f;
}

float Math_OutSin(float x)
{
	if (x < 0)
		return 0;
	if (x > 1.0f)
		return 1.0f;

	return sinf(x * ((float)pMath_PI / 2.0f));
}

float Math_InOutSin(float x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return -0.5f * (cosf((float)pMath_PI * x) - 1.0f);
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