/*	Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef __ENGINEPARTICLE__
#define	__ENGINEPARTICLE__

Particle_t *Particle_Allocate(void);

void Particle_Initialize(void);
void Particle_CreateEffect(ParticleType_t pType, ParticleBehaviour_t pBehaviour, MathVector3_t vOrigin, MathVector3_t vDirection, float fVelocity, float fScale, int iCount, int iMaterial);
void Particle_Parse(void);
void Particle_ClearAll(void);
void Particle_Frame(void);

#endif