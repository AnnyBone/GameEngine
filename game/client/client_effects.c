/*	Copyright (C) 2011-2015 OldTimes Software
*/

#include "client_main.h"

#include "client_effects.h"

/*
	Particle Effects
*/

/* Field of particles, useful for forcefields etc.
*/
#if 0
void Client_ParticleField(
	vec3_t vOrigin,
	vec3_t vMins,
	vec3_t vMaxs,
	float fDensity)
{
}
#endif

void Effect_Explosion(MathVector3f_t position)
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