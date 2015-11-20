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

#include "client.h"

#include "client_effects.h"

/*
	Particle Effects
*/

/*	Field of particles, useful for forcefields etc.
*/
void ClientEffect_ParticleField(MathVector3f_t position, MathVector3f_t mins, MathVector3f_t maxs, float density)
{
	if (density <= 0)
		return;
}

void ClientEffect_BloodSpray(MathVector3f_t position)
{
	int			i, j;
	char		blood[16];
	Particle_t	*part;

	for (i = 0; i < 4; i++)
	{
		part = Engine.Client_AllocateParticle();
		if (!part)
			break;

		// Keep the textures random.
		PARTICLE_BLOOD(blood);

		for (j = 0; j < 3; j++)
			part->vOrigin[j] = position[j] + ((rand() & 15) - 5.0f);

		for (j = 0; j < 3; j++)
			part->vVelocity[j] = (float)((rand() % 512) - 256);

		part->lifetime		= (float)(Client.time + 0.3*(rand() % 5));
		part->pBehaviour	= PARTICLE_BEHAVIOUR_GRAVITY;
		part->fRamp			= (float)(rand() & 4);
		part->fScale		= (float)(rand() % 8 + 2);
		part->iMaterial		= Engine.Client_GetEffect(blood);
	}
}

void ClientEffect_Explosion(MathVector3f_t position)
{
	int				i, j;
	Particle_t		*part;
	DynamicLight_t	*light;	

	for (i = 0; i < 5; i++)
	{
		part = Engine.Client_AllocateParticle();
		if (!part)
			break;

		part->iMaterial = Engine.Client_GetEffect(va("smoke%i", rand() % 4));
		part->fScale = ((float)(rand() % 5)) + 10.0f*10.0f;
		part->lifetime = Client.time + rand() % 4;
		part->fRamp = (float)(rand() & 3);
		part->pBehaviour = PARTICLE_BEHAVIOUR_EXPLODE;

		for (j = 0; j < 3; j++)
		{
			part->vOrigin[j] = position[j] + (((float)(rand() & 32)) - 16.0f);
			part->vVelocity[j] = (float)((rand() % 50) * 2);	// TODO: THIS IS STUPID!! HOLY FUCK.
		}
	}

	light = Engine.Client_AllocateDlight(0);
	if (!light)
		return;

	light->lightmap = true;
	light->radius = 300.0f;
	light->color[0] = 255.0f;
	light->color[1] = 255.0f;
	light->color[2] = 50.0f;
	light->minlight = 32.0f;
	light->die = (Client.time + 0.5);
}