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

typedef enum
{
#if 1
	MODEL_TYPE_MD2,		// MD2 support.
	MODEL_TYPE_IQM,		// IQM support.
	MODEL_TYPE_OBJ,		// OBJ support.
#endif
	/*	TODO:
			Eventually I want to convert meshes
			during load time to somewhat of an abstrac
			type, so it's no longer format-specific.
	*/
	MODEL_TYPE_STATIC,		// Static mesh (MD2, U3D, OBJ)
	MODEL_TYPE_SKELETAL,	// Skeletal mesh (IQM)
	MODEL_TYPE_VERTEX,		// Per-vertex animated mesh (MD2, U3D)
	MODEL_TYPE_LEVEL,		// Level / World model.

	MODEL_NONE
} ModelType_t;

//#include "format_u3d.h"	// UNREAL 3D
//#include "format_obj.h"	// OBJ
//#include "format_iqm.h"	// INTER-QUAKE MODEL
#include "format_md2.h"	// MD2
#include "format_bsp.h"	// LEVEL/BSP

// Obsolete

typedef struct
{
	int			nummiptex;
	int			dataofs[4];		// [nummiptex]
} dmiptexlump_t;

typedef struct miptex_s
{
	char		name[16];
	unsigned	width,height;
} miptex_t;

// 0-2 are axial planes
#define	PLANE_X			0
#define	PLANE_Y			1
#define	PLANE_Z			2

// 3-5 are non-axial planes snapped to the nearest
#define	PLANE_ANYX		3
#define	PLANE_ANYY		4
#define	PLANE_ANYZ		5
