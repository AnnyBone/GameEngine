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

#include "server_mode.h"

/*	
	This is my little "Capture The Flag" gamemode which is pretty much
	based around a single entity (item_flag), this code mainly served as
	a good study for states and such for entities which I hope to take
	advantage of for the AI in OpenKatana. You can pretty much do as you
	please with this code, it should translate back into QuakeC quite easily
	for those who understand it well enough ;)
	It's worth mentioning that there may be some flaws/mistakes, I blame the
	lack of sleep really but I've cleaned and revised it a number of times
	so if you find anything wrong let me know and I'll sort it out in future
	releases. ~hogsy
*/

#include "server_monster.h"
#include "server_item.h"

#define	RESPAWN_DELAY		1000

#define STATE_FLAG_DROPPED	0
#define STATE_FLAG_CARRIED	1
#define STATE_FLAG_IDLE		3
#define STATE_FLAG_CAPTURED	4

void CTF_FlagTouch(ServerEntity_t *ent,ServerEntity_t *other);

void CTF_FlagReset(ServerEntity_t *ent)
{
	Entity_SetOrigin(ent,ent->local.pos1);
	SetAngle(ent,ent->local.pos2);

	// Make sure our owner no longer thinks he has us.
	if(ent->local.eOwner)
	{
		ent->local.eOwner->local.flag = NULL;
		ent->local.eOwner = NULL;
	}

	ent->local.flags = STATE_FLAG_IDLE;

	ent->v.TouchFunction	= CTF_FlagTouch;
	// Just clear the effects out.
	ent->v.effects			= 0;
	ent->v.nextthink		= Server.time+0.5;
}

void CTF_FlagThink(ServerEntity_t *ent)
{
	switch(ent->local.flags)
	{
	case STATE_FLAG_IDLE:
//		MONSTER_Animate(ent,FALSE,0,9,0.1f,0);
		break;
	case STATE_FLAG_CARRIED:
		if(ent->local.eOwner->v.health <= 0)
		{
			ent->local.hit = RESPAWN_DELAY;
			ent->local.flags = STATE_FLAG_DROPPED;
//			MONSTER_Animate(ent,FALSE,0,9,0.1f,0);
			break;
		}

		Math_VectorCopy(ent->local.eOwner->v.origin,ent->v.origin);

		ent->v.angles[0] = ent->local.eOwner->v.angles[0]+20;
		ent->v.angles[1] = ent->local.eOwner->v.angles[1];

//		MONSTER_Animate(ent,FALSE,15,15,0.01f,15);
		break;
	case STATE_FLAG_CAPTURED:
		CTF_FlagReset(ent);
		break;
	case STATE_FLAG_DROPPED:
		if(ent->local.hit <= 0)
		{
			CTF_FlagReset(ent);
			break;
		}
		else if(ent->local.hit == RESPAWN_DELAY)
		{
			Sound(ent,CHAN_ITEM,"ctf/flagdropped.wav",255,ATTN_NORM);

			ent->local.eOwner->local.flag	= NULL;
			ent->local.eOwner				= NULL;

			ent->v.TouchFunction	= CTF_FlagTouch;
			ent->v.angles[0]		= ent->v.angles[0]+10;
			ent->v.effects |= EF_MOTION_ROTATE|EF_MOTION_FLOAT;
		}
#if 1
		else if(ent->local.hit == 70	||
				ent->local.hit == 40	||
				ent->local.hit == 10)
			Sound(ent,CHAN_ITEM,"ctf/flagdropped.wav",255,ATTN_NORM);	// "VWOOM"
#endif

		ent->local.hit--;

		Engine.Con_DPrintf("Flag respawn in... %i\n",ent->local.hit);

//		MONSTER_Animate(ent,FALSE,0,9,0.1f,0);
		break;
	default:
//		MONSTER_Animate(ent,FALSE,0,9,0.1f,0);
		Engine.Con_Warning("Unknown flag state (%i)!\n",ent->local.flags);
	}
}

