/*	Copyright (C) 2011-2015 OldTimes Software

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
