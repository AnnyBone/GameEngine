/*	Copyright (C) 2011-2016 OldTimes Software

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

Material_t *mat_blood		= nullptr;
Material_t *mat_bloodcloud	= nullptr;
Material_t *mat_smoke		= nullptr;
Material_t *mat_spark		= nullptr;

ISprite *spr_debug = NULL;

void ClientEffect_Initialize(void)
{
	// Load default set of particles.
	mat_blood		= g_engine->LoadMaterial("particles/blood");
	mat_bloodcloud	= g_engine->LoadMaterial("particles/bloodcloud");
	mat_smoke		= g_engine->LoadMaterial("particles/smoke");
	mat_spark		= g_engine->LoadMaterial("particles/spark");
}

/*
	Sprite Effects
*/

/*
	Particle Effects
*/

/*	Field of particles, useful for forcefields etc.
	TODO: get entity pointer, so we can do this per-model instead.
*/
void ClientEffect_ParticleField(MathVector3f_t position, MathVector3f_t mins, MathVector3f_t maxs, float density)
{
	if (density <= 0)
		return;


}

void ClientEffect_LavaSplash(MathVector3f_t position)
{
	MathVector3f_t	direction;
	Particle_t		*part;
	int				i, j, k;
	float			velocity;

	for (i = -16; i < 16; i++)
		for (j = -16; j < 16; j++)
			for (k = 0; k < 1; k++)
			{
				part = Engine.Client_AllocateParticle();
				if (!part)
					return;

				direction[0] = j*8.0f + (rand() & 7);
				direction[1] = i*8.0f + (rand() & 7);
				direction[2] = 256;

				part->material		= mat_smoke;
				part->skin			= 0;
				part->fScale		= 7.0f;
				part->lifetime		= Client.time + 2.0f + (rand() & 31)*0.02f;
				part->pBehaviour	= PARTICLE_BEHAVIOUR_SLOWGRAVITY;
				part->vOrigin[0]	= position[0] + direction[0];
				part->vOrigin[1]	= position[1] + direction[1];
				part->vOrigin[2]	= position[2] + (rand() & 63);

				velocity = (float)(50 + (rand() & 63));

				plVectorNormalize(direction);
				plVectorScale3fv(direction, velocity, part->vVelocity);
			}
}

void ClientEffect_Teleport(MathVector3f_t position)
{
	Particle_t		*part;
	MathVector3f_t	direction;
	int				i, j, k;
	float			velocity;

	for (i = -16; i < 16; i += 4)
		for (j = -16; j < 16; j += 4)
			for (k = -24; k < 32; k += 4)
			{
				part = Engine.Client_AllocateParticle();
				if (!part)
					return;

				direction[0] = j*8.0f;
				direction[1] = i*8.0f;
				direction[2] = k*8.0f;

				part->material		= mat_spark;
				part->skin			= 0;
				part->fScale		= 7.0f;
				part->lifetime		= Client.time + 2.0f + (rand() & 7)*0.02f;
				part->pBehaviour	= PARTICLE_BEHAVIOUR_SLOWGRAVITY;
				part->vOrigin[0]	= position[0] + i + (rand() & 3);
				part->vOrigin[1]	= position[1] + j + (rand() & 3);
				part->vOrigin[2]	= position[2] + k + (rand() & 3);

				velocity = (float)(50 + (rand() & 63));

				plVectorNormalize(direction);
				Math_VectorScale(direction, velocity, part->vVelocity);
			}

	// TODO: sound
}

void ClientEffect_BloodPuff(MathVector3f_t position)
{
	int			j;
	Particle_t	*part;

	// TODO: support for different blood types

	for (int i = 0; i < 4; i++)
	{
		part = Engine.Client_AllocateParticle();
		if (!part)
			break;

		for (j = 0; j < 3; j++)
			part->vOrigin[j] = position[j] + ((rand() & 15) - 5.0f);

		for (j = 0; j < 3; j++)
			part->vVelocity[j] = (float)((rand() % 512) - 256);

		part->lifetime		= (float)(Client.time + 0.3*(rand() % 5));
		part->pBehaviour	= PARTICLE_BEHAVIOUR_GRAVITY;
		part->fRamp			= (float)(rand() & 4);
		part->fScale		= (float)(rand() % 8 + 2);
		part->material		= mat_blood;
		part->skin			= (rand() % mat_blood->num_skins);
	}
}

