#ifndef __ENGINEVIDEOSHADOW__
#define __ENGINEVIDEOSHADOW__

#define PARTICLE_DEFAULT		0
#define PARTICLE_LAVASPLASH		1
#define PARTICLE_TELEPORTSPLASH	2

void Shadow_Initialize(void);
void Shadow_Draw(ClientEntity_t *ent);

#endif