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

#pragma once

#include "platform.h"

#define PL_PI			3.14159265358979323846
#define	PL_PI_DIV180	(PL_PI/180.0)

#define PL_PIf 3.14159265358979323846f

#define PL_EPSILON_ON		0.1						// Point on plane side epsilon.
#define	PL_EPSILON_EQUAL	0.001

#define	PL_RINT(a)		((a)>0?(int)((a)+0.5):(int)((a)-0.5))
#define	PL_DEG2RAD(a)	((a)*PL_PI_DIV180)
#define	PL_ISNAN(a)		(((*(int*)&a)&255<<23)==255<<23)

plEXTERN_C_START

typedef PLfloat MathVector3f_t[3], PLVector3f[3];

typedef PLfloat		plVector2f_t[2],	plVector3f_t[3],	plVector4f_t[4];	// Floating-point precision.
typedef PLdouble	plVector2d_t[2],	plVector3d_t[3],	plVector4d_t[4];	// Double precision.
typedef PLint		plVector2i_t[2],	plVector3i_t[3],	plVector4i_t[4];	// Integer precision.

typedef plVector3f_t plMatrix3x3f_t[3];
typedef plVector4f_t plMatrix4x4f_t[4];

typedef plVector4f_t PLColour;

typedef struct
{
#ifdef PLATFORM_MATH_DOUBLE
	double
#else
	float
#endif
		vX, vY, vZ;
} MathVector_t;

extern plVector2f_t	pl_origin2f;
extern plVector3f_t	pl_origin3f;
extern plVector4f_t	pl_origin4f;

extern PLColour
	pl_white,
	pl_red,
	pl_green, 
	pl_blue,
	pl_black;

#define	PL_WIDTH	0
#define	PL_HEIGHT	1
#ifndef WIDTH
#	define WIDTH	PL_WIDTH
#endif
#ifndef HEIGHT
#	define HEIGHT	PL_HEIGHT
#endif

#define	PL_X	0
#define	PL_Y	1
#define	PL_Z	2
#if 0
#ifndef X
#	define X PL_X
#endif
#ifndef Y
#	define Y PL_Y
#endif
#ifndef Z
#	define Z PL_Z
#endif
#endif

#define PL_PITCH	0
#define	PL_YAW		1
#define	PL_ROLL		2
#ifndef PITCH
#	define PITCH	PL_PITCH
#endif
#ifndef YAW
#	define YAW		PL_YAW
#endif
#ifndef ROLL
#	define ROLL		PL_ROLL
#endif

#define	PL_RED		0
#define	PL_GREEN	1
#define	PL_BLUE		2
#define	PL_ALPHA	3
#ifndef RED
#	define RED		PL_RED
#endif
#ifndef GREEN
#	define GREEN	PL_GREEN
#endif
#ifndef BLUE
#	define BLUE		PL_BLUE
#endif
#ifndef ALPHA
#	define ALPHA	PL_ALPHA
#endif

/*
	Utility Defines
*/

#define plMin(a,b)						(	((a)<(b))?(a):(b)	                    )
#define plMax(a,b)						(	((a)>(b))?(a):(b)	                    )
#define plClamp(mini,x,maxi)			(   (x)<(mini)?(mini):(x)>(maxi)?(maxi):(x) )

#define	Math_MVToVector(a,b)			(	b[0]=a.vX,b[1]=a.vY,b[2]=a.vZ	)

