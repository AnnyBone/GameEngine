/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
	Copyright (C) 2011-2015 OldTimes Software

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

#include "server_main.h"

#include "server_monster.h"
#include "server_weapon.h"
#include "server_player.h"

/*
	Katana Bot
	Used as companions in single-player
	and as opponents in multi-player.
	Server admins can modify any of this
	to their desire.

	TODO:
		Parse a file for phrases and names?

		Create a WAYPOINT_SPAWN at the point
		we spawn if there isn't one already.
		We'll check for this whenever we're
		moving later on to make sure we don't
		risk getting "spawn gibbed" by someone.
	~hogsy
*/

// These need to match player specs!
#define	BOT_MAX_HEALTH		cvServerMaxHealth.iValue
#define	BOT_MAX_SIGHT		900.0f
#define	BOT_MAX_SPEED		320.0f	// Must match player speed.
#define	BOT_DEFAULT_HEALTH	cvServerDefaultHealth.iValue
#define	BOT_MIN_SPEED		200.0f	// Must match player walk speed.
#define BOT_MIN_HEALTH		-20

// List of death phrases.
char *BotDeathPhrases[] =
{
	"You got lucky, punk.\n",
	"%s is a dirty hacker!\n",
	"Looks like %s is cheating!\n",
	"Lucky shot, %s.\n",
	"I'll get you for this!\n",
	"%s should be banned.\n",
	"That was unfair, %s!\n",
	"%s has an unfair advantage.\n",
	"Please ban %s!\n",
	"I hit you like 5 times, %s!\n"
};

// List of kill phrases.
char *BotKillPhrases[] =
{
	"Suck it down!\n",
	"Messed %s up, hah!\n",
	"I just made %s my bitch.\n",
	"Looks like it's not your day, %s.\n",
	"Baw, %s is dead... Oh well.\n",
	"Owned.\n",
	"%s licked my shoe!\n",
	"%s just lost his piece.\n",
	"Learn to play, %s!\n",
	"You all suck, especially %s.\n"
};

// Emotion based phrases.
char *BotBoredPhrases[] =
{
	"Ugh this is boring...\n",
	"Are we playing yet?\n"
};

// Emotion based phrases.
char *BotAngryPhrases[] =
{
	"Argh!!\n",
	"I'm going to kill you!!!\n"
};

// Emotion based phrases.
char *BotFearPhrases[] =
{
	"Hold up! Wait!! Go easy on me, %s\n",
	"You're too strong! I can't do this...\n"
};

// Emotion based phrases.
char *BotSurprisePhrases[] =
{
	"Woah! Snuck up on me there, %s ;)\n",
	"Haha you almost scared me for a moment there.\n"
};

// Emotion based phrases.
char *BotJoyPhrases[] =
{
	"This is pretty fun!\n",
	"Nothing can stop me :)\n"
};

// List of bot names.
const char *BotNames[] =
{
	"[BOT] Mercury",
	"[BOT] Atlas",
	"[BOT] Apollo",
	"[BOT] Hector",
	"[BOT] Prometheus",
	"[BOT] Athena",
	"[BOT] Styx",
	"[BOT] Erebos",
	"[BOT] Hecate",
	"[BOT] Oceanus",
	"[BOT] Chaos",
	"[BOT] Odysseus",
	"[BOT] Theseus",
	"[BOT] Castor"
};

void Bot_Run(ServerEntity_t *ent);
void Bot_Pain(ServerEntity_t *ent, ServerEntity_t *other);
void Bot_Die(ServerEntity_t *ent, ServerEntity_t *other);
void Bot_Stand(ServerEntity_t *eBot);
void Bot_Walk(ServerEntity_t *eBot);
void Bot_Think(ServerEntity_t *eBot);

