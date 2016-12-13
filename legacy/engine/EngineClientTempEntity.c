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

#include "engine_base.h"

#include "EngineClientTempEntity.h"
#include "EngineVideoParticle.h"
#include "engine_client.h"	// TODO: Remove this.

/*
	Client-side Temporary Entities
*/

int	num_temp_entities;
ClientEntity_t cl_temp_entities[MAX_TEMP_ENTITIES];
beam_t cl_beams[MAX_BEAMS];

ClientEntity_t *CL_NewTempEntity (void)
{
	ClientEntity_t *ent;

	if(cl_numvisedicts == MAX_VISEDICTS ||	num_temp_entities == MAX_TEMP_ENTITIES)
		return NULL;
	ent = &cl_temp_entities[num_temp_entities];
	memset (ent, 0, sizeof(*ent));
	num_temp_entities++;
	cl_visedicts[cl_numvisedicts] = ent;
	cl_numvisedicts++;

	return ent;
}

void CL_UpdateTEnts (void)
{
	int				i, j; //johnfitz -- use j instead of using i twice, so we don't corrupt memory
	beam_t			*b;
	plVector3f_t	dist, org;
	float			d;
	entity_t		*ent;
	float			yaw, pitch;
	float			forward;

	num_temp_entities = 0;

	srand((int)(cl.time*1000)); //johnfitz -- freeze beams when paused

	// update lightning
	for (i=0, b=cl_beams ; i< MAX_BEAMS ; i++, b++)
	{
		if (!b->model || b->endtime < cl.time)
			continue;

		// if coming from the player, update the start position
		if (b->entity == cl.viewentity)
			Math_VectorCopy (cl_entities[cl.viewentity].origin, b->start);

		// calculate pitch and yaw
		Math_VectorSubtract(b->end,b->start,dist);

		if (dist[1] == 0 && dist[0] == 0)
		{
			yaw = 0;
			if (dist[2] > 0)
				pitch = 90;
			else
				pitch = 270;
		}
		else
		{
			yaw = (int) (atan2(dist[1], dist[0]) * 180 / PL_PI);
			if (yaw < 0)
				yaw += 360;

			forward = sqrt (dist[0]*dist[0] + dist[1]*dist[1]);
			pitch = (int)(atan2(dist[2], forward) * 180 / PL_PI);
			if (pitch < 0)
				pitch += 360;
		}

		// Add new entities for the lightning
		Math_VectorCopy (b->start, org);
		d = plVectorNormalize(dist);
		while (d > 0)
		{
			ent = CL_NewTempEntity ();
			if (!ent)
				return;

			Math_VectorCopy (org, ent->origin);
			ent->model		= b->model;
			ent->angles[0]	= pitch;
			ent->angles[1]	= yaw;
			ent->angles[2]	= rand()%360;

			//johnfitz -- use j instead of using i twice, so we don't corrupt memory
			for (j=0 ; j<3 ; j++)
				org[j] += dist[j]*30;
			d -= 30;
		}
	}
}