#define Math_VectorSubtract(a,b,c)		{	c[0]=a[0]-b[0];c[1]=a[1]-b[1];c[2]=a[2]-b[2];								}
#define Math_VectorAdd(a,b,c)			{	c[0]=a[0]+b[0];c[1]=a[1]+b[1];c[2]=a[2]+b[2];								}
#define Math_VectorCopy(a,b)			(	b[0]=a[0],b[1]=a[1],b[2]=a[2]												)
#define Math_VectorScale(a,b,c)			{	c[0]=a[0]*b;c[1]=a[1]*b;c[2]=a[2]*b;										}
#define	Math_VectorAddValue(a,b,c)		{	c[0]=a[0]+b;c[1]=a[1]+b;c[2]=a[2]+b;										}
#define	Math_VectorSubtractValue(a,b,c)	{	c[0]=a[0]-b;c[1]=a[1]-b;c[2]=a[2]-b;										}
#define plCrossProduct(a,b,c)			{	c[0]=a[1]*b[2]-a[2]*b[1];c[1]=a[2]*b[0]-a[0]*b[2];c[2]=a[0]*b[1]-a[1]*b[0];	}
#define Math_VectorMA(a,b,c,d)			{	d[0]=a[0]+b*c[0];d[1]=a[1]+b*c[1];d[2]=a[2]+b*c[2];							}
#define Math_VectorInverse(a)			(	a[0]=-a[0],a[1]=-a[1],a[2]=-a[2]											)
#define Math_VectorClear(a)				(	a[0]=a[1]=a[2]=0															)
#define Math_VectorNegate(a,b)			(	b[0]=-a[0],b[1]=-a[1],b[2]=-a[2]											)
#define	Math_VectorSet(a,b)				(	b[0]=b[1]=b[2]=a															)
#define	Math_VectorDivide(a,b,c)		{	c[0]=a[0]/b;c[1]=a[1]/b;c[2]=a[2]/b;										}
#define	Math_VectorToMV(a,b)			(	b.vX=a[0],b.vY=a[1],b.vZ=a[2]												)

#define	Math_Vector2Copy(a,b)			(	b[0]=a[0],b[1]=a[1]	)
#define	Math_Vector2Set(a,b)			(	b[0]=b[1]=a			)

#define	Math_Vector4Copy(a,b)			(	b[0]=a[0],b[1]=a[1],b[2]=a[2],b[3]=a[3]	)

#define Math_Matrix4x4Row(a,b,c) { int k; for(k = 0; k < 4; ++k) a[k] = b[c][k]; }
#define Math_Matrix4x4Negate(a,b) \
{ \
	float s[6]; \
	float c[6]; \
	s[0] = a[0][0] * a[1][1] - a[1][0] * a[0][1]; \
	s[1] = a[0][0] * a[1][2] - a[1][0] * a[0][2]; \
	s[2] = a[0][0] * a[1][3] - a[1][0] * a[0][3]; \
	s[3] = a[0][1] * a[1][2] - a[1][1] * a[0][2]; \
	s[4] = a[0][1] * a[1][3] - a[1][1] * a[0][3]; \
	s[5] = a[0][2] * a[1][3] - a[1][2] * a[0][3]; \
	c[0] = a[2][0] * a[3][1] - a[3][0] * a[2][1]; \
	c[1] = a[2][0] * a[3][2] - a[3][0] * a[2][2]; \
	c[2] = a[2][0] * a[3][3] - a[3][0] * a[2][3]; \
	c[3] = a[2][1] * a[3][2] - a[3][1] * a[2][2]; \
	c[4] = a[2][1] * a[3][3] - a[3][1] * a[2][3]; \
	c[5] = a[2][2] * a[3][3] - a[3][2] * a[2][3]; \
	float idet = 1.0f / (s[0] * c[5] - s[1] * c[4] + s[2] * c[3] + s[3] * c[2] - s[4] * c[1] + s[5] * c[0]); \
	b[0][0] = (a[1][1] * c[5] - a[1][2] * c[4] + a[1][3] * c[3]) * idet; \
	b[0][1] = (-a[0][1] * c[5] + a[0][2] * c[4] - a[0][3] * c[3]) * idet; \
	b[0][2] = (a[3][1] * s[5] - a[3][2] * s[4] + a[3][3] * s[3]) * idet; \
	b[0][3] = (-a[2][1] * s[5] + a[2][2] * s[4] - a[2][3] * s[3]) * idet; \
	b[1][0] = (-a[1][0] * c[5] + a[1][2] * c[2] - a[1][3] * c[1]) * idet; \
	b[1][1] = (a[0][0] * c[5] - a[0][2] * c[2] + a[0][3] * c[1]) * idet; \
	b[1][2] = (-a[3][0] * s[5] + a[3][2] * s[2] - a[3][3] * s[1]) * idet; \
	b[1][3] = (a[2][0] * s[5] - a[2][2] * s[2] + a[2][3] * s[1]) * idet; \
	b[2][0] = (a[1][0] * c[4] - a[1][1] * c[2] + a[1][3] * c[0]) * idet; \
	b[2][1] = (-a[0][0] * c[4] + a[0][1] * c[2] - a[0][3] * c[0]) * idet; \
	b[2][2] = (a[3][0] * s[4] - a[3][1] * s[2] + a[3][3] * s[0]) * idet; \
	b[2][3] = (-a[2][0] * s[4] + a[2][1] * s[2] - a[2][3] * s[0]) * idet; \
	b[3][0] = (-a[1][0] * c[3] + a[1][1] * c[1] - a[1][2] * c[0]) * idet; \
	b[3][1] = (a[0][0] * c[3] - a[0][1] * c[1] + a[0][2] * c[0]) * idet; \
	b[3][2] = (-a[3][0] * s[3] + a[3][1] * s[1] - a[3][2] * s[0]) * idet; \
	b[3][3] = (a[2][0] * s[3] - a[2][1] * s[1] + a[2][2] * s[0]) * idet; \
}

