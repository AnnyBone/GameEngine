/*	Copyright (C) 1996-2001 Id Software, Inc.
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

#include "EngineVideoParticle.h"
#include "video.h"

/*
	Particle System
*/

Particle_t	*pActiveParticles,
			*pFreeParticles,
			*pParticles;

ConsoleVariable_t cv_video_drawparticles = { "video_particles", "1", true, false, "Toggles the rendering of particles." };	// TODO: Move into enginevideo!

void Particle_Initialize(void)
{
	pParticles = (Particle_t*)Hunk_AllocName(MAX_PARTICLES*sizeof(Particle_t), "particles");

	Cvar_RegisterVariable(&cv_video_drawparticles, NULL);
}

Particle_t *Particle_Allocate(void)
{
	Particle_t *pParticle;

	// Don't allocate if the client is paused or we're within the console.
	if (cl.paused || ((key_dest == key_console) && (svs.maxclients == 1)))
		return NULL;

	if (!pFreeParticles)
	{
		Con_Warning("Failed to allocate particle!\n");
		return NULL;
	}

	pParticle = pFreeParticles;
	pFreeParticles = pParticle->next;
	pParticle->next = pActiveParticles;
	pActiveParticles = pParticle;

	pParticle->colour = PLColour(PL_COLOUR_WHITE);

	return pParticle;
}

void Particle_CreateEffect(
        ParticleType_t pType,
        ParticleBehaviour_t pBehaviour,
        PLVector3D vOrigin,
        PLVector3D vDirection,
        float fVelocity,
        float fScale,
        int iCount,
        int iMaterial)
{
	Particle_t	*pParticle;
	int			i, j, k;

	switch (pType)
	{
	case PARTICLE_TYPE_LAVASPLASH:
		for (i = -16; i < 16; i++)
			for (j = -16; j < 16; j++)
				for (k = 0; k < 1; k++)
				{
					pParticle = Particle_Allocate();
					if (!pParticle)
						return;

					vDirection[0] = ((float)j)*8.0f + (float)(rand() & 7);
					vDirection[1] = ((float)i)*8.0f + (float)(rand() & 7);
					vDirection[2] = 256.0f;

					fVelocity = 50.0f + (float)(rand() & 63);

					pParticle->lifetime = cl.time + 2.0f + (rand() & 31)*0.02f;
					pParticle->pBehaviour = PARTICLE_BEHAVIOUR_SLOWGRAVITY;
					//pParticle->iMaterial = iMaterial;

					Math_VectorAdd(vOrigin, vDirection, pParticle->origin);
                    vDirection.Normalize();
                    pParticle->velocity = vDirection * fVelocity;
				}
		break;
	case PARTICLE_TYPE_TELEPORTSPLASH:
		for (i = -16; i < 16; i += 4)
			for (j = -16; j < 16; j += 4)
				for (k = -24; k < 32; k += 4)
				{
					pParticle = Particle_Allocate();
					if (!pParticle)
						return;

					vDirection[0] = j * 8;
					vDirection[1] = i * 8;
					vDirection[2] = k * 8;

					fVelocity = 50 + (rand() & 63);

					pParticle->lifetime = cl.time + 0.2f + (rand() & 7)*0.02f;
					pParticle->pBehaviour = PARTICLE_BEHAVIOUR_SLOWGRAVITY;
					pParticle->origin[0] = vOrigin[0] + i + (rand() & 3);
					pParticle->origin[1] = vOrigin[1] + j + (rand() & 3);
					pParticle->origin[2] = vOrigin[2] + k + (rand() & 3);

                    vDirection.Normalize();
					Math_VectorScale(vDirection, fVelocity, pParticle->velocity);
				}
		break;
	case PARTICLE_TYPE_DEFAULT:
		for (i = 0; i < iCount; i++)
		{
			pParticle = Particle_Allocate();
			if (!pParticle)
				return;

			//pParticle->iMaterial = iMaterial;
			pParticle->fScale = fScale;
			pParticle->lifetime = cl.time + 0.1*(rand() % 5);
			pParticle->pBehaviour = pBehaviour;

			// TODO: Simplify this.
			for (j = 0; j < 3; j++)
				pParticle->origin[j] = vOrigin[j] + ((rand() & 15) - 8);

			Math_VectorScale(vDirection, 15, pParticle->velocity);
		}
		break;
	default:
		Con_Warning("Unknown particle type (%i)!\n", pType);
	}
}