/*	style
		0	Mikiko
		1	Superfly
*/
void Bot_Spawn(ServerEntity_t *eBot)
{
	int		iSpawnType;
	ServerEntity_t	*eSpawnPoint;

	// Don't spawn bots unless it's allowed by admin.
	if(!cvServerBots.value)
		return;

	Math_VectorClear(eBot->v.velocity);

	switch(eBot->local.style)
	{
	case BOT_DEFAULT:
		iSpawnType = INFO_PLAYER_DEATHMATCH;

		eBot->v.model = cvServerPlayerModel.string;
		strncpy(eBot->v.netname, BotNames[(rand() % pARRAYELEMENTS(BotNames))], 64);

		eBot->Monster.iType	= MONSTER_PLAYER;
		break;
#ifdef OPENKATANA
	case BOT_MIKIKO:
		iSpawnType = INFO_PLAYER_MIKIKO;

		Server_PrecacheModel("models/mikiko.md2");

		eBot->v.model	= "models/mikiko.md2";
		eBot->v.netname	= "Mikiko Ebihara";

		eBot->Monster.iType	= MONSTER_MIKIKO;
		break;
	case BOT_SUPERFLY:
		iSpawnType = INFO_PLAYER_SUPERFLY;

		Server_PrecacheModel("models/sprfly.md2");
		Server_PrecacheSound("player/superfly/superflydeath1.wav");
		Server_PrecacheSound("player/superfly/superflydeath2.wav");
		Server_PrecacheSound("player/superfly/superflydeath3.wav");
		Server_PrecacheSound("player/superfly/superflydeath4.wav");

		eBot->v.model = "models/sprfly.md2";
		eBot->v.netname	= "Superfly Johnson";

		eBot->Monster.iType	= MONSTER_SUPERFLY;
		break;
#endif
	default:
		Engine.Con_Warning("Attempted to spawn unknown bot type! (%i) (%i %i %i)\n",
			eBot->local.style,
			(int)eBot->v.origin[0],
			(int)eBot->v.origin[1],
			(int)eBot->v.origin[2]);

		Entity_Remove(eBot);
		return;
	}

	eBot->v.cClassname	= "bot";
	eBot->v.iHealth		= 100;
	eBot->local.iMaxHealth = cvServerMaxHealth.iValue;
	eBot->v.movetype	= MOVETYPE_STEP;
	eBot->v.bTakeDamage	= true;

	Entity_SetPhysics(eBot, SOLID_SLIDEBOX, 1.4f, 4.0f);

	eBot->local.bBleed	= true;

	eBot->Monster.Think = Bot_Think;
	eBot->Monster.PainFunction = Bot_Pain;

	// Mikiko and Superfly are set manually to avoid issues...
	if(eBot->local.style == BOT_DEFAULT)
	{
		// Must be set after teams are set up.
		eSpawnPoint = Entity_SpawnPoint(eBot,iSpawnType);
		if(!eSpawnPoint)
		{
			Engine.Con_Warning("%s failed to find spawnpoint!\n",eBot->v.netname);

			Entity_Remove(eBot);
			return;
		}

		Entity_SetOrigin(eBot,eSpawnPoint->v.origin);
		SetAngle(eBot,eSpawnPoint->v.angles);
	}

	Entity_SetKilledFunction(eBot, Bot_Die);

	eBot->Monster.PainFunction = Bot_Pain;
	eBot->Monster.Think	= Bot_Think;

	Entity_SetModel(eBot,eBot->v.model);
	Entity_SetSize(eBot,-16.0f,-16.0f,-24.0f,16.0f,16.0f,32.0f);

	Monster_SetState(eBot,STATE_AWAKE);
	Monster_SetThink(eBot,THINK_IDLE);

	// Make sure we're not in the air.
	Entity_DropToFloor(eBot);
}