void CTF_FlagTouch(ServerEntity_t *ent,ServerEntity_t *other)
{
	/*	Don't let us pick this up if we're either not a client
		or we are dead.											*/
	if(other->Monster.iType != MONSTER_PLAYER || other->v.health <= 0)
		return;
	else if(other->local.flag && other->local.flag->local.style != ent->local.style)
	{
		if(other->local.flag->local.flags == STATE_FLAG_CAPTURED)
			return;

		switch(other->local.pTeam)
		{
		case TEAM_RED:
			iRedScore++;
			Sound(ent,CHAN_ITEM,"ctf/redscore.wav",255,ATTN_NORM);
			// TODO: Update score HUD
			break;
		case TEAM_BLUE:
			iBlueScore++;
			Sound(ent,CHAN_ITEM,"ctf/bluescore.wav",255,ATTN_NORM);
			// TODO: Update score HUD
			break;
		default:
			other->v.iScore++;
		}

		// TODO: Check our scores...
		if(iRedScore > cvServerMaxScore.value)
		{
		}
		else if(iBlueScore > cvServerMaxScore.value)
		{
		}

		other->local.flag->local.flags = STATE_FLAG_CAPTURED;
	}
	else if(!other->local.flag && other->local.pTeam != ent->local.style)
	{
		other->local.flag					= ent;
		other->local.flag->local.flags = STATE_FLAG_CARRIED;
		other->local.flag->local.eOwner		= other;
		other->local.flag->v.TouchFunction	= NULL;

		// Get the flags style and set our effect...
		switch(other->local.flag->local.style)
		{
		case TEAM_RED:
			other->local.flag->v.effects = EF_GLOW_RED;
			break;
		case TEAM_BLUE:
			other->local.flag->v.effects = EF_GLOW_BLUE;
			break;
		default:
			other->local.flag->v.effects = EF_GLOW_WHITE;
			break;
		}

		Engine.CenterPrint(other,va("%s stole the %s!\n",other->v.netname,other->local.flag->v.netname));

		Sound(ent,CHAN_ITEM,"ctf/flagtaken.wav",255,ATTN_NORM);
		Sound(ent,CHAN_ITEM,other->local.flag->v.noise,255,ATTN_STATIC);		//	"RED FLAG TAKEN!"
	}
}

/*	Spawns the flag for CTF mode!
*/
void CTF_FlagSpawn(ServerEntity_t *eFlag)
{
	if(!cvServerGameMode.iValue == MODE_CAPTURETHEFLAG)
		return;

	eFlag->v.movetype	= MOVETYPE_NONE;
	eFlag->v.items		= ITEM_FLAG;

	eFlag->Physics.solid = SOLID_TRIGGER;

	eFlag->local.flags = STATE_FLAG_IDLE;

	switch(eFlag->local.style)
	{
	/*	Once we have support for setting
		our models skin we'll do that here
		instead ;)							*/
	// TODO: This is in now, so rewrite the below to use skins instead.
	case TEAM_RED:
		Server_PrecacheSound("ctf/redscore.wav");

		eFlag->v.model		= "models/ctf/flag_red.md2";
		eFlag->v.noise		= "ctf/redflagtaken.wav";
		eFlag->v.netname	= "red flag";
		break;
	case TEAM_BLUE:
		Server_PrecacheSound("ctf/bluescore.wav");

		eFlag->v.model		= "models/ctf/flag_blue.md2";
		eFlag->v.noise		= "ctf/blueflagtaken.wav";
		eFlag->v.netname	= "blue flag";
		break;
	default:
		eFlag->v.model		= "models/ctf/flag_neutral.md2";
		eFlag->v.noise		= "ctf/neutralflagtaken.wav";
		eFlag->v.netname	= "neutral flag";
		break;
	}

	Server_PrecacheModel(eFlag->v.model);
	Server_PrecacheSound(eFlag->v.noise);
	Server_PrecacheSound("ctf/flagtaken.wav");

	Entity_SetModel(eFlag,eFlag->v.model);
	Entity_SetSize(eFlag,-10,-10,0,10,10,65);
	Entity_SetOrigin(eFlag,eFlag->v.origin);

	// Save our original position.
	Math_VectorCopy(eFlag->v.angles,eFlag->local.pos1);
	Math_VectorCopy(eFlag->v.angles,eFlag->local.pos2);

	eFlag->v.takedamage	= false;
	eFlag->v.TouchFunction	= CTF_FlagTouch;
	eFlag->v.think			= CTF_FlagThink;
	eFlag->v.nextthink		= Server.time+0.01;
}
