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

#include "game_main.h"

/*
	Main Entry
*/

#include "platform_library.h"

// Client
#include "client.h"

// Server
#include "server_main.h"
#include "server_player.h"
#include "server_debug.h"

GameExport_t	Export;	// Game exports.
ModuleImport_t	Engine;	// Engine imports.
ModuleImport_t	*g_engine;

GameServer_t Server;
GameClient_t Client;

int	iOldGameMode;

int	iRedScore, iBlueScore;

// [2/8/2012] TODO: Why are we doing this!? Should be using the one from the lib ~hogsy
char *va(const char *format,...) {
	va_list		argptr;
	static char	string[1024];

	va_start (argptr, format);
	vsprintf (string, format, argptr);
	va_end (argptr);

	return string;
}

void SetAngle(ServerEntity_t *ent, PLVector3D angle)
{
	ent->v.angles = angle;

	Entity_Link(ent,false);
}

// [8/2/2012] Brought ChangeYaw over from the engine ~hogsy
void ChangeYaw(ServerEntity_t *ent)
{
#if 0
	float ideal,current,move,speed;

	current	= plAngleMod(ent->v.angles[1]);
	ideal	= ent->v.ideal_yaw;
	speed	= ent->v.yaw_speed;

	if(current == ideal)
		return;

	move = ideal - current;
	if(ideal > current)
	{
		if(move >= 180)
			move = move-360;
	}
	else if(move <= -180)
		move = move+360;

	if(move > 0)
	{
		if(move > speed)
			move = speed;
	}
	else if(move < -speed)
		move = -speed;

	ent->v.angles[1] = plAngleMod(current+move);
#endif
}

/*	Used for use tracing and shot
	targeting. Traces are blocked by
	bbox and exact bsp entities, and
	also slide box entities	if the
	tryents flag is set.
*/
trace_t Traceline(ServerEntity_t *ent, PLVector3D vStart, PLVector3D vEnd, int type)
{
	return Engine.Server_Move(vStart, plCreateVector3D(0, 0, 0), plCreateVector3D(0, 0, 0), vEnd, type, ent);
}

/*	Each entity can have eight
	independant sound sources, like
	voice, weapon, feet, etc.
	Channel 0 is an auto-allocate
	channel, the others override
	anything allready running on that
	entity/channel pair.
	An attenuation of 0 will play full
	volume everywhere in the level.
	Larger attenuations will drop off.
*/
void Sound(ServerEntity_t *ent, AudioChannel_t channel, const char *sound, int volume, float attenuation)
{
	g_engine->Sound(ent,(int)channel,sound,volume,attenuation);

	/* TODO
	// [21/3/2012] Revised ~hogsy
	#define SOUND_DEFAULT	0
	#define SOUND_REFERENCE	1

	ServerEntity_t *sound;

	if(type)
		sound = Entity_Spawn();
	else
		return;

	switch(type)
	{
	case SOUND_REFERENCE:
		if(volume >= 0.5f)
		{
			sound->v.classname = "sound_reference";

			SetOrigin(sound,ent->v.origin);

			sound->v.think		= RemoveEntity(ref);
			sound->v.nextthink	= (Server.time+0.01)*volume;
		}
		else
			RemoveEntity(sound);
		break;
	default:
		RemoveEntity(sound);
	}
	*/
}

void WriteByte(int mode,int c)
{
	Engine.WriteByte(mode,c);
}

// OBSOLETE
bool Game_Init(int state,ServerEntity_t *ent,double dTime)
{
	Server.time = dTime;

	switch(state)
	{
	case SERVER_CLIENTPOSTTHINK:
		Player_PostThink(ent);
		break;
	case SERVER_PLAYERPRETHINK:
		Player_PreThink(ent);
		break;
	case SERVER_CLIENTCONNECT:
		Server.iClients++;

		Engine.Server_BroadcastPrint("%s connected\n",ent->v.netname);
		break;
	case SERVER_CLIENTDISCONNECT:
		Server.iClients--;

		Engine.Server_BroadcastPrint("%s disconnected\n",ent->v.netname);

		Entity_Remove(ent);
		break;
	case SERVER_SETCHANGEPARMS:
		if(ent->v.health <= 0)
			break;
		else if(ent->v.health < 50)
			ent->v.health = 50;

		// TODO: Set all necessary parms...
		break;
	case SERVER_SETNEWPARMS:
		if(!Server.bActive)
			Server.bActive = true;

		iRedScore	=
		iBlueScore	= 0;
		break;
	}

	return true;
}

void	Server_PreFrame(void);	// server_main
void	Server_SendClientInformation(ServerClient_t *client);
void	Game_Shutdown(void);

PL_EXPORT GameExport_t *Game_Main(ModuleImport_t *Import)
{
	// Imports...

	Engine = *Import;
	g_engine = &Engine;

	// Exports...

	Export.iVersion		= GAME_VERSION;
	Export.Name			= GAME_NAME;
	Export.ChangeYaw	= ChangeYaw;
	Export.SetSize		= Entity_SetSize;
	Export.Draw			= Client_Draw;
	Export.Game_Init	= Game_Init;
	Export.Shutdown		= Game_Shutdown;

	// Client
	Export.Client_RelinkEntities		= Client_RelinkEntities;
	Export.Client_Initialize			= Client_Initialize;
	Export.Client_ParseTemporaryEntity	= Client_ParseTemporaryEntity;
	Export.Client_ViewFrame				= Client_ViewFrame;

	// Server
	Export.Server_Initialize				= Server_Initialize;
	Export.Server_PreFrame					= Server_PreFrame;
	Export.Server_SpawnEntity				= Server_SpawnEntity;
	Export.Server_EntityFrame				= Server_EntityFrame;
	Export.Server_KillClient				= Server_KillClient;
	Export.Server_SetSizeVector				= Entity_SetSizeVector;
	Export.Server_ParseEntityField			= ServerEntity_ParseField;
	Export.Server_SpawnPlayer				= Player_Spawn;
	Export.Server_SendClientInformation		= Server_SendClientInformation;

	// Debugging
	Export.Server_Draw = Server_Draw;

	// Physics
	Export.Physics_SetGravity				= Physics_SetGravity;
	Export.Physics_CheckWaterTransition		= Physics_CheckWaterTransition;
	Export.Physics_CheckVelocity			= Physics_CheckVelocity;
	Export.Physics_CheckWater				= Physics_CheckWater;
	Export.Physics_WallFriction				= Physics_WallFriction;
	Export.Physics_Impact					= Physics_Impact;
	Export.Physics_PushEntity				= Physics_PushEntity;

	return &Export;
}

/* Called upon engine shutdown.
*/
void Game_Shutdown(void)
{
}
