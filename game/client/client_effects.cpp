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

Material *mat_blood		= nullptr;
Material *mat_bloodcloud	= nullptr;
Material *mat_smoke		= nullptr;
Material *mat_spark		= nullptr;

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
void ClientEffect_ParticleField(PLVector3D position, PLVector3D mins, PLVector3D maxs, float density)
{
	if (density <= 0)
		return;


}

void ClientEffect_LavaSplash(PLVector3D position) {
	for (int i = -16; i < 16; i++)
		for (int j = -16; j < 16; j++)
			for (int k = 0; k < 1; k++)
			{
				Particle_t *part = Engine.Client_AllocateParticle();
				if (!part)
					return;

				PLVector3D direction;
				direction[0] = j*8.0f + (rand() & 7);
				direction[1] = i*8.0f + (rand() & 7);
				direction[2] = 256;

				part->material		= mat_smoke;
				part->skin			= 0;
				part->fScale		= 7.0f;
				part->lifetime		= Client.time + 2.0f + (rand() & 31)*0.02f;
				part->pBehaviour	= PARTICLE_BEHAVIOUR_SLOWGRAVITY;
				part->origin[0]	= position[0] + direction[0];
				part->origin[1]	= position[1] + direction[1];
				part->origin[2]	= position[2] + (rand() & 63);

				float velocity = (float)(50 + (rand() & 63));
				direction.Normalize();
                part->velocity = direction * velocity;
			}
}

void ClientEffect_Teleport(PLVector3D position)
{
	for (int i = -16; i < 16; i += 4)
		for (int j = -16; j < 16; j += 4)
			for (int k = -24; k < 32; k += 4)
			{
				Particle_t *part = Engine.Client_AllocateParticle();
				if (!part)
					return;

				PLVector3D direction;
				direction[0] = j*8.0f;
				direction[1] = i*8.0f;
				direction[2] = k*8.0f;

				part->material		= mat_spark;
				part->skin			= 0;
				part->fScale		= 7.0f;
				part->lifetime		= Client.time + 2.0f + (rand() & 7)*0.02f;
				part->pBehaviour	= PARTICLE_BEHAVIOUR_SLOWGRAVITY;
				part->origin[0]	= position[0] + i + (rand() & 3);
				part->origin[1]	= position[1] + j + (rand() & 3);
				part->origin[2]	= position[2] + k + (rand() & 3);

				float velocity = (float)(50 + (rand() & 63));

                direction.Normalize();
                part->velocity = direction * velocity;
			}

	// TODO: sound
}

void ClientEffect_BloodPuff(PLVector3D position)
{
	// TODO: support for different blood types

	for (int i = 0; i < 4; i++)
	{
		Particle_t *part = Engine.Client_AllocateParticle();
		if (!part)
			break;

		for (int j = 0; j < 3; j++)
			part->origin[j] = position[j] + ((rand() & 15) - 5.0f);

		for (int j = 0; j < 3; j++)
			part->velocity[j] = (float)((rand() % 512) - 256);

		part->lifetime		= (float)(Client.time + 0.3*(rand() % 5));
		part->pBehaviour	= PARTICLE_BEHAVIOUR_GRAVITY;
		part->fRamp			= (float)(rand() & 4);
		part->fScale		= (float)(rand() % 8 + 2);
		part->material		= mat_blood;
		part->skin			= (rand() % mat_blood->num_skins);
	}
}

void ClientEffect_BloodCloud(PLVector3D position, BloodType_t type)
{
	Particle_t *part = g_engine->Client_AllocateParticle();
	if (!part)
		return;

	// TODO: support for different blood types

    part->origin        = position;
	part->lifetime		= (float)(Client.time + 0.3*(rand() % 5));
	part->fScale		= 20.0f;
	part->material		= mat_blood;
	part->skin			= (rand() % mat_bloodcloud->num_skins);
	part->pBehaviour	= PARTICLE_BEHAVIOUR_STATIC;
}

void ClientEffect_Smoke(PLVector3D position)
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
			part->origin[j] = position[j] + ((rand() & 8) - 5.0f);

		part->velocity = 0;
	}
}

void ClientEffect_Explosion(PLVector3D position)
{
	for (int i = 0; i < 5; i++)
	{
		Particle_t *part = Engine.Client_AllocateParticle();
		if (!part)
			break;

		part->material		= mat_smoke;
		part->skin			= (rand() % mat_smoke->num_skins);
		part->fScale		= ((float)(rand() % 5)) + 10.0f*10.0f;
		part->lifetime		= Client.time + rand() % 4;
		part->fRamp			= (float)(rand() & 3);
		part->pBehaviour	= PARTICLE_BEHAVIOUR_EXPLODE;

		for (int j = 0; j < 3; j++)
		{
			part->origin[j]	= position[j] + (((float)(rand() & 32)) - 16.0f);
			part->velocity[j]	= (float)((rand() % 128) - 256);
		}
	}

	// TODO: sound

	DynamicLight_t *light = Engine.Client_AllocateDlight(0);
	if (!light)
		return;

    light->origin       = position;
	light->lightmap		= true;
	light->radius		= 300.0f;
	light->color[0]		= 255.0f;
	light->color[1]		= 255.0f;
	light->color[2]		= 50.0f;
	light->minlight		= 32.0f;
	light->die			= (Client.time + 0.5);
}

void ClientEffect_MuzzleFlash(PLVector3D position, PLVector3D angles)
{
	DynamicLight_t *light = Engine.Client_AllocateDlight(0);

    light->origin = position;
	light->origin.z += 16.0f;

	PLVector3D f, r, u;
    Math_AngleVectors(angles, &f, &r, &u);
	Math_VectorMA(light->origin, 18, f, light->origin);

	light->radius			= 170.0f + (rand() & 31);
	light->color[0]		    = 255.0f;
	light->color[1]		    = 255.0f;
	light->color[2]		    = 50.0f;
	light->minlight			= 32.0f;
	light->die				= Client.time + 0.1;
	light->lightmap			= true;
}

#ifdef GAME_OPENKATANA
ISprite *spr_debug = NULL;

void ClientEffect_IonBallTrail(PLVector3D position)
{
	for (int i = 0; i < 4; i++)
	{
		Particle_t *part = Engine.Client_AllocateParticle();
		if (!part)
			break;

		part->material		= mat_spark;
		part->skin			= 0;
		part->fRamp			= (float)(rand() & 3);
		part->fScale		= 1.5f + (float)(rand() % 15 / 10);
		part->lifetime		= (Client.time + (double)(rand() % 2));
		part->pBehaviour	= PARTICLE_BEHAVIOUR_SLOWGRAVITY;

		for (int j = 0; j < 3; j++)
			part->origin[j] = position[j] + ((rand() & 15) - 5.0f);

        part->velocity = 0;
	}

	if (!spr_debug)
	{
		spr_debug = g_engine->AddSprite();
		spr_debug->SetMaterial(g_engine->LoadMaterial("debug/debug_sprite"));
	}

	spr_debug->SetPosition(position);
}
#endif