/*
	Utility Functions
*/

struct mplane_s;

void Math_VectorMake(PLVector3f veca, PLfloat scale, PLVector3f vecb, PLVector3f vecc);

float Math_DotProduct(PLVector3f a, PLVector3f b);

MathVector_t plVectorToAngles(PLVector3f vValue);

/*	Vector3f	*/

static PL_INLINE bool plVectorCompare(PLVector3f a, PLVector3f b)
{
	for (int i = 0; i < 3; i++)
		if (a[i] != b[i])
			return false;
	
	return true;
}

static PL_INLINE void plVectorAdd3fv(PLVector3f in, PLVector3f add, PLVector3f out)
{
	out[0] = in[0] + add[0];
	out[1] = in[1] + add[1];
	out[2] = in[2] + add[2];
}

static PL_INLINE void plVectorAddf(PLVector3f in, PLfloat add, PLVector3f out)
{
	out[0] = in[0] + add;
	out[1] = in[1] + add;
	out[2] = in[2] + add;
}

static PL_INLINE void plVectorSubtract3fv(PLVector3f in, PLVector3f subtract, PLVector3f out)
{
	out[0] = in[0] - subtract[0];
	out[1] = in[1] - subtract[1];
	out[2] = in[2] - subtract[2];
}

static PL_INLINE void plVectorMultiply3fv(PLVector3f in, PLVector3f multi, PLVector3f out)
{
	out[0] = in[0] * multi[0];
	out[1] = in[1] * multi[1];
	out[2] = in[2] * multi[2];
}

static PL_INLINE void plVectorSet3f(PLVector3f out, PLfloat x, PLfloat y, PLfloat z)
{
	out[0] = x; out[1] = y; out[2] = z;
}

static PL_INLINE void plVectorCopy(PLVector3f in, PLVector3f out)
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}

static PL_INLINE void plVectorScalef(PLVector3f in, PLfloat scale, PLVector3f out)
{
	out[0] = in[0] * scale;
	out[1] = in[1] * scale;
	out[2] = in[2] * scale;
}

static PL_INLINE void plVectorClear(PLVector3f out)
{
	out[0] = out[1] = out[2] = 0;
}

static PL_INLINE PLfloat plLengthf(PLVector3f a)
{
	float l = 0;
	for (int i = 0; i<3; i++)
		l += a[i] * a[i];
	l = sqrtf(l);

	return l;
}

static PL_INLINE PLdouble plLengthd(plVector3d_t a)
{
	double l = 0;
	for (int i = 0; i<3; i++)
		l += a[i] * a[i];
	l = sqrt(l);

	return l;
}

static PL_INLINE PLfloat plVectorNormalize(PLVector3f a)
{
	float i, l = (float)plLengthf(a);
	if (l)
	{
		i = 1.0f / l;
		plVectorScalef(a, i, a);
	}

	return l;
}

static PL_INLINE void plVectorNormalizeFast(PLVector3f vec)
{
	float y, num = Math_DotProduct(vec, vec);
	if (num != 0.0f)
	{
		*((long*)&y) = 0x5f3759df - ((*(long*)&num) >> 1);
		y = y*(1.5f - (num*0.5f*y*y));

		plVectorScalef(vec, y, vec);
	}
}

