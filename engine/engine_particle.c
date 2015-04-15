/*	Copyright (C) 2011-2015 OldTimes Software
*/
#include "quakedef.h"

#include "engine_particle.h"

#include "video.h"
#include "engine_videoshadow.h"

#if 0
/*
	Particle System
*/

class CSprite
{
public:
protected:
private:
};

class CParticle : public CSprite
{
public:
protected:
private:
};
#endif

/*
	Legacy Particle System
*/

Particle_t	*pActiveParticles,
			*pFreeParticles,
			*pParticles;

cvar_t	cvParticleDraw = { "video_particles", "1", true, false, "Toggles the rendering of particles." };

void Particle_Initialize(void)
{
	pParticles = (Particle_t*)Hunk_AllocName(MAX_PARTICLES*sizeof(Particle_t), "particles");

	Cvar_RegisterVariable(&cvParticleDraw, NULL);
}

Particle_t *Particle_Allocate(void)
{
	Particle_t *pParticle;

	// Don't allocate if the client is paused or we're within the console.
	if (cl.bIsPaused || ((key_dest == key_console) && (svs.maxclients == 1)))
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

	Math_Vector4Set(1.0f, pParticle->vColour);

	return pParticle;
}

void Particle_CreateEffect(
	ParticleType_t pType,
	ParticleBehaviour_t pBehaviour,
	MathVector3_t vOrigin,
	MathVector3_t vDirection,
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

					// [5/9/2012] Keep this pre-defined? ~hogsy
					fVelocity = 50.0f + (float)(rand() & 63);

					pParticle->fDie = cl.time + 2.0f + (rand() & 31)*0.02f;
					pParticle->pBehaviour = PARTICLE_BEHAVIOUR_SLOWGRAVITY;
					pParticle->iMaterial = iMaterial;

					Math_VectorAdd(vOrigin, vDirection, pParticle->vOrigin);
					Math_VectorNormalize(vDirection);
					Math_VectorScale(vDirection, fVelocity, pParticle->vVelocity);
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

					pParticle->fDie = cl.time + 0.2f + (rand() & 7)*0.02f;
					pParticle->pBehaviour = PARTICLE_BEHAVIOUR_SLOWGRAVITY;
					pParticle->vOrigin[0] = vOrigin[0] + i + (rand() & 3);
					pParticle->vOrigin[1] = vOrigin[1] + j + (rand() & 3);
					pParticle->vOrigin[2] = vOrigin[2] + k + (rand() & 3);

					Math_VectorNormalize(vDirection);
					Math_VectorScale(vDirection, fVelocity, pParticle->vVelocity);
				}
		break;
	case PARTICLE_TYPE_DEFAULT:
		for (i = 0; i < iCount; i++)
		{
			pParticle = Particle_Allocate();
			if (!pParticle)
				return;

			pParticle->iMaterial = iMaterial;
			pParticle->fScale = fScale;
			pParticle->fDie = cl.time + 0.1f*(rand() % 5);
			pParticle->pBehaviour = pBehaviour;

			// [5/9/2012] TODO: Simplify this ~hogsy
			for (j = 0; j < 3; j++)
				pParticle->vOrigin[j] = vOrigin[j] + ((rand() & 15) - 8);

			Math_VectorScale(vDirection, 15, pParticle->vVelocity);
		}
		break;
	default:
		Con_Warning("Unknown particle type (%i)!\n", pType);
	}
}

