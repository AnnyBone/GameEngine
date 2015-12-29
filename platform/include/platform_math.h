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

#ifndef PLATFORM_MATH_H
#define	PLATFORM_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "platform.h"

#ifdef __cplusplus
	/*
	TODO: C++ Math Classes
	*/
#endif

#define pMath_PI			3.14159265358979323846
#define	pMath_PI_DIV180		(pMath_PI/180.0)
#define	pMath_EPSILON_ON	0.1						// Point on plane side epsilon.
#define	pMath_EPSILON_EQUAL	0.001

typedef double MathVectord_t;	// Double precision
typedef float MathVectorf_t;	// Floating-point precision

// For compatability...
#ifdef PLATFORM_MATH_DOUBLE
#define vec_t MathVectord_t
#else
#define vec_t MathVectorf_t
#endif

#define	pMath_RINT(a)		((a)>0?(int)((a)+0.5):(int)((a)-0.5))
#define	pMath_DEG2RAD(a)	((a)*pMath_PI_DIV180)
#define	pMath_ISNAN(a)		(((*(int*)&a)&255<<23)==255<<23)

typedef MathVectorf_t MathVector2f_t[2], MathVector3f_t[3], MathVector4f_t[4];
typedef MathVector4f_t Colour_t, MathMatrix4x4f_t[4];

// For compatability...
#define	vec3_t	MathVector3f_t

typedef MathVectord_t MathVector2d_t[2], MathVector3d_t[3], MathVector4d_t[4];
typedef MathVector4d_t MathMatrix4x4d_t[4];

typedef struct
{
#ifdef PLATFORM_MATH_DOUBLE
	MathVectord_t
#else
	MathVectorf_t 
#endif
		vX, vY, vZ;
} MathVector_t;

extern MathVector_t		mvOrigin;
extern MathVector2f_t	mv2Origin;
extern MathVector3f_t	g_mvOrigin3f;
extern MathVector4f_t	mv4Origin;

extern Colour_t	
	g_colourwhite, 
	g_colourred, 
	g_colourgreen, 
	g_colourblue;

enum PlatformMathWH
{
	pWIDTH,	pHEIGHT	// 0, 1
};
// For compatability...
#define	WIDTH	pWIDTH
#define	HEIGHT	pHEIGHT

enum PlatformMathXYZ
{
	pX, pY, pZ	// 0, 1, 2
};

enum PlatformMathPYR
{
	pPITCH, pYAW, pROLL	// 0, 1, 2
};
// For compatability...
#define PITCH	pPITCH
#define	YAW		pYAW
#define	ROLL	pROLL

enum PlatformMathRGBA
{
	pRED, pGREEN, pBLUE, pALPHA	// 0, 1, 2, 3
};
// For compatability...
#define	RED		pRED
#define	GREEN	pGREEN
#define	BLUE	pBLUE
#define	ALPHA	pALPHA

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

void	_Math_VectorSubtract(MathVector3f_t a, MathVector3f_t b, MathVector3f_t c);
void	_Math_VectorAdd(MathVector3f_t a, MathVector3f_t b, MathVector3f_t c);
void	_Math_VectorCopy(MathVector3f_t a, MathVector3f_t b);
void	_Math_VectorScale(MathVector3f_t in, MathVectorf_t scale, MathVector3f_t out);

struct	mplane_s;

void	Math_VectorAngles(const MathVector3f_t forward, MathVector3f_t angles);
void	Math_AngleVectors(MathVector3f_t angles, MathVector3f_t forward, MathVector3f_t right, MathVector3f_t up);
void	Math_VectorNormalizeFast(MathVector3f_t vVector);
void	Math_VectorMake(MathVector3f_t veca, float scale, MathVector3f_t vecb, MathVector3f_t vecc);

double	Math_VectorLength(MathVector3f_t a);

float	Math_AngleMod(float a);
float	Math_VectorToYaw(MathVectorf_t *vVector);

bool Math_VectorCompare(MathVector3f_t a, MathVector3f_t b);
bool Math_IsIntersecting(MathVector3f_t mvFirstMins, MathVector3f_t mvFirstMaxs, MathVector3f_t mvSecondMins, MathVector3f_t mvSecondMaxs);

MathVectorf_t Math_Lengthf(MathVector3f_t a);
MathVectord_t Math_Lengthd(MathVector3d_t a);
#if 0
#define Math_Length(a) _Generic((a), \
	MathVector3d_t: Math_Lengthd(a) \
	MathVector3f_t: Math_Lengthf(a))(a)
#else
#define Math_Length(a) Math_Lengthf(a)
#endif

MathVectorf_t Math_VectorNormalize(MathVector3f_t a);
MathVectorf_t Math_ColorNormalize(MathVector3f_t in, MathVector3f_t out);
MathVectorf_t Math_DotProduct(MathVector3f_t a, MathVector3f_t b);

MathVector_t Math_VectorToAngles(MathVector3f_t vValue);

float Math_Linear(float x);
float Math_InPow(float x, float p);
float Math_OutPow(float x, float p);
float Math_InOutPow(float x, float p);
float Math_InSin(float x);
float Math_OutSin(float x);
float Math_InOutSin(float x);
float Math_InExp(float x);
float Math_OutExp(float x);
float Math_InOutExp(float x);
float Math_InCirc(float x);
float Math_OutCirc(float x);
float Math_InOutCirc(float x);
float Math_Rebound(float x);
float Math_InBack(float x);
float Math_OutBack(float x);
float Math_InOutBack(float x);
float Math_Impulse(float x, float k);
float Math_ExpPulse(float x, float k, float n);

#ifdef __cplusplus
}
#endif

#endif // !PLATFORM_MATH_H