static PL_INLINE PLfloat plVectorDifference(PLVector3f a, PLVector3f b)
{
	plVector3f_t spot;
	plVectorSubtract3fv(a, b, spot);
	return plLengthf(spot);
}

/*	Vector2f	*/

static PL_INLINE void plVector2Set2f(plVector2f_t out, PLfloat a, PLfloat b)
{
	out[0] = a; out[1] = b;
}

/*	Vector4f	*/

static PL_INLINE void plVector4Setf(PLfloat in, plVector4f_t out)
{
	out[0] = out[1] = out[2] = out[3] = in;
}

/*	Colour	*/

static PL_INLINE void plColourSetf(PLColour in, PLfloat r, PLfloat g, PLfloat b, PLfloat a)
{
	in[0] = r; in[1] = g; in[2] = b; in[3] = a;
}

static PL_INLINE PLfloat plColourNormalize(PLVector3f in, PLVector3f out)
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
	plVectorScalef(in, scale, out);
	return max;
}

/*
	Interpolation
	This is borrowed from http://probesys.blogspot.co.uk/
*/

static PL_INLINE PLfloat plOutPow(PLfloat x, PLfloat p)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	PLint sign = (PLint)p % 2 == 0 ? -1 : 1;
	return (sign * (powf(x - 1.0f, p) + sign));
}

static PL_INLINE PLfloat plLinear(PLfloat x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return x;
}

static PL_INLINE PLfloat plInPow(PLfloat x, PLfloat p)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return powf(x, p);
}

static PL_INLINE PLfloat plInSin(PLfloat x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return -cosf(x * ((float)PL_PI / 2.0f)) + 1.0f;
}

static PL_INLINE PLfloat plOutSin(PLfloat x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return sinf(x * ((float)PL_PI / 2.0f));
}

static PL_INLINE PLfloat plInExp(PLfloat x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return powf(2.0f, 10.0f * (x - 1.0f));
}

static PL_INLINE PLfloat plOutExp(PLfloat x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return -powf(2.0f, -1.0f * x) + 1.0f;
}

static PL_INLINE PLfloat plInOutExp(PLfloat x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return x < 0.5f ? 0.5f * powf(2.0f, 10.0f * (2.0f*x - 1.0f)) :
		0.5f * (-powf(2.0f, 10.0f * (-2.0f*x + 1.0f)) + 1.0f);
}

static PL_INLINE PLfloat plInCirc(PLfloat x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return -(sqrtf(1.0f - x *x) - 1.0f);
}

static PL_INLINE PLfloat plOutBack(PLfloat x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return (x - 1.0f) * (x - 1.0f) * ((1.70158f + 1.0f) * (x - 1.0f) + 1.70158f) + 1.0f;
}

// The variable, k, controls the stretching of the function.
static PL_INLINE PLfloat plImpulse(PLfloat x, PLfloat k)
{
	PLfloat h = k*x;
	return h*expf(1.0f - h);
}

static PL_INLINE PLfloat plRebound(PLfloat x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	if (x < (1.0f / 2.75f)) return 1.0f - 7.5625f * x * x;
	else if (x < (2.0f / 2.75f)) return 1.0f - (7.5625f * (x - 1.5f / 2.75f) *
		(x - 1.5f / 2.75f) + 0.75f);
	else if (x < (2.5f / 2.75f)) return 1.0f - (7.5625f * (x - 2.25f / 2.75f) *
		(x - 2.25f / 2.75f) + 0.9375f);
	else return 1.0f - (7.5625f * (x - 2.625f / 2.75f) * (x - 2.625f / 2.75f) +
		0.984375f);
}

static PL_INLINE PLfloat plExpPulse(PLfloat x, PLfloat k, PLfloat n)
{
	return expf(-k*powf(x, n));
}

static PL_INLINE PLfloat plInOutBack(PLfloat x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return x < 0.5f ? 0.5f * (4.0f * x * x * ((2.5949f + 1.0f) * 2.0f * x - 2.5949f)) :
		0.5f * ((2.0f * x - 2.0f) * (2.0f * x - 2.0f) * ((2.5949f + 1.0f) * (2.0f * x - 2.0f) +
		2.5949f) + 2.0f);
}