void Particle_RocketTrail(vec3_t start, vec3_t end, int type)
{
	vec3_t		vec;
	float		len;
	int			j;
	Particle_t	*p;
	int			dec;
	static int	tracercount;

	Math_VectorSubtract(end, start, vec);
	len = Math_VectorNormalize(vec);
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

		Math_VectorCopy(mv3Origin, p->vVelocity);
		p->fDie = cl.time + 2;

		switch (type)
		{
		case 0:	// rocket trail
			p->fRamp = (rand() & 3);
			p->pBehaviour = PARTICLE_BEHAVIOUR_FIRE;

			for (j = 0; j<3; j++)
				p->vOrigin[j] = start[j] + ((rand() % 6) - 3);
			break;
		case 1:	// smoke smoke
			p->fRamp = (rand() & 3) + 2;
			p->pBehaviour = PARTICLE_BEHAVIOUR_FIRE;

			for (j = 0; j<3; j++)
				p->vOrigin[j] = start[j] + ((rand() % 6) - 3);
			break;
		case 2:	// blood
			p->pBehaviour = PARTICLE_BEHAVIOUR_GRAVITY;
			for (j = 0; j < 3; j++)
				p->vOrigin[j] = start[j] + ((rand() % 6) - 3);
			break;
		case 3:
		case 5:	// tracer
			p->fDie = cl.time + 0.5;
			p->pBehaviour = PARTICLE_BEHAVIOUR_STATIC;

			Math_VectorCopy(start, p->vOrigin);

			tracercount++;
			if (tracercount & 1)
			{
				p->vVelocity[0] = 30 * vec[1];
				p->vVelocity[1] = 30 * -vec[0];
			}
			else
			{
				p->vVelocity[0] = 30 * -vec[1];
				p->vVelocity[1] = 30 * vec[0];
			}
			break;
		case 4:	// slight blood
			p->pBehaviour = PARTICLE_BEHAVIOUR_GRAVITY;

			for (j = 0; j<3; j++)
				p->vOrigin[j] = start[j] + ((rand() % 6) - 3);

			len -= 3;
			break;
		case 6:	// voor trail
			p->pBehaviour = PARTICLE_BEHAVIOUR_STATIC;
			p->fDie = cl.time + 0.3f;

			for (j = 0; j<3; j++)
				p->vOrigin[j] = start[j] + ((rand() & 15) - 8);
			break;
		}

		Math_VectorAdd(start, vec, start);
	}
}