void Bot_Think(ServerEntity_t *eBot)
{
	Weapon_t *wActiveWeapon;

	// If the bot isn't dead, then add animations.
	if(eBot->Monster.iState != STATE_DEAD)
	{
		if(eBot->v.flags & FL_ONGROUND)
		{
			if((	(eBot->v.velocity[0] < -4.0f || eBot->v.velocity[0] > 4.0f)	|| 
					(eBot->v.velocity[1] < -4.0f || eBot->v.velocity[1] > 4.0f))	&& 
					(!eBot->local.dAnimationTime || eBot->local.iAnimationEnd == 9))
				Entity_Animate(eBot,PlayerAnimation_Walk);
			else if((eBot->v.velocity[0] == 0 || eBot->v.velocity[1] == 0) && (!eBot->local.dAnimationTime || eBot->local.iAnimationEnd > 9))
			{
#ifdef GAME_OPENKATANA
				if(eBot->v.iActiveWeapon == WEAPON_DAIKATANA)
					Entity_Animate(eBot,PlayerAnimation_KatanaIdle);
				else
#endif
					Entity_Animate(eBot,PlayerAnimation_Idle);
			}
		}
	}

	switch(eBot->Monster.iThink)
	{
	case THINK_IDLE:
	{
		Monster_MoveRandom(eBot, 50.0f);

		// Attempt to find an enemy.
		if (!eBot->Monster.eEnemy)
		{
			ServerEntity_t *Enemy = Monster_GetEnemy(eBot);
			if (Enemy)
			{
				eBot->Monster.eEnemy = Enemy;

				Monster_SetThink(eBot, THINK_PURSUING);
				return;
			}
		}

		Waypoint_t *wMoveTarget = Monster_GetMoveTarget(eBot);
		if (wMoveTarget)
		{
			Math_VectorCopy(wMoveTarget->position, eBot->Monster.mvMoveTarget);

			Monster_SetThink(eBot, THINK_WANDERING);
			return;
		}
	}
	break;
	case THINK_ATTACKING:
	{
		if (eBot->Monster.eTarget->v.iHealth <= 0)
			Monster_SetThink(eBot, THINK_WANDERING);

		wActiveWeapon = Weapon_GetCurrentWeapon(eBot);
		if (!wActiveWeapon)
		{
			Monster_SetThink(eBot, THINK_FLEEING);
			return;
		}

#if 1
		// Add some random movement.
		Monster_MoveRandom(eBot, BOT_MIN_SPEED);

		if (rand()%500 == 0)
			wActiveWeapon->Primary(eBot);
#endif
	}
	break;
	case THINK_FLEEING:
		// Add some random movement.
		Monster_MoveRandom(eBot, BOT_MAX_SPEED);
	break;
	case THINK_WANDERING:
		{
#if 0
			wMyWeapon = Weapon_GetCurrentWeapon(eBot);
			if(MONSTER_GetRange(eBot,eBot->v.enemy->v.origin) > 4000)
				return;
			else if(wMyWeapon->iPrimaryType == AM_MELEE && MONSTER_GetRange(eBot,eBot->v.enemy->v.origin) > MONSTER_RANGE_MELEE)
				return;
			else if(Monster_IsVisible(eBot,eBot->v.enemy))
			{
				// [5/8/2012] No ammo and it's not a melee weapon? ~hogsy
				if(!Weapon_CheckPrimaryAmmo(eBot) && wMyWeapon->iPrimaryType != AM_MELEE)
				{
					// [5/8/2012] Should probably flee ~hogsy
					Monster_SetThink(eBot,THINK_FLEEING);
					return;
				}

				Math_VectorSubtract(eBot->v.enemy->v.origin,eBot->v.origin,vAngle);

		//		ent->v.ideal_yaw	= VectorToAngles(vAngle);

				ChangeYaw(eBot);
			}
#endif
		}
		break;
	}
}