static PL_INLINE PLfloat plInBack(PLfloat x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return x * x * ((1.70158f + 1.0f) * x - 1.70158f);
}

static PL_INLINE PLfloat plInOutCirc(PLfloat x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return x < 1.0f ? -0.5f * (sqrtf(1.0f - x*x) - 1.0f) :
		0.5f * (sqrtf(1.0f - ((1.0f * x) - 2.0f) * ((2.0f * x) - 2.0f)) + 1.0f);
}

static PL_INLINE PLfloat plOutCirc(PLfloat x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return sqrtf(1.0f - (x - 1.0f)*(x - 1.0f));
}

static PL_INLINE PLfloat plInOutSin(PLfloat x)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	return -0.5f * (cosf((PLfloat)PL_PI * x) - 1.0f);
}

static PL_INLINE PLfloat plInOutPow(PLfloat x, PLfloat p)
{
	if (x < 0)		return 0;
	if (x > 1.0f)	return 1.0f;

	int sign = (int)p % 2 == 0 ? -1 : 1;
	return (sign / 2.0f * (powf(x - 2.0f, p) + sign * 2.0f));
}

/*	Utility Functions	*/

static PL_INLINE PLfloat plAngleMod(PLfloat a)
{
	return a = (360.0f / 65536)*((int)(a*(65536 / 360.0f)) & 65535);
}

static PL_INLINE PLfloat plAngleDelta(PLfloat a)
{
	a = plAngleMod(a);
	if (a > 180)
		a -= 360;

	return a;
}

// Calculate a sphere from bounds.
static PL_INLINE void plSphereFromBounds(PLVector3f mins, PLVector3f maxs, PLVector3f origin, PLfloat *radius)
{
	plVectorAdd3fv(mins, maxs, origin);
	plVectorScalef(origin, 0.5f, origin);

	PLVector3f temp;
	plVectorSubtract3fv(maxs, origin, temp);

	*radius = plLengthf(temp);
}

// Check to see if an area is intersecting another area.
static PL_INLINE bool plIsIntersecting(
	PLVector3f mins1, PLVector3f maxs1,
	PLVector3f mins2, PLVector3f maxs2
)
{
	if (mins1[0] > maxs2[0] ||
		mins1[1] > maxs2[1] ||
		mins1[2] > maxs2[2] ||
		maxs1[0] < mins2[0] ||
		maxs1[1] < mins2[1] ||
		maxs1[2] < mins2[2])
		return false;

	return true;
}

static PL_INLINE void plNormalizeAngles(PLVector3f angles)
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

static PL_INLINE PLfloat plVectorToYaw(PLfloat *vec)
{
	float result;
	if (vec[1] == 0 && vec[0] == 0)
		result = 0;
	else
	{
		result = (PLfloat)(atan2(vec[1], vec[0])*180.0f / PL_PI);
		if (result < 0)
			result += 360.0f;
	}

	return result;
}

static PL_INLINE void plVectorAngles(const PLVector3f forward, PLVector3f angles)
{
	PLVector3f temp;
	temp[0] = forward[0];
	temp[1] = forward[1];
	temp[2] = 0;

	angles[PITCH] = -atan2f(forward[2], plLengthf(temp)) / (PLfloat)PL_PI;
	angles[YAW] = atan2f(forward[1], forward[0]) / (PLfloat)PL_PI;
	angles[ROLL] = 0;
}

