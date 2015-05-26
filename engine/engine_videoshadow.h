#ifndef __KATGL__
#define __KATGL__

#include "quakedef.h"

// [3/9/2012] Included purely for ptype_t ~hogsy
#include "engine_client.h"

#define PARTICLE_DEFAULT		0
#define PARTICLE_LAVASPLASH		1
#define PARTICLE_TELEPORTSPLASH	2

struct	gltexture_s	*gEffectTexture[MAX_EFFECTS];

void	Shadow_Initialize(void);
void	Draw_String(int x, int y, char *msg);
void	Draw_Shadow(entity_t *ent);

#endif