void Bot_BroadcastMessage(ServerEntity_t *eBot, ServerEntity_t *other)
{
	char *cPhrase;

	if(!bIsMultiplayer || rand()%5 == 5)
		return;

	if(eBot->v.iHealth <= 0)
		cPhrase = BotDeathPhrases[(rand()%pARRAYELEMENTS(BotDeathPhrases))];
	else
	{
#if 0
		if(eBot->Monster.meEmotion[EMOTION_ANGER].iEmotion > 50)
			cPhrase = BotAngryPhrases[(rand()%pARRAYELEMENTS(BotAngryPhrases))];
		else if (eBot->Monster.meEmotion[EMOTION_BOREDOM].iEmotion > 50)
			cPhrase = BotBoredPhrases[(rand()%pARRAYELEMENTS(BotBoredPhrases))];
		else if (eBot->Monster.meEmotion[EMOTION_FEAR].iEmotion > 50)
			cPhrase = BotFearPhrases[(rand()%pARRAYELEMENTS(BotFearPhrases))];
		else if (eBot->Monster.meEmotion[EMOTION_JOY].iEmotion > 50)
			cPhrase = BotJoyPhrases[(rand()%pARRAYELEMENTS(BotJoyPhrases))];
		else
#endif
			// Emotions don't give us anything worth saying...
			return;
	}

	// TODO: Temporary until we can simulate chat!
	Engine.Server_BroadcastPrint
	(
		"%s: %s\n",
		eBot->v.netname,
		cPhrase,
		other->v.netname
	);
}

void Bot_Pain(ServerEntity_t *ent, ServerEntity_t *other)
{
	char sound[MAX_QPATH];
	Weapon_t *wMyWeapon, *wHisWeapon;

	// [4/10/2012] Let the player know how we're feeling :) ~hogsy
	Bot_BroadcastMessage(ent,other);

	// [14/9/2012] Get both mine and our enemies weapon ~hogsy
	wMyWeapon	= Weapon_GetCurrentWeapon(ent);
	wHisWeapon	= Weapon_GetCurrentWeapon(other);
	if(!wMyWeapon || (!Weapon_CheckPrimaryAmmo(wMyWeapon,ent) && wMyWeapon->iPrimaryType != AM_MELEE))
	{
		Monster_SetThink(ent,THINK_FLEEING);
		return;
	}
	// [14/9/2012] Otherwise check what we can see our enemy having (don't check ammo since it's unrealistic) ~hogsy
	else if(!wHisWeapon || wHisWeapon->iPrimaryType == AM_MELEE)
	{
		// [14/9/2012] We see you!! ~hogsy
		if(Monster_IsVisible(ent,other))
		{
			Monster_SetThink(ent,THINK_ATTACKING);
			return;
		}
	}

#if 0
	// [16/7/2012] Moved to avoid a situation where we try fleeing without a target ~hogsy
	if(	(ent->v.enemy && other->v.iHealth > ent->v.enemy->v.iHealth) ||
		!ent->v.enemy)
		// [15/7/2012] Reckless but change our target to this new enemy ~hogsy
		ent->v.enemy = other;

	mywep	= Weapon_GetCurrentWeapon(ent);
	wHisWeapon	= Weapon_GetCurrentWeapon(other);

	// [15/7/2012] Check that I have a weapon and ammo ~hogsy
	if(!mywep || ent->v.currentammo <= 0)
	{
		// [15/7/2012] I don't! Find the closest waypoint to move back to a little ~hogsy
		point = Waypoint_GetByVisibility(ent->v.origin);
POINTCHECK:
		if(!point)
			return;
		if(	point->type	== WAYPOINT_WEAPON	||
			point->type == WAYPOINT_COVER	||
			point->type == WAYPOINT_DEFAULT)
		{
			Monster_SetThink(ent,MONSTER);

			// [15/7/2012] Set the position we'll move to next ~hogsy
			Math_VectorCopy(ent->Monster.target,point->position);
		}
		else
		{
			// [15/7/2012] Keep on checking until we get something ~hogsy
			if(pointchecks >= 5)
			{
				// [15/7/2012] Didn't get anything... ~hogsy
				return;
			}
			pointchecks++;

			point = Waypoint_GetByVisibility(point->position);
			goto POINTCHECK;
		}

		return;
	}
	// [15/7/2012] Okay, he doesn't have a weapon ~hogsy
	else if(!wHisWeapon)
		// [15/7/2012] Since it's probably not even a player ~hogsy
		return;
#endif

#ifdef OPENKATANA
	// [26/7/2012] Character-based sounds ~hogsy
	// [29/7/2012] Revised ~hogsy
	switch(ent->local.style)
	{
	case BOT_MIKIKO:
		if(ent->v.watertype == BSP_CONTENTS_WATER && ent->v.waterlevel == 3)
		{
		}
		else
			sprintf(sound,"player/mikiko/mikikodeath%i.wav",rand()%5+1);
		break;
	case BOT_SUPERFLY:
		if(ent->v.watertype == BSP_CONTENTS_WATER && ent->v.waterlevel == 3)
		{
		}
		else
			sprintf(sound,"player/superfly/superflydeath%i.wav",rand()%4+1);
		break;
	default:
#endif
		if(ent->v.watertype == BSP_CONTENTS_WATER && ent->v.waterlevel == 3)
		{
			if(rand()%2 == 1)
				sprintf(sound,"player/playerchoke1.wav");
			else
				sprintf(sound,"player/playerchoke3.wav");
		}
		else
			sprintf(sound,"player/playerpain%i.wav",rand()%4+1);
#ifdef OPENKATANA
	}
#endif

	Sound(ent,CHAN_VOICE,sound,255,ATTN_NORM);
}