void Particle_Parse(void)
{
	MathVector3_t	vOrigin, vDirection;
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

vec3_t	avelocities[NUMVERTEXNORMALS], avelocity = { 23, 7, 3 };

float	beamlength = 16;

void Client_EntityParticles(entity_t *ent)
{
	int			i;
	Particle_t	*p;
	float		angle,
		sr, sp, sy, cr, cp, cy,
		dist = 64.0f;
	vec3_t		forward;

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

void Particle_Frame(void)
{
	Particle_t	*pParticle, *pKill;
	float		fFrameTime, fDVelocity,
				fTime[3],
				fGravity,fActualGravity,fSlowGravity;
	int			i;

	// Don't continue if there aren't any active particles.
	if (!pActiveParticles)
		return;

	// Don't continue if the client is either paused or in the console.
	if (cl.bIsPaused || ((key_dest == key_console) && (svs.maxclients == 1)))
		return;

	fFrameTime = cl.time - cl.oldtime;

	fTime[2] = fFrameTime*15.0f;
	fTime[1] = fFrameTime*10.0f;
	fTime[0] = fFrameTime*5.0f;

	// TODO: Alternative? Both variables are stored in the game module...
	fActualGravity = (	Cvar_VariableValue("server_gravity")*
						Cvar_VariableValue("server_gravityamount"));

	fGravity = fFrameTime*fActualGravity;
	fSlowGravity = fFrameTime*fActualGravity*0.05f;

	fDVelocity = 4.0f*fFrameTime;

	for (;;)
	{
		pKill = pActiveParticles;
		if (pKill && (pKill->fDie < cl.time))
		{
			pActiveParticles = pKill->next;
			pKill->next = pFreeParticles;
			pFreeParticles = pKill;

			continue;
		}

		break;
	}

	for (pParticle = pActiveParticles; pParticle; pParticle = pParticle->next)
	{
		for (;;)
		{
			pKill = pParticle->next;
			if (pKill && (pKill->fDie < cl.time))
			{
				pParticle->next = pKill->next;
				pKill->next = pFreeParticles;
				pFreeParticles = pKill;

				continue;
			}

			break;
		}

		// Update the particles movement.
		for (i = 0; i < 3; i++)
			pParticle->vOrigin[i] += pParticle->vVelocity[i] * fFrameTime;

		// Keep the alpha updated.
		pParticle->vColour[pALPHA] = Math_Clamp(0, pParticle->fDie - cl.time, 1.0f);

		// Lighting support was here, but for now it's being left out since I want to implement a better solution.

		switch (pParticle->pBehaviour)
		{
		case PARTICLE_BEHAVIOUR_SMOKE:
			pParticle->fScale += fTime[2];
			pParticle->vVelocity[2] += fSlowGravity / 10.0f;
			break;
		case PARTICLE_BEHAVIOUR_FIRE:
			pParticle->fRamp += fTime[0];
			if (pParticle->fRamp >= 6.0f)
				pParticle->fDie = -1.0f;

			pParticle->vVelocity[2] += fGravity;
			break;
		case PARTICLE_BEHAVIOUR_EXPLODE:
			pParticle->fRamp += fTime[1];
			if (pParticle->fRamp >= 8.0f)
				pParticle->fDie = -1.0f;

			for (i = 0; i < 3; i++)
				pParticle->vVelocity[i] += pParticle->vVelocity[i] * fDVelocity;

			pParticle->vVelocity[2] -= fSlowGravity;
			break;
		case PARTICLE_BEHAVIOUR_GRAVITY:
			pParticle->vVelocity[2] -= fGravity;
			break;
		case PARTICLE_BEHAVIOUR_SLOWGRAVITY:
			pParticle->vVelocity[2] -= fSlowGravity;
			break;
		case PARTICLE_BLOB:
			for (i = 0; i < 3; i++)
				pParticle->vVelocity[i] += pParticle->vVelocity[i] * fDVelocity;

			pParticle->vVelocity[2] -= fGravity;
			break;
		case PARTICLE_BEHAVIOUR_STATIC:
		default:
			break;
		}
	}
}

void Particle_Draw(void)
{
	Particle_t	*pParticle;

	if (!pActiveParticles || !cvParticleDraw.value)
		return;

	Video_ResetCapabilities(false);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	Video_SetBlend(VIDEO_BLEND_IGNORE, VIDEO_DEPTH_FALSE);
	Video_EnableCapabilities(VIDEO_BLEND | VIDEO_TEXTURE_2D);

	for (pParticle = pActiveParticles; pParticle; pParticle = pParticle->next)
	{
		VideoObjectVertex_t	voParticle[4] = { 0 };

		if (!r_showtris.value)
			Video_SetTexture(gEffectTexture[pParticle->iMaterial]);

		Video_ObjectVertex(&voParticle[0], 
			pParticle->vOrigin[0], pParticle->vOrigin[1], pParticle->vOrigin[2]);
		Video_ObjectColour(&voParticle[0],
			pParticle->vColour[0], pParticle->vColour[1], pParticle->vColour[2], pParticle->vColour[3]);
		Video_ObjectTexture(&voParticle[0], 0, 0, 0);

		Math_VectorMA(pParticle->vOrigin, pParticle->fScale, vup, voParticle[1].mvPosition);
		Video_ObjectColour(&voParticle[1],
			pParticle->vColour[0], pParticle->vColour[1], pParticle->vColour[2], pParticle->vColour[3]);
		Video_ObjectTexture(&voParticle[1], 0, 1.0f, 0);

		Math_VectorMA(voParticle[1].mvPosition, pParticle->fScale, vright, voParticle[2].mvPosition);
		Video_ObjectColour(&voParticle[2],
			pParticle->vColour[0], pParticle->vColour[1], pParticle->vColour[2], pParticle->vColour[3]);
		Video_ObjectTexture(&voParticle[2], 0, 1.0f, 1.0f);

		Math_VectorMA(pParticle->vOrigin, pParticle->fScale, vright, voParticle[3].mvPosition);
		Video_ObjectColour(&voParticle[3],
			pParticle->vColour[0], pParticle->vColour[1], pParticle->vColour[2], pParticle->vColour[3]);
		Video_ObjectTexture(&voParticle[3], 0, 0, 1.0f);

		Video_DrawFill(voParticle, NULL);

		rs_particles++;
	}

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	Video_ResetCapabilities(true);
}
