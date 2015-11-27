/*  Copyright (C) 1996-2001 Id Software, Inc.
    Copyright (C) 2002-2009 John Fitzgibbons and others
    Copyright (C) 2011-2013 OldTimes Software

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
#include "engine_base.h"

cvar_t	chase_back      = { "chase_back",   "100"   };
cvar_t	chase_up        = { "chase_up",     "16"    };
cvar_t	chase_right     = { "chase_right",  "0"     };
cvar_t	chase_active    = { "chase_active", "0"     };

void Chase_Init (void)
{
	Cvar_RegisterVariable(&chase_back, NULL);
	Cvar_RegisterVariable(&chase_up, NULL);
	Cvar_RegisterVariable(&chase_right, NULL);
	Cvar_RegisterVariable(&chase_active, NULL);
}

/*
==============
TraceLine

TODO: impact on bmodels, monsters
==============
*/
void TraceLine (vec3_t start, vec3_t end, vec3_t impact)
{
	trace_t	trace;

	memset (&trace, 0, sizeof(trace));
	SV_RecursiveHullCheck (cl.worldmodel->hulls, 0, 0, 1, start, end, &trace);

	Math_VectorCopy (trace.endpos, impact);
}

// TODO: stay at least 8 units away from all walls in this leaf
void Chase_UpdateForDrawing (void)
{
	int		i;
	vec3_t	forward, up, right;
	vec3_t	ideal, crosshair, temp;

	Math_AngleVectors(cl.viewangles, forward, right, up);

	// calc ideal camera location before checking for walls
	for (i=0 ; i<3 ; i++)
		ideal[i] = cl.viewent.origin[i]
		- forward[i]*chase_back.value
		+ right[i]*chase_right.value;
		//+ up[i]*chase_up.value;
	ideal[2] = cl.viewent.origin[2] + chase_up.value;

	// make sure camera is not in or behind a wall
	TraceLine(r_refdef.vieworg, ideal, temp);
	if(Math_Length(temp) != 0)
		Math_VectorCopy(temp, ideal);

	// place camera
	Math_VectorCopy(ideal, r_refdef.vieworg);

	// find the spot the player is looking at
	Math_VectorMA(cl.viewent.origin, 4096, forward, temp);
	TraceLine(cl.viewent.origin, temp, crosshair);

	// calculate camera angles to look at the same spot
	Math_VectorSubtract(crosshair,r_refdef.vieworg,temp);
	Math_VectorAngles (temp, r_refdef.viewangles);
	if (r_refdef.viewangles[PITCH] == 90 || r_refdef.viewangles[PITCH] == -90)
		r_refdef.viewangles[YAW] = cl.viewangles[YAW];
}