void Particle_RocketTrail(PLVector3D start, PLVector3D end, int type)
{
#if 0
	PLVector3D		vec;
	float				len;
	int					j;
	Particle_t			*p;
	int					dec;
	static int			tracercount;

	Math_VectorSubtract(end, start, vec);
	len = plVectorNormalize(vec);
	if (type < 128)
		dec = 3;
	else
	{
		dec = 1;
		type -= 128;
	}

	while (len > 0)
	{
		len -= dec;

		p = Particle_Allocate();

		Math_VectorCopy(pl_origin3f, p->velocity);
		p->lifetime = cl.time + 2;

		switch (type)
		{
		case 0:	// rocket trail
			p->fRamp = (rand() & 3);
			p->pBehaviour = PARTICLE_BEHAVIOUR_FIRE;

			for (j = 0; j<3; j++)
				p->origin[j] = start[j] + ((rand() % 6) - 3);
			break;
		case 1:	// smoke smoke
			p->fRamp = (rand() & 3) + 2;
			p->pBehaviour = PARTICLE_BEHAVIOUR_FIRE;

			for (j = 0; j<3; j++)
				p->origin[j] = start[j] + ((rand() % 6) - 3);
			break;
		case 2:	// blood
			p->pBehaviour = PARTICLE_BEHAVIOUR_GRAVITY;
			for (j = 0; j < 3; j++)
				p->origin[j] = start[j] + ((rand() % 6) - 3);
			break;
		case 3:
		case 5:	// tracer
			p->lifetime = cl.time + 0.5;
			p->pBehaviour = PARTICLE_BEHAVIOUR_STATIC;

			Math_VectorCopy(start, p->origin);

			tracercount++;
			if (tracercount & 1)
			{
				p->velocity[0] = 30 * vec[1];
				p->velocity[1] = 30 * -vec[0];
			}
			else
			{
				p->velocity[0] = 30 * -vec[1];
				p->velocity[1] = 30 * vec[0];
			}
			break;
		case 4:	// slight blood
			p->pBehaviour = PARTICLE_BEHAVIOUR_GRAVITY;

			for (j = 0; j<3; j++)
				p->origin[j] = start[j] + ((rand() % 6) - 3);

			len -= 3;
			break;
		case 6:	// voor trail
			p->pBehaviour = PARTICLE_BEHAVIOUR_STATIC;
			p->lifetime = cl.time + 0.3f;

			for (j = 0; j<3; j++)
				p->origin[j] = start[j] + ((rand() & 15) - 8);
			break;
		}

		Math_VectorAdd(start, vec, start);
	}
#endif
}

void Particle_Parse(void)
{
	PLVector3D	vOrigin, vDirection;
	int				i, iCount, iMaterial;
	float			fScale;

	for (i = 0; i < 3; i++)
		vOrigin[i] = MSG_ReadCoord();

	for (i = 0; i < 3; i++)
		vDirection[i] = MSG_ReadChar()*(1.0f / 16.0f);

	fScale = MSG_ReadFloat();

	iMaterial = MSG_ReadByte();
	iCount = MSG_ReadByte();

	Particle_CreateEffect(
		PARTICLE_TYPE_DEFAULT,
		PARTICLE_BEHAVIOUR_SLOWGRAVITY,
		vOrigin,
		vDirection,
		0.5f,
		fScale,
		iCount,
		iMaterial);
}

#if 0
#define NUMVERTEXNORMALS	162

plVector3f_t	avelocities[NUMVERTEXNORMALS], avelocity = { 23, 7, 3 };

float	beamlength = 16;

void Client_EntityParticles(entity_t *ent)
{
	int					i;
	Particle_t			*p;
	float				angle,
						sr, sp, sy, cr, cp, cy,
						dist = 64.0f;
	plVector3f_t		forward;

	if (!avelocities[0][0])
		for (i = 0; i<NUMVERTEXNORMALS * 3; i++)
			avelocities[0][i] = (rand() & 255)*0.01f;

	for (i = 0; i<NUMVERTEXNORMALS; i++)
	{
		angle = cl.time*avelocities[i][0];
		sy = sin(angle);
		cy = cos(angle);
		angle = cl.time*avelocities[i][1];
		sp = sin(angle);
		cp = cos(angle);
		angle = cl.time*avelocities[i][2];
		sr = sin(angle);
		cr = cos(angle);

		forward[0] = cp*cy;
		forward[1] = cp*sy;
		forward[2] = -sp;

		if (!pFreeParticles)
			return;

		p = pFreeParticles;
		pFreeParticles = p->next;
		p->next = active_particles;
		active_particles = p;

		p->die = cl.time + 0.01f;
		p->pBehaviour = PARTICLE_BEHAVIOUR_EXPLODE;
		p->org[0] = ent->origin[0] + r_avertexnormals[i][0] * dist + forward[0] * beamlength;
		p->org[1] = ent->origin[1] + r_avertexnormals[i][1] * dist + forward[1] * beamlength;
		p->org[2] = ent->origin[2] + r_avertexnormals[i][2] * dist + forward[2] * beamlength;
	}
}
#endif

/*	Clear all particles.
*/
void Particle_ClearAll(void)
{
	int	i;

	pFreeParticles = &pParticles[0];
	pActiveParticles = NULL;

	for (i = 0; i < MAX_PARTICLES; i++)
		pParticles[i].next = &pParticles[i + 1];

	pParticles[MAX_PARTICLES - 1].next = NULL;
}

