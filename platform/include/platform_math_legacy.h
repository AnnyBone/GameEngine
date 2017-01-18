/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#pragma once

#include "platform.h"

#define PL_PI			3.14159265358979323846
#define	PL_PI_DIV180	(PL_PI/180.0)

#define PL_PIf 			3.14159265358979323846f
#define PL_PI_DIV180f	(PL_PIf/180.0f)
#define PL_DEG2RADf(a)	((a) * PL_PI_DIV180f)

#define	PL_RINT(a)		((a)>0?(int)((a)+0.5):(int)((a)-0.5))
#define	PL_DEG2RAD(a)	((a)*PL_PI_DIV180)
#define	PL_ISNAN(a)		(((*(int*)&a)&255<<23)==255<<23)

plEXTERN_C_START

typedef plVector3f_t PLMatrix3x3f[3];
typedef plVector4f_t PLMatrix4x4f[4], PLQuaternion;

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
// LEGACY END

extern plVector2f_t	pl_origin2f;
extern plVector3f_t	pl_origin3f;
extern plVector4f_t	pl_origin4f;

extern PLColour
	pl_white,
	pl_red,
	pl_green, 
	pl_blue,
	pl_black;

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

void Math_VectorMake(PLVector3f veca, PLfloat scale, PLVector3f vecb, PLVector3f vecc);

float Math_DotProduct(PLVector3f a, PLVector3f b);

MathVector_t plVectorToAngles(PLVector3f vValue);

static PL_INLINE PLint plMaxi(const PLint a, const PLint b) 		{ return (a) < (b) ? (b) : (a); }
static PL_INLINE PLfloat plMaxf(const PLfloat a, const PLfloat b) 	{ return (a) < (b) ? (b) : (a); }

/*	Vector3f	*/

static PL_INLINE bool plVectorCompare(const PLVector3f a, const PLVector3f b)
{
	return ((a[0] == b[0]) && (a[1] == b[1]) && (a[2] == b[2]));
}

static PL_INLINE void plVectorAdd3fv(const PLVector3f in, const PLVector3f add, PLVector3f out)
{
	out[0] = in[0] + add[0]; out[1] = in[1] + add[1]; out[2] = in[2] + add[2];
}

static PL_INLINE void plVectorAddf(const PLVector3f in, const PLfloat add, PLVector3f out)
{
	out[0] = in[0] + add; out[1] = in[1] + add; out[2] = in[2] + add;
}

static PL_INLINE void plVectorSubtract3fv(const PLVector3f in, const PLVector3f subtract, PLVector3f out)
{
	out[0] = in[0] - subtract[0]; out[1] = in[1] - subtract[1]; out[2] = in[2] - subtract[2];
}

static PL_INLINE void plVectorMultiply3fv(PLVector3f in, PLVector3f multi, PLVector3f out)
{
	out[0] = in[0] * multi[0]; out[1] = in[1] * multi[1]; out[2] = in[2] * multi[2];
}

static PL_INLINE void plVectorSet3f(PLVector3f out, const PLfloat x, const PLfloat y, const PLfloat z)
{
	out[0] = x; out[1] = y; out[2] = z;
}

static PL_INLINE PLfloat plLengthf(const PLVector3f a)
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
	float l = plLengthf(a);
	if (l)
	{
		float i = 1.0f / l;
		plVectorScalef(a, i, a);
	}

	return l;
}

/*	Colour	*/

static PL_INLINE void plColourSetf(PLColour in, PLfloat r, PLfloat g, PLfloat b, PLfloat a)
{
	in[0] = r; in[1] = g; in[2] = b; in[3] = a;
}

static PL_INLINE PLfloat plColourNormalize(PLVector3f in, PLVector3f out)
{
	float max = in[0];
	if (in[1] > max)
		max = in[1];
	else if (in[2] > max)
		max = in[2];
	if (!max)
		return 0;

	float scale = 1.0f / max;
	plVectorScalef(in, scale, out);
	return max;
}