void Bot_Walk(ServerEntity_t *eBot)
{
	eBot->v.velocity[0] -= 5.0f;
	eBot->v.velocity[1] -= 10.0f;
}

void Bot_Die(ServerEntity_t *eBot,ServerEntity_t *eOther)
{
	char sound[MAX_QPATH];

	if(eBot->Monster.iState == STATE_DEAD)
		return;

	// He's dead, Jim.
	Monster_SetState(eBot, STATE_DEAD);

	eBot->v.movetype	= MOVETYPE_TOSS;
	eBot->v.flags		-= (eBot->v.flags & FL_ONGROUND);
	eBot->v.angles[0]	= eBot->v.angles[2] = 0;

	eBot->Physics.iSolid	= SOLID_NOT;

	if(rand()%2)
		Entity_Animate(eBot,PlayerAnimation_Death1);
	else
		Entity_Animate(eBot,PlayerAnimation_Death2);

	// Let the player know how we're feeling.
	Bot_BroadcastMessage(eBot,eOther);

#ifdef OPENKATANA
	// Character-based sounds.
	switch(eBot->local.style)
	{
	case BOT_MIKIKO:
		sprintf(sound,"player/mikiko/mikikodeath%i.wav",rand()%5+1);
		break;
	case BOT_SUPERFLY:
		sprintf(sound,"player/superfly/superflydeath%i.wav",rand()%4+1);
		break;
	default:
#endif
		sprintf(sound,"player/playerdeath%i.wav",rand()%4+1);
#ifdef OPENKATANA
	}
#endif

	Sound(eBot,CHAN_VOICE,sound,255,ATTN_NORM);

	if(eBot->v.iHealth < BOT_MIN_HEALTH)
	{
		Sound(eBot, CHAN_VOICE, "misc/gib1.wav", 255, ATTN_NORM);

		ThrowGib(eBot->v.origin,eBot->v.velocity,PHYSICS_MODEL_GIB0,(float)(eBot->v.iHealth*-1),true);
		ThrowGib(eBot->v.origin,eBot->v.velocity,PHYSICS_MODEL_GIB1,(float)(eBot->v.iHealth*-1),true);
		ThrowGib(eBot->v.origin,eBot->v.velocity,PHYSICS_MODEL_GIB2,(float)(eBot->v.iHealth*-1),true);
	}
}