void ParticleManager_Simulate(void) {
	// Don't continue if there aren't any active particles.
	if (!pActiveParticles)
		return;

	// Don't continue if the client is either paused or in the console.
	if (cl.paused || ((key_dest == key_console) && (svs.maxclients == 1)))
		return;

	double time1 = cl.time - cl.oldtime, time2[3];
	time2[2] = time1 * 15;
	time2[1] = time1 * 10;
	time2[0] = time1 * 5;

	// TODO: Alternative? Both variables are stored in the game module...
	float actual_gravity = (
            Cvar_VariableValue("server_gravity") *
		    Cvar_VariableValue("server_gravityamount")
    );

	float gravity = ((float)time1) * actual_gravity;
	float slow_gravity = ((float)time1) * actual_gravity * 0.05f;
	float velocity = 4 * ((float)time1);

    Particle_t *kill;
	for (;;) {
		kill = pActiveParticles;
		if (kill && (kill->lifetime < cl.time))
		{
			pActiveParticles = kill->next;
			kill->next = pFreeParticles;
			pFreeParticles = kill;

			continue;
		}
		break;
	}

	for (Particle_t *particle = pActiveParticles; particle; particle = particle->next) {
		for (;;) {
			kill = particle->next;
			if (kill && (kill->lifetime < cl.time))
			{
				particle->next = kill->next;
				kill->next = pFreeParticles;
				pFreeParticles = kill;

				continue;
			}

			break;
		}

		// Update the particles movement.
		for (int i = 0; i < 3; i++) {
            particle->origin[i] += particle->velocity[i] * time1;
        }

		// Keep the alpha updated.
		particle->colour.a = plFloatToByte(Math_Clamp(0, particle->lifetime - cl.time, 1.0f));

		switch (particle->pBehaviour)
		{
		case PARTICLE_BEHAVIOUR_SMOKE:
			particle->fScale += time2[2];
			particle->velocity[2] += slow_gravity / 10.0f;
			break;
		case PARTICLE_BEHAVIOUR_FIRE:
			particle->fRamp += time2[0];
			if (particle->fRamp >= 6.0f)
				particle->lifetime = -1.0;

			particle->velocity[2] += gravity;
			break;
		case PARTICLE_BEHAVIOUR_EXPLODE:
			particle->fRamp += time2[1];
			if (particle->fRamp >= 8.0f)
				particle->lifetime = -1.0;

			for (int i = 0; i < 3; i++)
				particle->velocity[i] += particle->velocity[i] * velocity;

			particle->velocity[2] -= slow_gravity;
			break;
		case PARTICLE_BEHAVIOUR_GRAVITY:
			particle->velocity[2] -= gravity;
			break;
		case PARTICLE_BEHAVIOUR_SLOWGRAVITY:
			particle->velocity[2] -= slow_gravity;
			break;
		case PARTICLE_BLOB: {
            for (int i = 0; i < 3; i++)
                particle->velocity[i] += particle->velocity[i] * velocity;

            particle->velocity[2] -= gravity;
            break;
        }
		case PARTICLE_BEHAVIOUR_STATIC:
		default:
			break;
		}
	}
}

void Particle_Draw(void) {
	if (!pActiveParticles || !cv_video_drawparticles.value) {
        return;
    }

	core::Camera *camera = g_cameramanager->GetCurrentCamera();
	if (!camera) {
        return;
    }

	for (Particle_t *particle = pActiveParticles; particle; particle = particle->next) {
		PLVertex fill[4] = { 0 };

		Video_ObjectVertex(&fill[0], particle->origin.x, particle->origin.y, particle->origin.z);
		Video_ObjectColour(&fill[0], particle->colour.r, particle->colour.g, particle->colour.b, particle->colour.a);
		Video_ObjectTexture(&fill[0], 0, 0, 0);

		Math_VectorMA(particle->origin, particle->fScale, camera->GetUp(), fill[1].position);
		Video_ObjectColour(&fill[1], particle->colour.r, particle->colour.g, particle->colour.b, particle->colour.a);
		Video_ObjectTexture(&fill[1], 0, 1.0f, 0);

		Math_VectorMA(fill[1].position, particle->fScale, camera->GetRight(), fill[2].position);
		Video_ObjectColour(&fill[2], particle->colour.r, particle->colour.g, particle->colour.b, particle->colour.a);
		Video_ObjectTexture(&fill[2], 0, 1.0f, 1.0f);

		Math_VectorMA(particle->origin, particle->fScale, camera->GetRight(), fill[3].position);
		Video_ObjectColour(&fill[3], particle->colour.r, particle->colour.g, particle->colour.b, particle->colour.a);
		Video_ObjectTexture(&fill[3], 0, 0, 1.0f);

		Video_DrawFill(fill, particle->material, particle->skin);

		rs_particles++;
	}
}
