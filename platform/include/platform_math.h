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

#pragma once

plEXTERN_C_START

#define PL_PI			3.14159265358979323846
#define	PL_PI_DIV180	(PL_PI/180.0)

#define PL_EPSILON_ON		0.1						// Point on plane side epsilon.
#define	PL_EPSILON_EQUAL	0.001

#define	PL_RINT(a)		((a)>0?(int)((a)+0.5):(int)((a)-0.5))
#define	PL_DEG2RAD(a)	((a)*PL_PI_DIV180)
#define	PL_ISNAN(a)		(((*(int*)&a)&255<<23)==255<<23)

typedef float MathVector3f_t[3];

typedef float plVector2f_t[2], plVector3f_t[3], plVector4f_t[4];	// Floating-point precision.
typedef double plVector2d_t[2], plVector3d_t[3], plVector4d_t[4];	// Double precision.
typedef int plVector2i[2], plVector3i_t[3];							// Integer precision.

typedef plVector3f_t plMatrix3x3f_t[3];
typedef plVector4f_t plMatrix4x4f_t[4];

typedef plVector4f_t plColour_t;

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

extern plColour_t	
	pl_white,
	pl_red,
	pl_green, 
	pl_blue;

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

#define Math_Min(a,b)					(	((a)<(b))?(a):(b)	                    )
#define Math_Max(a,b)                   (	((a)>(b))?(a):(b)	                    )
#define Math_Clamp(mini,x,maxi)			(   (x)<(mini)?(mini):(x)>(maxi)?(maxi):(x) )

#define	Math_MVSet(a,b)					(	b.vX=a,b.vY=a,b.vZ=a			)
#define	Math_MVToVector(a,b)			(	b[0]=a.vX,b[1]=a.vY,b[2]=a.vZ	)

#define Math_VectorSubtract(a,b,c)		{	c[0]=a[0]-b[0];c[1]=a[1]-b[1];c[2]=a[2]-b[2];								}
#define Math_VectorAdd(a,b,c)			{	c[0]=a[0]+b[0];c[1]=a[1]+b[1];c[2]=a[2]+b[2];								}
#define Math_VectorCopy(a,b)			(	b[0]=a[0],b[1]=a[1],b[2]=a[2]												)
#define Math_VectorScale(a,b,c)			{	c[0]=a[0]*b;c[1]=a[1]*b;c[2]=a[2]*b;										}
#define	Math_VectorAddValue(a,b,c)		{	c[0]=a[0]+b;c[1]=a[1]+b;c[2]=a[2]+b;										}
#define	Math_VectorSubtractValue(a,b,c)	{	c[0]=a[0]-b;c[1]=a[1]-b;c[2]=a[2]-b;										}
#define Math_CrossProduct(a,b,c)		{	c[0]=a[1]*b[2]-a[2]*b[1];c[1]=a[2]*b[0]-a[0]*b[2];c[2]=a[0]*b[1]-a[1]*b[0];	}
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
#define	Math_Vector4Set(a,b)			(	b[0]=b[1]=b[2]=b[3]=a					)

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

#define Math_Random()	((rand() & 0x7fff)/((float)0x7fff))
#define Math_CRandom()	(2.0f*((float)Math_Random()-0.5f))

/*
	Utility Functions
*/

struct mplane_s;

void Math_VectorAngles(const plVector3f_t forward, plVector3f_t angles);
void plAngleVectors(MathVector3f_t angles, plVector3f_t forward, plVector3f_t right, plVector3f_t up);
void plVectorNormalizeFast(MathVector3f_t vVector);
void Math_VectorMake(MathVector3f_t veca, float scale, plVector3f_t vecb, plVector3f_t vecc);

float Math_AngleMod(float a);
float plVectorToYaw(float *vec);

float plLengthf(plVector3f_t a);
double plLengthd(plVector3d_t a);

float plVectorNormalize(plVector3f_t a);
float Math_DotProduct(MathVector3f_t a, MathVector3f_t b);

MathVector_t plVectorToAngles(plVector3f_t vValue);

float plLinear(float x);
float plInPow(float x, float p);
float plOutPow(float x, float p);
float Math_InOutPow(float x, float p);
float Math_InSin(float x);
float Math_OutSin(float x);
float Math_InOutSin(float x);
float plInExp(float x);
float plOutExp(float x);
float plInOutExp(float x);
float plInCirc(float x);
float Math_OutCirc(float x);
float Math_InOutCirc(float x);
float Math_Rebound(float x);
float Math_InBack(float x);
float plOutBack(float x);
float Math_InOutBack(float x);
float plImpulse(float x, float k);
float Math_ExpPulse(float x, float k, float n);

float plColourNormalize(plVector3f_t in, plVector3f_t out);

void plColourSetf(plColour_t in, float r, float g, float b, float a);

// Vector3

inline static bool plVectorCompare(plVector3f_t a, plVector3f_t b)
{
	for (int i = 0; i < 3; i++)
		if (a[i] != b[i])
			return false;

	return true;
}

inline static void plVectorAdd3fv(plVector3f_t in, plVector3f_t add, plVector3f_t out)
{
	out[0] = in[0] + add[0];
	out[1] = in[1] + add[1];
	out[2] = in[2] + add[2];
}

inline static void plVectorSubtract3fv(plVector3f_t in, plVector3f_t subtract, plVector3f_t out)
{
	out[0] = in[0] - subtract[0];
	out[1] = in[1] - subtract[1];
	out[2] = in[2] - subtract[2];
}

inline static void plVectorMultiply3fv(plVector3f_t in, plVector3f_t multi, plVector3f_t out)
{
	out[0] = in[0] * multi[0];
	out[1] = in[1] * multi[1];
	out[2] = in[2] * multi[2];
}

inline static void plVectorSet3f(plVector3f_t out, float x, float y, float z)
{
	out[0] = x; out[1] = y; out[2] = z;
}

inline static void plVectorCopy(plVector3f_t in, plVector3f_t out)
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}

inline static void plVectorScalef(plVector3f_t in, float scale, plVector3f_t out)
{
	out[0] = in[0] * scale;
	out[1] = in[1] * scale;
	out[2] = in[2] * scale;
}

inline static void plVectorClear(plVector3f_t out)
{
	out[0] = out[1] = out[2] = 0;
}

inline static float plVectorNormalize(plVector3f_t a)
{
	float i, l = (float)plLengthf(a);
	if (l)
	{
		i = 1.0f / l;
		plVectorScalef(a, i, a);
	}

	return l;
}

// Vector2

inline static void plVector2Set2f(plVector2f_t out, float a, float b)
{
	out[0] = a; out[1] = b;
}

// Utility

void plNormalizeAngles(plVector3f_t angles);
void plTurnVector(plVector3f_t out, const plVector3f_t forward, const plVector3f_t side, float angle);
char *plVectorToString(plVector3f_t vector);
bool plIsIntersecting(plVector3f_t mvFirstMins, plVector3f_t mvFirstMaxs, plVector3f_t mvSecondMins, MathVector3f_t mvSecondMaxs);

plEXTERN_C_END
