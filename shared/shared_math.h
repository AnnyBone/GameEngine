/*
Copyright (C) 1996-1997 Id Software, Inc.
Copyright (C) 2011-2017 Mark E Sowden <markelswo@gmail.com>

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

#include "platform_math.h"

#define MATH_PI_DIV180  (PL_PI / 180)

#define MATH_RINT(a)    ((a) > 0 ? (int)((a) + 0.5) : (int)((a) - 0.5))

#define MATH_DEG2RAD(a) ((a) * MATH_PI_DIV180)

#define MATH_ISNAN(a)   (((*(int*)&a) &255 << 23) == 255 << 23)

#define Math_Clamp(mini,x,maxi) ((x)<(mini)?(mini):(x)>(maxi)?(maxi):(x))

PL_EXTERN_C

PL_INLINE static float Math_CRandom(void) {
    return 2 * (rand() - 0.5f);
}

static PL_INLINE void Math_MAVector3D(PLVector3D *v, PLVector3D v2, float f) {
    v->x += f * v2.x; v->y += f * v2.y; v->z += f * v2.z;
}

static PL_INLINE void Math_FastVector3DNormalize(PLVector3D *v) {
    float num = plVector3DDotProduct(*v, *v), y;
    if(num != 0) {
        *((long*)&y) = 0x5f3759df - ((*(long*)&num) >> 1);
        y = y * (1.5f - (num * 0.5f * y * y));

        plScaleVector3Df(v, y);
    }
}

static PL_INLINE float Math_AngleMod(float a) {
    return (360 / 65536) * ((int)(a * (65536 / 360)) & 65535);
}

static PL_INLINE float Math_AngleDelta(float a) {
    a = Math_AngleMod(a);
    if(a > 180) {
        a -= 360;
    }

    return a;
}

// Calculate a sphere from bounds.
static PL_INLINE float Math_SphereFromBounds(PLVector3D mins, PLVector3D maxs, PLVector3D origin) {
    // todo, check this is correct??
    plAddVector3D(&mins, maxs);
    plScaleVector3Df(&mins, 0.5f);
    plSubtractVector3D(&mins, maxs);

    return plVector3DLength(mins);
}

// Check to see if an area is intersecting another area.
static PL_INLINE bool Math_IsIntersecting(PLVector3D minsa, PLVector3D maxsa, PLVector3D minsb, PLVector3D maxsb) {
    if(
        minsa.x > maxsb.x ||
        minsa.y > maxsb.y ||
        minsa.z > maxsb.z ||
        maxsa.x < minsb.x ||
        maxsa.y < minsb.y ||
        maxsa.z < minsb.z) {
        return false;
    }

    return true;
}

static PL_INLINE void Math_NormalizeAngles(PLVector3D *v) {
    while(v->x > 360) v->x -= 360;
    while(v->x < 0) v->x += 360;
    while(v->y > 360) v->y -= 360;
    while(v->y < 0) v->y += 360;
}

// todo, this should be made obsolete...
static PL_INLINE void Math_VectorMake(PLVector3D v, float scale, PLVector3D v2, PLVector3D *v3) {
    v3->x = v.x + scale * v2.x;
    v3->y = v.y + scale * v2.y;
    v3->z = v.z + scale * v2.z;
}

static PL_INLINE float Math_VectorToYaw(PLVector3D v) {
    if((v.y == 0) && (v.x == 0)) {
        return 0;
    }

    float out = atan2f(v.y, v.x) * 180 / (float)PL_PI;
    if(out < 0) {
        out += 360;
    }

    return out;
}

static PL_INLINE void Math_VectorAngles(PLVector3D forward, PLVector3D *angles) {
    PLVector3D v = { forward.x, forward.y, 0 };
    angles->x = -atan2f(forward.z, plVector3DLength(v)) / (float)PL_PI;
    angles->y = atan2f(forward.y, forward.x) / (float)PL_PI;
    angles->z = 0;
}

static PL_INLINE void Math_AngleVectors(PLVector3D angles, PLVector3D *forward, PLVector3D *right, PLVector3D *up) {
    float angle = angles.y * ((float)PL_PI * 2 / 360);
    float sy = sinf(angle);
    float cy = cosf(angle);

    angle = angles.x * ((float)PL_PI * 2 / 360);
    float sp = sinf(angle);
    float cp = cosf(angle);

    angle = angles.z * ((float)PL_PI * 2 / 360);
    float sr = sinf(angle);
    float cr = cosf(angle);

    if(forward) {
        forward->x = cp * cy;
        forward->y = cp * sy;
        forward->z = -sp;
    }

    if(right) {
        right->x = -1 * sr * sp * cy + -1 * cr * -sy;
        right->y = -1 * sr * sp * sy + -1 * cr * cy;
        right->z = -1 * sr * cp;
    }

    if(up) {
        up->x = cr * sp * cy + -sr * -sy;
        up->y = cr * sp * sy + -sr * cy;
        up->z = cr * cp;
    }
}

/* turn forward towards side on the plane defined by forward and side
 * if angle = 90, the result will be equal to side
 * assumes side and forward are perpendicular, and normalized
 * to turn away from side, use a negative angle
 */
static PL_INLINE PLVector3D Math_TurnVector(PLVector3D forward, PLVector3D side, float angle) {
    float scale_forward = cosf((float)MATH_DEG2RAD(angle));
    float scale_side = sinf((float)MATH_DEG2RAD(angle));

    PLVector3D out = {
        scale_forward * forward.x + scale_side * side.x,
        scale_forward * forward.y + scale_side * side.y,
        scale_forward * forward.z + scale_side * side.z
    };
    return out;
}

static PL_INLINE float Math_SnapToEights(float a) {
    a *= 8;
    if(a > 0) {
        a += 0.5f;
    } else {
        a -= 0.5f;
    }

    return 0.125f * (int)a;
}

PL_EXTERN_C_END