static PL_INLINE void plAngleVectors(const PLVector3f angles, PLVector3f forward, PLVector3f right, PLVector3f up)
{
	PLfloat	angle, sr, sp, sy, cr, cp, cy;

	angle = angles[PL_YAW] * ((PLfloat)PL_PI * 2 / 360);
	sy = (PLfloat)sin(angle);
	cy = (PLfloat)cos(angle);
	angle = angles[PL_PITCH] * ((PLfloat)PL_PI * 2 / 360);
	sp = (PLfloat)sin(angle);
	cp = (PLfloat)cos(angle);
	angle = angles[PL_ROLL] * ((PLfloat)PL_PI * 2 / 360);
	sr = (PLfloat)sin(angle);
	cr = (PLfloat)cos(angle);

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

/*	turn forward towards side on the plane defined by forward and side
	if angle = 90, the result will be equal to side
	assumes side and forward are perpendicular, and normalized
	to turn away from side, use a negative angle
*/
static PL_INLINE void plTurnVector(PLVector3f out, const PLVector3f forward, const PLVector3f side, PLfloat angle)
{
	float scale_forward, scale_side;

	scale_forward = cosf((PLfloat)PL_DEG2RAD(angle));
	scale_side = sinf((PLfloat)PL_DEG2RAD(angle));

	out[0] = scale_forward*forward[0] + scale_side*side[0];
	out[1] = scale_forward*forward[1] + scale_side*side[1];
	out[2] = scale_forward*forward[2] + scale_side*side[2];
}

static PL_INLINE PLfloat plSnapToEights(PLfloat x)
{
	x *= 8.0f;
	if (x > 0) x += 0.5f;
	else x -= 0.5f;

	return 0.125f * (PLint)x;
}

static PL_INLINE PLfloat plRandom(void)
{
	return ((rand() & 0x7fff) / ((PLfloat)0x7fff));
}

static PL_INLINE PLfloat plCRandom(void)
{
	return (2.0f*((PLfloat)plRandom() - 0.5f));
}

const PLchar *plVectorToString(PLVector3f vector);

plEXTERN_C_END

#ifdef __cplusplus

namespace pl
{
	class Vector
	{};

	class Vector3f : public Vector
	{
	public:
		Vector3f()
		{
			memset(vector, 0, sizeof(PLfloat[3]));
		}

		std::vector<PLfloat> Get()
		{
			return std::vector<PLfloat> { vector[0], vector[1], vector[2] };
		}

		void Set(PLfloat x, PLfloat y, PLfloat z)
		{
			vector[0] = x;
			vector[1] = y;
			vector[2] = z;
		}

	protected:
	private:
		float vector[3];
	};

	class Colour
	{
	public:
		Colour()
		{
			memset(colour, 255, sizeof(PLuchar[4]));
		}

		Colour(PLuchar _colour[4])
		{
			Set(_colour);
		}

		Colour(PLuchar _r, PLuchar _g, PLuchar _b, PLuchar _a = 255)
		{
			Set(_r, _g, _b, _a);
		}

		std::vector<PLuchar> Get()
		{
			return std::vector<PLuchar> { colour[0], colour[1], colour[2], colour[3] };
		}

		void Set(PLuchar _colour[4])
		{
			colour[0] = _colour[0];
			colour[1] = _colour[1];
			colour[2] = _colour[2];
			colour[3] = _colour[3];
		}

		void Set(PLuchar _r, PLuchar _g, PLuchar _b, PLuchar _a = 255)
		{
			colour[0] = _r;
			colour[1] = _g;
			colour[2] = _b;
			colour[3] = _a;
		}

		void Set(PLfloat _r, PLfloat _g, PLfloat _b, PLfloat _a = 1.0f)
		{
			colour[0] = (PLuchar)(_r / 255.0f);
			colour[1] = (PLuchar)(_g / 255.0f);
			colour[2] = (PLuchar)(_b / 255.0f);
			colour[3] = (PLuchar)(_a / 255.0f);
		}

		void Set(plVector3f_t _colour)
		{
			plVector3f_t in;
			plVectorCopy(_colour, in);
			plVectorNormalize(in);

			Set(in[0], in[1], in[2]);
		}

	protected:
	private:
		PLuchar colour[4]; // RGBA
	};
}

#	ifndef PLATFORM_EXCLUDE_GLM
#		ifdef _MSC_VER
#			pragma warning(disable: 4201)
#		endif
#		include <glm/gtc/type_ptr.hpp>
#		include <glm/vec2.hpp>
#		include <glm/vec3.hpp>
#		include <glm/vec4.hpp>
#		include <glm/mat4x4.hpp>
#		include <glm/gtc/matrix_transform.hpp>
#		ifdef _MSC_VER
#			pragma warning(default: 4201)
#		endif
#	endif

#endif
