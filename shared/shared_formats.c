#include "platform.h"
#include "platform_math.h"

#include "shared_flags.h"
#include "shared_formats.h"

/*
	BSP
*/

// Each individual hull size.
float	vHullSizes[BSP_HULL_MAX][2][3] =
{
	{ { 0, 0, 0 }, { 0, 0, 0 } },

	// Standing
	{ { -16, -16, -36 }, { 16, 16, 36 } },

	// Large
	{ { -32, -32, -32 }, { 32, 32, 32 } },

	// Crouch
	{ { -16, -16, -18 }, { 16, 16, 18 } }
};
