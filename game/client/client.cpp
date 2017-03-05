/*
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

#include "client.h"

#include "client_player.h"
#include "client_effects.h"

/*
	Only functions that are called by the engine should
	exist in here.
*/

/*	Initialize any client-side specific variables.
*/
void Client_Initialize(void) {
	ClientEffect_Initialize();
	ClientPlayer_Initialize();
}

/*	Crappy the sticky renderer loop!
	This lets us draw images and text to the screen... Forcefully.
*/
void Client_Draw(void) {
}

typedef struct {
	int ident;

	void(*Function)();
} ClientMessageFunction_t;

ClientMessageFunction_t stochandles[]=
{
	{ STOC_SPAWNSPRITE, }
};

/*	Parse messages from the server.
*/
void Client_ParseServerMessage(int cmd)
{
	switch (cmd)
	{
		
	}
}

/*	Parse temporary entity types, useful for particles and other small crap.
*/
void Client_ParseTemporaryEntity(void) {
	int	type;

	// Set type, so if the type is missing we can mention it below.
	type = g_engine->ReadByte();
	switch (type)
	{
		case CTE_EXPLOSION:
		{
			PLVector3D	position;
			for(int i = 0; i < 3; i++)
				position[i] = g_engine->ReadCoord();

			ClientEffect_Explosion(position);
		}
		break;

		case CTE_BLOODPUFF:
		{
			PLVector3D	position;
			for(int i = 0; i < 3; i++)
				position[i] = g_engine->ReadCoord();

			ClientEffect_BloodPuff(position);
		}
		break;

		case CTE_BLOODCLOUD:
		{
			PLVector3D position;
			for (int i = 0; i < 3; i++)
				position[i] = g_engine->ReadCoord();

			ClientEffect_BloodCloud(position, (BloodType_t)g_engine->ReadByte());
		}
		break;

		case CTE_PARTICLE_FIELD:
		{
			PLVector3D position;
			for (int i = 0; i < 3; i++)
				position[i] = g_engine->ReadCoord();

			PLVector3D mins;
			for (int i = 0; i < 3; i++)
				mins[i] = g_engine->ReadCoord();

			PLVector3D maxs;
			for (int i = 0; i < 3; i++)
				maxs[i] = g_engine->ReadCoord();

			float density = g_engine->ReadFloat();

			ClientEffect_ParticleField(position, mins, maxs, density);
		}
		break;

		case CTE_MUZZLEFLASH:
		{
			PLVector3D position, angles;
			for (int i = 0; i < 3; i++)
				position[i] = g_engine->ReadCoord();

			for (int i = 0; i < 3; i++)
				angles[i] = g_engine->ReadAngle();

			ClientEffect_MuzzleFlash(position, angles);
		}
		break;

		default:
			Engine.Con_Warning("Unknown temporary entity type! (%i)\n", type);
	}
}

PL_EXTERN_C

/*	Called by the engine.
*/
void Client_RelinkEntities(ClientEntity_t *entity, int i, double time) {
	DynamicLight_t	*light;

	// TODO: Move over to somewhere more appropriate please
	Client.time = time;

	if (entity->effects & EF_MOTION_FLOAT)
		entity->origin[2] += ((float)sin(Client.time*2.0f))*5.0f;

	if (entity->effects & EF_MOTION_ROTATE)
		entity->angles[PL_YAW] = Math_AngleMod((float)(100.0*Client.time));

	if (entity->effects & EF_PARTICLE_SMOKE)
		ClientEffect_Smoke(entity->origin);

	if (entity->effects & EF_PARTICLE_BLOOD)
		ClientEffect_BloodPuff(entity->origin);

	if (entity->effects & EF_LIGHT_GREEN) {
#ifdef GAME_OPENKATANA
		light = Engine.Client_AllocateDlight(i);
		light->origin 		    = entity->origin;
		light->radius		    = (float)(rand() % 20) * 10;
		light->color[PL_RED]	= 0;
		light->color[PL_GREEN] 	= 255.0f;
		light->color[PL_BLUE]	= 0;
		light->minlight		    = 16.0f;
		light->die			    = (float)(Client.time + 0.01);
		light->lightmap		    = true;

		ClientEffect_IonBallTrail(entity->origin);
#endif
	}

	if (entity->effects & EF_LIGHT_BLUE) {
		light = Engine.Client_AllocateDlight(i);
        light->origin           = entity->origin;
		light->radius			= 120.0f;
		light->color[PL_RED]	= 0;
		light->color[PL_GREEN]	= 0;
		light->color[PL_BLUE]	= 255.0f;
		light->minlight			= 32.0f;
		light->die				= (Client.time+0.01);
		light->lightmap			= true;
	}

	if (entity->effects & EF_LIGHT_RED) {
		light = Engine.Client_AllocateDlight(i);
        light->origin           = entity->origin;
		light->radius			= 120.0f;
		light->color		    = PLVector3D(255, 0, 0);
		light->minlight			= 32.0f;
		light->die				= Client.time + 0.01;
		light->lightmap			= true;
	}

	if (entity->effects & EF_DIMLIGHT) {
		light = Engine.Client_AllocateDlight(i);
        light->origin       = entity->origin;
		light->radius		= 200.0f;
		light->color	    = 255.0f;
		light->minlight		= 32.0f;
		light->die			= Client.time + 0.01;
		light->lightmap		= true;
	}

	if (entity->effects & EF_BRIGHTLIGHT) {
		light = Engine.Client_AllocateDlight(i);
        light->origin       = entity->origin;
        light->color        = 255;
		light->origin[2]	+= 16.0f;
		light->radius		= 300.0f + (rand() & 31);
		light->minlight		= 32.0f;
		light->die			= Client.time + 0.001;
		light->lightmap		= true;
	}

	if (entity->effects & EF_GLOW_RED)
	{
		light = Engine.Client_AllocateDlight(i);

        light->origin       = entity->origin;
		light->radius		= (float)sin(Client.time*2.0f)*100.0f;
		light->color	    = PLVector3D(255, 0, 0);
		light->minlight		= 16.0f;
		light->die			= Client.time + 0.01f;
		light->lightmap		= true;
	}

/*	if(ent->effects & EF_GLOW_GREEN)
	{
		dLight = Engine.Client_AllocateDlight(i);

		Math_VectorCopy(ent->origin,dLight->origin);

		dLight->radius			= (float)sin(Client.time*2.0f)*100.0f;
		dLight->color[RED]		= 0;
		dLight->color[GREEN]	= 255.0f;
		dLight->color[BLUE]		= 0;
		dLight->minlight		= 16.0f;
		dLight->die				= Client.time+0.01f;
		dLight->lightmap		= true;
	}*/

	if (entity->effects & EF_GLOW_BLUE)
	{
		light = Engine.Client_AllocateDlight(i);

        light->origin       = entity->origin;
		light->radius		= (float)sin(Client.time*2.0f)*100.0f;
		light->color        = PLVector3D(0, 0, 255);
		light->minlight		= 16.0f;
		light->die			= (Client.time + 0.01);
		light->lightmap		= true;
	}

	if (entity->effects & EF_GLOW_WHITE)
	{
		light = Engine.Client_AllocateDlight(i);

        light->origin       = entity->origin;
        light->color        = 255;
		light->radius		= (float)sin(Client.time*2.0f)*100.0f;
		light->minlight		= 16.0f;
		light->die			= (Client.time + 0.01);
		light->lightmap		= true;
	}

	entity->bForceLink = false;
}

PL_EXTERN_C_END