void ClientEffect_BloodCloud(MathVector3f_t position, BloodType_t type)
{
	Particle_t *part = g_engine->Client_AllocateParticle();
	if (!part)
		return;

	// TODO: support for different blood types

	plVectorCopy3fv(position, part->vOrigin);

	part->lifetime		= (float)(Client.time + 0.3*(rand() % 5));
	part->fScale		= 20.0f;
	part->material		= mat_blood;
	part->skin			= (rand() % mat_bloodcloud->num_skins);
	part->pBehaviour	= PARTICLE_BEHAVIOUR_STATIC;
}

void ClientEffect_Smoke(MathVector3f_t position)
{
	if (rand() % 5 == 0)
	{
		Particle_t *part = Engine.Client_AllocateParticle();
		if (!part)
			return;

		part->material		= mat_smoke;
		part->skin			= (rand() % mat_smoke->num_skins);
		part->fRamp			= (float)(rand() & 3);
		part->fScale		= ((float)(rand() % 15) * 2);
		part->lifetime		= (Client.time + (rand() % 5));
		part->pBehaviour	= PARTICLE_BEHAVIOUR_SMOKE;

		for (int j = 0; j < 3; j++)
			part->vOrigin[j] = position[j] + ((rand() & 8) - 5.0f);

		Math_VectorClear(part->vVelocity);
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

		part->material		= mat_smoke;
		part->skin			= (rand() % mat_smoke->num_skins);
		part->fScale		= ((float)(rand() % 5)) + 10.0f*10.0f;
		part->lifetime		= Client.time + rand() % 4;
		part->fRamp			= (float)(rand() & 3);
		part->pBehaviour	= PARTICLE_BEHAVIOUR_EXPLODE;

		for (j = 0; j < 3; j++)
		{
			part->vOrigin[j]	= position[j] + (((float)(rand() & 32)) - 16.0f);
			part->vVelocity[j]	= (float)((rand() % 128) - 256);
		}
	}

	// TODO: sound

	light = Engine.Client_AllocateDlight(0);
	if (!light)
		return;

	Math_VectorCopy(position, light->origin);

	light->lightmap		= true;
	light->radius		= 300.0f;
	light->color[0]		= 255.0f;
	light->color[1]		= 255.0f;
	light->color[2]		= 50.0f;
	light->minlight		= 32.0f;
	light->die			= (Client.time + 0.5);
}

void ClientEffect_MuzzleFlash(MathVector3f_t position, MathVector3f_t angles)
{
	DynamicLight_t *light = Engine.Client_AllocateDlight(0);

	Math_VectorCopy(position, light->origin);

	light->origin[2] += 16.0f;

	MathVector3f_t f, r, u;
	plAngleVectors(angles, f, r, u);
	Math_VectorMA(light->origin, 18, f, light->origin);

	light->radius			= 170.0f + (rand() & 31);
	light->color[RED]		= 255.0f;
	light->color[GREEN]		= 255.0f;
	light->color[BLUE]		= 50.0f;
	light->minlight			= 32.0f;
	light->die				= Client.time + 0.1;
	light->lightmap			= true;
}

#ifdef GAME_OPENKATANA
void ClientEffect_IonBallTrail(MathVector3f_t position)
{
	Particle_t	*part;
	int			i, j;

	for (i = 0; i < 4; i++)
	{
		part = Engine.Client_AllocateParticle();
		if (!part)
			break;

		part->material		= mat_spark;
		part->skin			= 0;
		part->fRamp			= (float)(rand() & 3);
		part->fScale		= 1.5f + (float)(rand() % 15 / 10);
		part->lifetime		= (Client.time + (double)(rand() % 2));
		part->pBehaviour	= PARTICLE_BEHAVIOUR_SLOWGRAVITY;

		for (j = 0; j < 3; j++)
			part->vOrigin[j] = position[j] + ((rand() & 15) - 5.0f);

		plVectorClear3fv(part->vVelocity);
	}

	if (!spr_debug)
	{
		spr_debug = g_engine->AddSprite();
		spr_debug->SetMaterial(g_engine->LoadMaterial("debug/debug_sprite"));
	}

	spr_debug->SetPosition(position);
}
#endif