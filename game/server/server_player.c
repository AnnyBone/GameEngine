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

#include "server_main.h"

#include "server_player.h"
#include "server_vehicle.h"
#include "server_weapon.h"
#include "server_item.h"
#include "server_menu.h"
#include "server_effects.h"

/*
	Code specific towards the player. This includes code for
	spawning, jumping, animating and everything else except
	for physics.
*/

#define	PLAYER_MAX_HEALTH	cvServerMaxHealth.iValue
#define	PLAYER_MIN_HEALTH	-20

EntityFrame_t PlayerAnimation_Idle[] =
{
	{	NULL,	0,	0.1f			},
	{   NULL,	1,	0.1f			},
	{   NULL,	2,	0.1f			},
	{   NULL,	3,	0.1f			},
	{   NULL,	4,	0.1f			},
	{   NULL,	5,	0.1f			},
	{   NULL,	6,	0.1f			},
	{   NULL,	7,	0.1f			},
	{   NULL,	8,	0.1f			},
	{   NULL,	9,	0.1f,	true    }
};

EntityFrame_t PlayerAnimation_Fire[] =
{
	{   NULL,	10, 0.02f			},
	{   NULL,	11, 0.02f			},
	{   NULL,	12, 0.02f			},
	{   NULL,	13, 0.02f			},
	{   NULL,	14, 0.02f			},
	{   NULL,	15, 0.02f			},
	{   NULL,	16, 0.02f			},
	{   NULL,	17, 0.02f			},
	{   NULL,	18,	0.02f			},
	{	NULL,	19,	0.02f,	true	}
};

EntityFrame_t PlayerAnimation_Walk[] =
{
	{	NULL,	20,	0.02f			},
	{   NULL,	21, 0.02f			},
	{   NULL,	22, 0.02f			},
	{   NULL,	23, 0.02f			},
	{   NULL,	24, 0.02f			},
	{   NULL,	25, 0.02f			},
	{   NULL,	26, 0.02f			},
	{   NULL,	27, 0.02f			},
	{   NULL,	28, 0.02f			},
	{   NULL,	29, 0.02f			},
	{   NULL,	30, 0.02f			},
	{   NULL,	31, 0.02f			},
	{   NULL,	32, 0.02f			},
	{   NULL,	33, 0.02f			},
	{   NULL,	34, 0.02f			},
	{   NULL,	35, 0.02f			},
	{   NULL,	36, 0.02f			},
	{   NULL,	37, 0.02f			},
	{   NULL,	38, 0.02f			},
	{   NULL,	39, 0.02f			},
	{   NULL,	40, 0.02f			},
	{   NULL,	41, 0.02f			},
	{   NULL,	42, 0.02f			},
	{   NULL,	43, 0.02f			},
	{   NULL,	44, 0.02f			},
	{   NULL,	45, 0.02f			},
	{   NULL,	46, 0.02f,	true	}
};

EntityFrame_t PlayerAnimation_Death1[] =
{
	{   NULL, 56, 0.02f    },
	{   NULL, 57, 0.02f    },
	{   NULL, 58, 0.02f    },
	{   NULL, 59, 0.02f    },
	{   NULL, 60, 0.02f    },
	{   NULL, 61, 0.02f    },
	{   NULL, 62, 0.02f    },
	{   NULL, 63, 0.02f    },
	{   NULL, 64, 0.02f    },
	{   NULL, 65, 0.02f    },
	{   NULL, 66, 0.02f    },
	{   NULL, 67, 0.02f    },
	{   NULL, 68, 0.02f    },
	{   NULL, 69, 0.02f    },
	{   NULL, 70, 0.02f    },
	{   NULL, 71, 0.02f    },
	{   NULL, 72, 0.02f    },
	{   NULL, 73, 0.02f    },
	{   NULL, 74, 0.02f    },
	{   NULL, 75, 0.02f    },
	{   NULL, 76, 0.02f    },
	{   NULL, 77, 0.02f    },
	{   NULL, 78, 0.02f, true    }
};

EntityFrame_t PlayerAnimation_Death2[] =
{
	{   NULL, 79, 0.02f    },
	{   NULL, 80, 0.02f    },
	{   NULL, 81, 0.02f    },
	{   NULL, 82, 0.02f    },
	{   NULL, 83, 0.02f    },
	{   NULL, 84, 0.02f    },
	{   NULL, 85, 0.02f    },
	{   NULL, 86, 0.02f    },
	{   NULL, 87, 0.02f    },
	{   NULL, 88, 0.02f    },
	{   NULL, 89, 0.02f    },
	{   NULL, 90, 0.02f    },
	{   NULL, 91, 0.02f    },
	{   NULL, 92, 0.02f    },
	{   NULL, 93, 0.02f    },
	{   NULL, 94, 0.02f    },
	{   NULL, 95, 0.02f    },
	{   NULL, 96, 0.02f    },
	{   NULL, 97, 0.02f    },
	{   NULL, 98, 0.02f    },
	{   NULL, 99, 0.02f, true    }
};

EntityFrame_t PlayerAnimation_Jump[] =
{
	{   NULL, 100, 0.02f    },
	{   NULL, 101, 0.02f    },
	{   NULL, 102, 0.02f    },
	{   NULL, 103, 0.02f    },
	{   NULL, 104, 0.02f    },
	{   NULL, 105, 0.02f    },
	{   NULL, 106, 0.02f    },
	{   NULL, 107, 0.02f    },
	{   NULL, 108, 0.02f    },
	{   NULL, 109, 0.02f    },
	{   NULL, 110, 0.02f    },
	{   NULL, 111, 0.02f    },
	{   NULL, 112, 0.02f    },
	{   NULL, 113, 0.02f    },
	{   NULL, 114, 0.02f    },
	{   NULL, 115, 0.02f    },
	{   NULL, 116, 0.02f    },
	{   NULL, 117, 0.02f    },
	{   NULL, 118, 0.02f    },
	{   NULL, 119, 0.02f    },
	{   NULL, 120, 0.02f    },
	{   NULL, 121, 0.02f    },
	{   NULL, 122, 0.02f    },
	{   NULL, 123, 0.02f    },
	{   NULL, 124, 0.02f    },
	{   NULL, 125, 0.02f    },
	{   NULL, 126, 0.02f    },
	{   NULL, 127, 0.02f    },
	{   NULL, 128, 0.02f    },
	{   NULL, 129, 0.02f, true    }
};

EntityFrame_t PlayerAnimation_RunJump[] =
{
	{   NULL, 130, 0.02f    },
	{   NULL, 131, 0.02f    },
	{   NULL, 132, 0.02f    },
	{   NULL, 133, 0.02f    },
	{   NULL, 134, 0.02f    },
	{   NULL, 135, 0.02f    },
	{   NULL, 136, 0.02f    },
	{   NULL, 137, 0.02f    },
	{   NULL, 138, 0.02f    },
	{   NULL, 139, 0.02f    },
	{   NULL, 140, 0.02f    },
	{   NULL, 141, 0.02f    },
	{   NULL, 142, 0.02f    },
	{   NULL, 143, 0.02f    },
	{   NULL, 144, 0.02f    },
	{   NULL, 145, 0.02f    },
	{   NULL, 146, 0.02f    },
	{   NULL, 147, 0.02f    },
	{   NULL, 148, 0.02f, true    }
};

EntityFrame_t PlayerAnimation_KatanaIdle[] =
{
	{   NULL, 150, 0.02f},
	{   NULL, 151, 0.02f},
	{   NULL, 152, 0.02f},
	{   NULL, 153, 0.02f},
	{   NULL, 154, 0.02f},
	{   NULL, 155, 0.02f},
	{   NULL, 156, 0.02f},
	{   NULL, 157, 0.02f},
	{   NULL, 158, 0.02f},
	{   NULL, 159, 0.02f,  true    }
};

EntityFrame_t PlayerAnimation_KatanaAttack1[] =
{
	{ NULL, 161, 0.02f },
	{ NULL, 162, 0.02f },
	{ NULL, 163, 0.02f },
	{ NULL, 164, 0.02f },
	{ NULL, 165, 0.02f },
	{ NULL, 166, 0.02f },
	{ NULL, 167, 0.02f },
	{ NULL, 168, 0.02f },
	{ NULL, 169, 0.02f },
	{ NULL, 170, 0.02f },
	{ NULL, 171, 0.02f },
	{ NULL, 172, 0.02f },
	{ NULL, 173, 0.02f },
	{ NULL, 174, 0.02f },
	{ NULL, 175, 0.02f },
	{ NULL, 176, 0.02f },
	{ NULL, 177, 0.02f },
	{ NULL, 178, 0.02f },
	{ NULL, 179, 0.02f },
	{ NULL, 180, 0.02f },
	{ NULL, 181, 0.02f },
	{ NULL, 182, 0.02f },
	{ NULL, 183, 0.02f },
	{ NULL, 184, 0.02f },
	{ NULL, 185, 0.02f },
	{ NULL, 186, 0.02f },
	{ NULL, 187, 0.02f },
	{ NULL, 188, 0.02f },
	{ NULL, 189, 0.02f, true }
};

EntityFrame_t PlayerAnimation_KatanaAttack2[] =
{
	{ NULL, 191, 0.02f },
	{ NULL, 192, 0.02f },
	{ NULL, 193, 0.02f },
	{ NULL, 194, 0.02f },
	{ NULL, 195, 0.02f },
	{ NULL, 196, 0.02f },
	{ NULL, 197, 0.02f },
	{ NULL, 198, 0.02f },
	{ NULL, 199, 0.02f },
	{ NULL, 200, 0.02f },
	{ NULL, 201, 0.02f },
	{ NULL, 202, 0.02f },
	{ NULL, 203, 0.02f },
	{ NULL, 204, 0.02f },
	{ NULL, 205, 0.02f },
	{ NULL, 206, 0.02f },
	{ NULL, 207, 0.02f },
	{ NULL, 208, 0.02f },
	{ NULL, 209, 0.02f },
	{ NULL, 210, 0.02f },
	{ NULL, 211, 0.02f },
	{ NULL, 212, 0.02f },
	{ NULL, 213, 0.02f },
	{ NULL, 214, 0.02f },
	{ NULL, 215, 0.02f },
	{ NULL, 216, 0.02f, true }
};

EntityFrame_t PlayerAnimation_KatanaDeath1[] =
{
	{ NULL, 218, 0.02f },
	{ NULL, 219, 0.02f },
	{ NULL, 220, 0.02f },
	{ NULL, 221, 0.02f },
	{ NULL, 222, 0.02f },
	{ NULL, 223, 0.02f },
	{ NULL, 224, 0.02f },
	{ NULL, 225, 0.02f },
	{ NULL, 226, 0.02f },
	{ NULL, 227, 0.02f },
	{ NULL, 228, 0.02f },
	{ NULL, 229, 0.02f },
	{ NULL, 230, 0.02f },
	{ NULL, 231, 0.02f },
	{ NULL, 232, 0.02f },
	{ NULL, 233, 0.02f },
	{ NULL, 234, 0.02f },
	{ NULL, 235, 0.02f },
	{ NULL, 236, 0.02f },
	{ NULL, 237, 0.02f },
	{ NULL, 238, 0.02f },
	{ NULL, 239, 0.02f },
	{ NULL, 240, 0.02f },
	{ NULL, 241, 0.02f },
	{ NULL, 242, 0.02f },
	{ NULL, 243, 0.02f },
	{ NULL, 244, 0.02f },
	{ NULL, 245, 0.02f },
	{ NULL, 246, 0.02f },
	{ NULL, 247, 0.02f },
	{ NULL, 248, 0.02f },
	{ NULL, 249, 0.02f, true }
};

void Player_CheckFootsteps(ServerEntity_t *player)
{
	float			fForce;
	double			dDelay;
	MathVector2f_t	vStep;

	// Also check movetype so we don't do steps while noclipping/flying.
	if ((player->v.movetype == MOVETYPE_WALK) && player->v.flags & FL_ONGROUND)
	{
		if (player->local.dStepTime > Server.dTime)
			return;
		else if (
			// Ensure there's enough movement that calls for us to produce a footstep.
			(player->v.velocity[0] < 5) && (player->v.velocity[0] > -5) &&
			(player->v.velocity[1] < 5) && (player->v.velocity[1] > -5))
			return;

		vStep[0] = player->v.velocity[0];
		if(vStep[0] < 0)
			vStep[0] *= -1.0f;

		vStep[1] = player->v.velocity[1];
		if(vStep[1] < 0)
			vStep[1] *= -1.0f;

		fForce = vStep[0]+vStep[1];

		// Base this on our velocity.
		dDelay = Math_Clamp(0.1, (double)(1.0f / (fForce / 100.0f)), 1.0);

		// TODO: Check if we're in water or not and change this accordingly :)
		Sound(player, CHAN_BODY, va("physics/concrete%i_footstep.wav", rand() % 4), 150, ATTN_NORM);

		player->local.dStepTime = Server.dTime + dDelay;
	}
}

void Player_CheckWater(ServerEntity_t *ePlayer)
{
	// [2/8/2014] Basic Drowning... ~eukos
	if(ePlayer->v.waterlevel != 3)
		ePlayer->local.dAirFinished = Server.dTime + 12;
	else if(ePlayer->local.dAirFinished < Server.dTime)
	{
		if(ePlayer->local.dPainFinished < Server.dTime)
		{
			Entity_Damage(ePlayer, ePlayer, 10, 0);

			ePlayer->local.dPainFinished = Server.dTime + 1;
		}
	}
}

void Player_PostThink(ServerEntity_t *ePlayer)
{
	// If round has not started then don't go through this!
	if ((ePlayer->Monster.state == MONSTER_STATE_DEAD) || !Server.round_started)
		return;
	// Check if we're in a vehicle.
	else if(ePlayer->local.eVehicle)
	{
		switch(ePlayer->local.eVehicle->Vehicle.iSlot[ePlayer->local.iVehicleSlot])
		{
		case SLOT_DRIVER:
			Math_VectorCopy(ePlayer->local.eVehicle->v.origin,ePlayer->v.origin);
			break;
		case SLOT_PASSENGER:
			break;
		default:
			Engine.Con_Warning("Player (%s) is occupying an unknown vehicle slot (%i)!\n",
				ePlayer->v.netname,ePlayer->local.iVehicleSlot);

			Vehicle_Exit(ePlayer->local.eVehicle,ePlayer);
		}
		return;
	}

	Weapon_CheckInput(ePlayer);

	if ((ePlayer->local.jump_velocity < -300.0f) &&
		(ePlayer->v.flags & FL_ONGROUND)			&&
		(ePlayer->v.iHealth > 0))
	{
		char snd[32];

		if(ePlayer->v.watertype == BSP_CONTENTS_WATER)
			sprintf(snd,"player/h2ojump.wav");
		else if (ePlayer->local.jump_velocity < -650.0f)
		{
			// TODO: Figure out if we were pushed by an enemy.
			// TODO: Base damage on velocity.
			Entity_Damage(ePlayer, ePlayer, 10, 0);

			if(rand()%2 == 1)
				sprintf(snd,"player/playerlandhurt.wav");
			else
				sprintf(snd,"player/playerlandhurt2.wav");

			ePlayer->local.deathtype = "falling";

			ePlayer->v.punchangle[0] += (float)(rand()%5+2)*7.0f;	// [13/4/2013] Give him a big punch... ~eukos
		}
		else
		{
			// Land sounds DO NOT use CHAN_VOICE otherwise they get horribly cut out!
			strncpy(snd, "player/land0.wav", sizeof(snd));

			// Give him a little punch...
			// TODO: Switch these over purely to the client.
			ePlayer->v.punchangle[0] -= ePlayer->local.jump_velocity / 100.0f;
		}

		Sound(ePlayer, CHAN_VOICE, snd, 255, ATTN_NORM);

		ePlayer->local.jump_velocity = 0;
	}

	if(!(ePlayer->v.flags & FL_ONGROUND))
		ePlayer->local.jump_velocity = ePlayer->v.velocity[2];
	else if((	(ePlayer->v.velocity[0] < -4.0f || ePlayer->v.velocity[0] > 4.0f)	||
				(ePlayer->v.velocity[1] < -4.0f || ePlayer->v.velocity[1] > 4.0f))	&&
				(!ePlayer->local.dAnimationTime || ePlayer->local.iAnimationEnd == 9))
		Entity_Animate(ePlayer,PlayerAnimation_Walk);
	else if((ePlayer->v.velocity[0] == 0 || ePlayer->v.velocity[1] == 0) && (!ePlayer->local.dAnimationTime || ePlayer->local.iAnimationEnd == 46))
	{
#ifdef GAME_OPENKATANA
		if(ePlayer->v.iActiveWeapon == WEAPON_DAIKATANA)
			Entity_Animate(ePlayer,PlayerAnimation_KatanaIdle);
		else
#endif
			Entity_Animate(ePlayer,PlayerAnimation_Idle);
	}

	Player_CheckFootsteps(ePlayer);
#ifdef GAME_OPENKATANA
	Player_CheckPowerups(ePlayer);
#endif
}

void Player_PreThink(ServerEntity_t *ePlayer)
{
	if (!Server.round_started)
		return;

	if (Server.round_started && !Server.players_spawned)
		// Spawn the player!
		Player_Spawn(ePlayer);

	Weapon_CheckFrames(ePlayer);
	Entity_CheckFrames(ePlayer);
	Player_CheckWater(ePlayer);

	if (ePlayer->Monster.state == MONSTER_STATE_DEAD)
	{
		Player_DeathThink(ePlayer);
		return;
	}

	// ladders only work in ZeroG ~eukara
	if (ePlayer->local.dZeroGTime < Server.dTime)
		ePlayer->Physics.fGravity = cvServerGravity.value;
	else
		ePlayer->Physics.fGravity = 0;

	if(ePlayer->v.waterlevel == 2)
	{
		int				i;
		trace_t			tTrace;
		MathVector3f_t	vStart,vEnd;

		// Check if we can jump onto an edge, was originally in a seperate function but merged here instead.
		plAngleVectors(ePlayer->v.angles, ePlayer->local.vForward, ePlayer->local.vRight, ePlayer->local.vUp);
		Math_VectorCopy(ePlayer->v.origin,vStart);

		vStart[pZ] += 8.0f;

		ePlayer->local.vForward[pZ] = 0;

		plVectorNormalize(ePlayer->local.vForward);

		for(i = 0; i < 3; i++)
			vEnd[i] = vStart[i] + ePlayer->local.vForward[i] * 24.0f;

		tTrace = Engine.Server_Move(vStart, vEnd, pl_origin3f, pl_origin3f, true, ePlayer);
		if(tTrace.fraction < 1.0f)
		{
			vStart[pZ] += ePlayer->v.maxs[pZ]-8.0f;

			for(i = 0; i < 3; i++)
				vEnd[i] = vStart[i] + ePlayer->local.vForward[i] * 24.0f;

			Math_VectorSubtractValue(tTrace.plane.normal,50.0f,ePlayer->v.movedir);

			tTrace = Engine.Server_Move(vStart, vEnd, pl_origin3f, pl_origin3f, true, ePlayer);
			if(tTrace.fraction == 1.0f)
			{
				ePlayer->v.flags		|= FL_WATERJUMP;
				ePlayer->v.flags		-= (ePlayer->v.flags & FL_JUMPRELEASED);
				ePlayer->v.velocity[pZ]	= 225.0f;
			}
		}
	}

	if(ePlayer->v.button[2])
		Player_Jump(ePlayer);
	else
		ePlayer->v.flags |= FL_JUMPRELEASED;

	// Crouch
	if(ePlayer->v.button[1])
	{
		if (!(ePlayer->v.flags & FL_CROUCHING))
		{
			Entity_SetSize(ePlayer, -16.0f, -16.0f, -18.0f, 16.0f, 16.0f, 18.0f);

			ePlayer->v.view_ofs[2] = 10.0f;

			ePlayer->v.flags |= FL_CROUCHING;
		}
	}
	// Uncrouch
	else if (ePlayer->v.flags & FL_CROUCHING)
	{
		MathVector3f_t	vOrigin,
			vMaxs = { 16.0f, 16.0f, 36.0f },
			vMins = { -16.0f, -16.0f, -36.0f };
		trace_t	tStandCheck;

		Math_VectorCopy(ePlayer->v.origin, vOrigin);

		vOrigin[2] += 20.0f;

		// Trace to ensure we have enough room to stand.
		tStandCheck = Engine.Server_Move(vOrigin, vMins, vMaxs, vOrigin, 0, ePlayer);
		if (!tStandCheck.bAllSolid)
		{
			Entity_SetSizeVector(ePlayer, vMins, vMaxs);

			ePlayer->v.view_ofs[2] = 28.0f;

			// Not crouching anymore.
			ePlayer->v.flags -= (ePlayer->v.flags & FL_CROUCHING);
		}
	}

	if(cvServerWaypointSpawn.value && (Server.dTime >= Server.dWaypointSpawnDelay))
	{
		if(ePlayer->v.movetype != MOVETYPE_WALK)
			return;

		if(ePlayer->v.flags & FL_ONGROUND)
			// Create a waypoint at our current position.
			Waypoint_Spawn(ePlayer->v.origin, WAYPOINT_TYPE_DEFAULT);
		else if((ePlayer->v.flags & FL_SWIM) /*&& !(ePlayer->v.flags & FL_ONGROUND)*/)
			Waypoint_Spawn(ePlayer->v.origin, WAYPOINT_TYPE_SWIM);
		// Create waypoints in the air.
		else if(!(ePlayer->v.flags & FL_ONGROUND))
			Waypoint_Spawn(ePlayer->v.origin, WAYPOINT_TYPE_JUMP);

		Server.dWaypointSpawnDelay = Server.dTime+((double)cvServerWaypointDelay.value);
	}
}

void Player_Gib(ServerEntity_t *player)
{
	Sound(player, CHAN_VOICE, "misc/gib1.wav", 255, ATTN_NORM);

	// [13/9/2012] Updated paths ~hogsy
	ThrowGib(player->v.origin, player->v.velocity, PHYSICS_MODEL_GIB0, (float)player->v.iHealth*-1, true);
	ThrowGib(player->v.origin, player->v.velocity, PHYSICS_MODEL_GIB1, (float)player->v.iHealth*-1, true);
	ThrowGib(player->v.origin, player->v.velocity, PHYSICS_MODEL_GIB2, (float)player->v.iHealth*-1, true);
	ThrowGib(player->v.origin, player->v.velocity, PHYSICS_MODEL_GIB3, (float)player->v.iHealth*-1, true);

	ServerEffect_BloodCloud(player->v.origin, BLOOD_TYPE_RED);
	
	Entity_SetModel(player, "");
}

void Player_Die(ServerEntity_t *ePlayer, ServerEntity_t *other, ServerDamageType_t type)
{
	char s[32];

	/*	TODO:
		Slowly fade our screen to red (or black?)
		Camera should follow the entity that killed us?
	*/

	Math_VectorClear(ePlayer->v.view_ofs);

	ePlayer->v.modelindex	= iPlayerModelIndex;
	ePlayer->v.view_ofs[2]	= -8.0f;
	ePlayer->v.flags		-= (ePlayer->v.flags & FL_ONGROUND);
	ePlayer->v.movetype		= MOVETYPE_TOSS;
	ePlayer->v.angles[0]	= ePlayer->v.angles[2] = 0;

	ePlayer->Monster.state = MONSTER_STATE_DEAD;

	ePlayer->Physics.iSolid	= SOLID_NOT;

#ifdef GAME_OPENKATANA
	// [15/10/2013] Detonate all C4 bombs we've laid out! ~hogsy
	C4Vizatergo_SecondaryAttack(ePlayer);
#endif

#if 0
	int i;
	// [25/8/2012] Death cam ~hogsy
	// [25/8/2012] TODO: Monster and client checks ~hogsy
	if(other)
	{
		Engine.SetMessageEntity(ent);

		Engine.WriteByte(MSG_ONE,SVC_SETANGLE);
		for(i = 0; i < 2; i++)
			Engine.WriteAngle(MSG_ONE,other->v.origin[i]);
	}
#endif

	if(ePlayer->v.waterlevel == 3)
		sprintf(s,"player/playerwaterdeath.wav");
	else
		sprintf(s,"player/playerdeath%i.wav",rand()%4+1);

	Sound(ePlayer,CHAN_VOICE,s,255,ATTN_NONE);

#ifdef GAME_OPENKATANA
	if(ePlayer->v.iActiveWeapon == WEAPON_DAIKATANA)
		Entity_Animate(ePlayer,PlayerAnimation_KatanaDeath1);
	else
#endif
	{
		if(rand()%2 == 1)
			Entity_Animate(ePlayer,PlayerAnimation_Death1);
		else
			Entity_Animate(ePlayer,PlayerAnimation_Death2);
	}
}

void Player_Pain(ServerEntity_t *ent, ServerEntity_t *other, ServerDamageType_t type)
{
	char cSound[24];

	PLAYER_SOUND_PAIN(cSound);

	Sound(ent,CHAN_VOICE,cSound,255,ATTN_NORM);
}

int	iSpawnSlot;

/*	Find a random spawn point for the entity (point_start).
*/
ServerEntity_t *Player_GetSpawnEntity(ServerEntity_t *entity, int type)
{
	ServerEntity_t	*spawn;
	char			*startname;

	switch (type)
	{
	case INFO_PLAYER_DEATHMATCH:
		startname = "point_deathmatch";
		break;
	case INFO_PLAYER_CTF:
		if (entity->local.pTeam == TEAM_RED)
			startname = "point_start_red";
		else if (entity->local.pTeam == TEAM_BLUE)
			startname = "point_start_blue";
		else
		{
			Engine.Con_Warning("Unknown team type for spawn point! (%i)", entity->local.pTeam);
			return NULL;
		}
		break;
#ifdef GAME_OPENKATANA
	case INFO_PLAYER_SUPERFLY:
	case INFO_PLAYER_MIKIKO:
#endif
	default:
		startname = "point_start";
	}

	spawn = Engine.Server_FindEntity(Server.eWorld, startname, true);
	if (spawn)
		if (type == spawn->local.style)
			return spawn;

	return NULL;
}

void Player_Spawn(ServerEntity_t *ePlayer)
{
	ServerEntity_t *eSpawnPoint;

	ePlayer->Monster.iType = MONSTER_PLAYER;

	ePlayer->v.cClassname = "player";
	ePlayer->v.iHealth = cvServerDefaultHealth.iValue;
	ePlayer->v.movetype = MOVETYPE_WALK;
	ePlayer->v.bTakeDamage = true;
	ePlayer->v.model = cvServerPlayerModel.string;
	ePlayer->v.effects = 0;

	ePlayer->Physics.iSolid = SOLID_SLIDEBOX;
	ePlayer->Physics.fMass = 1.4f;
	ePlayer->Physics.fGravity = SERVER_GRAVITY;
	ePlayer->Physics.fFriction = 4.0f;

	ePlayer->local.iMaxHealth = PLAYER_MAX_HEALTH;				// Set the players default maximum health.
	ePlayer->local.fSpawnDelay = cvServerRespawnDelay.value;	// Set the delay before we spawn.
	ePlayer->local.pTeam = TEAM_NEUTRAL;						// Set the default team.
	ePlayer->local.bBleed = true;								// The player bleeds!

	// Clear the velocity and current view offset.
	Math_VectorClear(ePlayer->v.velocity);
	Math_VectorClear(ePlayer->v.view_ofs);

	ePlayer->v.bFixAngle = true;
	ePlayer->v.view_ofs[2] = 28.0f;

	// Set our think functions.
	Entity_SetDamagedFunction(ePlayer, Player_Pain);
	Entity_SetKilledFunction(ePlayer, Player_Die);

	// Clear out the players inventory.
	Item_ClearInventory(ePlayer);

	// Let the server know that a player has spawned.
	Server.players_spawned = true;

	if(bIsMultiplayer)
	{
#ifdef GAME_OPENKATANA
		switch(cvServerGameMode.iValue)
		{
		// TODO: Check what model this player has set in a cvar
		// TODO: Set texture function for team based skins?
		// [25/2/2012] Added mode specific model selection ~hogsy
		case MODE_CAPTURETHEFLAG:
			if(!ePlayer->local.pTeam)
			{
				if(!iSpawnSlot)
				{
					ePlayer->local.pTeam = TEAM_BLUE;
					Engine.Server_BroadcastPrint("%s has been assigned to the blue team!\n",ePlayer->v.netname);
					iSpawnSlot++;
				}
				else
				{
					ePlayer->local.pTeam = TEAM_RED;
					Engine.Server_BroadcastPrint("%s has been assigned to the red team!\n",ePlayer->v.netname);
					iSpawnSlot = 0;
				}

	#if 0
				{
					ent->local.pTeam = TEAM_SPECTATOR;
					Game.Server_BroadcastPrint("%s is spectating.\n",ePlayer->v.netname);
				}
	#endif
			}

			/*	Model names are temporary until we get
				player models sorted. Had a bit of an
				idea that team blue could be a random
				selection of Mikiko, Superfly and Hiro
				while team red could be random selection
				of special robots which Mishima has
				created just for CTF.
			*/
			switch(ePlayer->local.pTeam)
			{
			case TEAM_BLUE:
				ePlayer->v.model = "models/sprfly.mdl";
				break;
			case TEAM_RED:
				ePlayer->v.model = "models/mikiko.mdl";
				break;
			case TEAM_SPECTATOR:
				ePlayer->v.model = "";
				break;
			default:
				ePlayer->v.model = "models/player.md2";
				break;
			}
			break;
		case MODE_DEATHMATCH:
			Deathmatch_Spawn(ePlayer);
			break;
		}
#endif
	}
	else // SINGLEPLAYER
	{
		ServerMenu_UpdateClient(ePlayer, MENU_STATE_HUD, true);

#ifdef GAME_OPENKATANA
		{
			Item_t	*iDaikatana = Item_GetItem(WEAPON_DAIKATANA);

			if(iDaikatana)
			{
				Item_AddInventory(iDaikatana,ePlayer);

				{
					Weapon_t *wStartWeapon = Weapon_GetWeapon(WEAPON_DAIKATANA);
					if(wStartWeapon)
						Weapon_SetActive(wStartWeapon,ePlayer,false);
				}
			}
		}
#endif
	}

	eSpawnPoint = Player_GetSpawnEntity(ePlayer, INFO_PLAYER_START);
	if(eSpawnPoint)
	{
		// Just copy our position and angle.
		Math_VectorCopy(eSpawnPoint->v.origin,ePlayer->v.origin);
		Math_VectorCopy(eSpawnPoint->v.angles,ePlayer->v.angles);
	}
	else
	{
		Engine.Con_Warning("Failed to find spawn point for player! (%s)\n",ePlayer->v.netname);

		Math_VectorClear(ePlayer->v.angles);
		Math_VectorClear(ePlayer->v.origin);
	}

	// Ensure we haven't spawned within the world.
	if(Engine.Server_PointContents(ePlayer->v.origin) == BSP_CONTENTS_SOLID)
		// Let us know if we have done, just so it's logged.
		Engine.Con_Warning("Player spawned inside world! (%s)\n",ePlayer->v.netname);

	// Reset the players animation, so the animation from their previous lives don't continue.
	Entity_ResetAnimation(ePlayer);

	Entity_SetModel(ePlayer, ePlayer->v.model);
	Entity_SetSize(ePlayer,-16.0f,-16.0f,-36.0f,16.0f,16.0f,36.0f);
	Entity_SetAngles(ePlayer, ePlayer->v.angles);
	Entity_SetOrigin(ePlayer, ePlayer->v.origin);

	iPlayerModelIndex = ePlayer->v.modelindex;
}

void Player_Jump(ServerEntity_t *ePlayer)
{
	char cJumpSound[32];

	// better ladder stuff ~eukara
	if ((ePlayer->local.dLadderTime > Server.dTime) && (ePlayer->local.dLadderJump < Server.dTime)) {

		if (!(ePlayer->v.flags & FL_ONGROUND))
			ePlayer->v.flags = ePlayer->v.flags + FL_ONGROUND;

		ePlayer->local.dLadderJump = Server.dTime + 0.4;
		ePlayer->v.velocity[2] = 0;

		plAngleVectors(ePlayer->v.angles, ePlayer->local.vForward, ePlayer->local.vRight, ePlayer->local.vUp);
		ePlayer->v.velocity[0] += (ePlayer->local.vForward[0] * 100);
		ePlayer->v.velocity[1] += (ePlayer->local.vForward[1] * 100);
		ePlayer->v.velocity[2] += (ePlayer->local.vForward[2] * 100);
	}

	// Don't let us jump while inside a vehicle.
	if(ePlayer->v.flags & FL_WATERJUMP || ePlayer->local.eVehicle)
		return;
	else if(ePlayer->v.waterlevel >= 2)
	{
		switch(ePlayer->v.watertype)
		{
		case BSP_CONTENTS_WATER:
			ePlayer->v.velocity[2] = 100.0f;
			break;
		case BSP_CONTENTS_SLIME:
			ePlayer->v.velocity[2] = 80.0f;
			break;
		default:
			ePlayer->v.velocity[2] = 50.0f;
		}

		if(ePlayer->local.swim_flag < Server.dTime)
		{
			ePlayer->local.swim_flag = (float)(Server.dTime+1.0);

			Sound(ePlayer,CHAN_BODY,"player/playerswim1.wav",255,ATTN_NORM);
		}
		return;
	}
	else if(!(ePlayer->v.flags & FL_ONGROUND) || !(ePlayer->v.flags & FL_JUMPRELEASED))
		return;

	ePlayer->v.flags		-= (ePlayer->v.flags & FL_JUMPRELEASED);
	ePlayer->v.flags		-= FL_ONGROUND;
	ePlayer->v.button[2]	= 0;

#ifdef GAME_OPENKATANA
	if(ePlayer->local.acro_finished > Server.dTime)
	{
		ePlayer->v.velocity[2] += 440.0f;

		sprintf(cJumpSound,"player/acroboost.wav");
	}
	else
#endif
	{
		ePlayer->v.velocity[2] += 250.0f;

		PLAYER_SOUND_JUMP(cJumpSound);
	}

	Sound(ePlayer,CHAN_VOICE,cJumpSound,255,ATTN_NORM);

	// Play a step sound too, so it sounds like they're pushing off the ground.
	Sound(ePlayer, CHAN_AUTO, va("physics/concrete%i_footstep.wav", rand() % 4), 250, ATTN_NORM);

	ePlayer->v.punchangle[0] += 3.0f;

	if((ePlayer->v.velocity[0] == 0) && (ePlayer->v.velocity[1] == 0))
		Entity_Animate(ePlayer,PlayerAnimation_Jump);
	else
		Entity_Animate(ePlayer,PlayerAnimation_RunJump);
}

#ifdef GAME_OPENKATANA
void Player_CheckPowerups(ServerEntity_t *ePlayer)
{
	Item_t	*iPowerBoost,
			*iVitalityBoost,
			*iSpeedBoost,
			*iAttackBoost,
			*iAcroBoost;

	if(ePlayer->v.iHealth <= 0)
		return;

	iPowerBoost = Item_GetInventory(ITEM_POWERBOOST, ePlayer);
	iVitalityBoost = Item_GetInventory(ITEM_VITABOOST, ePlayer);
	iSpeedBoost = Item_GetInventory(ITEM_SPEEDBOOST, ePlayer);
	iAttackBoost = Item_GetInventory(ITEM_ATTACKBOOST, ePlayer);
	iAcroBoost = Item_GetInventory(ITEM_ACROBOOST, ePlayer);

	if(iPowerBoost && ePlayer->local.power_finished)
	{
		if(ePlayer->local.power_time == 1)
			if(ePlayer->local.power_finished < Server.dTime+3.0)
			{
				Engine.CenterPrint(ePlayer,"Your power boost is running out.\n");

				ePlayer->local.power_time = Server.dTime+1.0;
			}

		if(ePlayer->local.power_finished < Server.dTime)
		{
			Item_RemoveInventory(iPowerBoost,ePlayer);

			ePlayer->local.power_finished	=
			ePlayer->local.power_time		= 0;
		}
	}

	if(iSpeedBoost && ePlayer->local.speed_finished)
	{
		if(ePlayer->local.speed_time == 1)
			if(ePlayer->local.speed_finished < Server.dTime+3.0)
			{
				Engine.CenterPrint(ePlayer,"Your speed boost is running out.\n");

				ePlayer->local.speed_time = Server.dTime+1.0;
			}

		if(ePlayer->local.speed_finished < Server.dTime)
		{
			Item_RemoveInventory(iSpeedBoost,ePlayer);

			ePlayer->local.speed_finished =
			ePlayer->local.speed_time = 0;
		}
	}

	if(iAttackBoost && ePlayer->local.attackb_finished)
	{
		if(ePlayer->local.attackb_time == 1.0f)
			if(ePlayer->local.attackb_finished < Server.dTime+3.0)
			{
				// [25/8/2012] Updated to use centerprint instead ~hogsy
				Engine.CenterPrint(ePlayer,"Your attack boost is running out.\n");

				ePlayer->local.attackb_time = Server.dTime+1.0;
			}

		if(ePlayer->local.attackb_finished < Server.dTime)
		{
			Item_RemoveInventory(iAttackBoost,ePlayer);

			ePlayer->local.attackb_finished =
			ePlayer->local.attackb_time		= 0;
		}
	}

	if(iAcroBoost && ePlayer->local.acro_finished)
	{
		if(ePlayer->local.acro_time == 1)
			if(ePlayer->local.acro_finished < Server.dTime+3.0)
			{
				// [25/8/2012] Updated to use centerprint instead ~hogsy
				Engine.CenterPrint(ePlayer,"Your acro boost is running out.\n");

				ePlayer->local.acro_time = Server.dTime+1.0;
			}

		if(ePlayer->local.acro_finished < Server.dTime)
		{
			Item_RemoveInventory(iAcroBoost,ePlayer);

			ePlayer->local.acro_finished	=
			ePlayer->local.acro_time		= 0;
		}
	}

	if(iVitalityBoost && ePlayer->local.vita_finished)
	{
		if(ePlayer->local.vita_time == 1)
			if(ePlayer->local.vita_finished < Server.dTime+3.0)
			{
				// [25/8/2012] Updated to use centerprint instead ~hogsy
				Engine.CenterPrint(ePlayer,"Your vitality boost is running out.\n");

				ePlayer->local.vita_time = Server.dTime+1.0;
			}

		if(ePlayer->local.vita_finished < Server.dTime)
		{
			Item_RemoveInventory(iVitalityBoost,ePlayer);

			ePlayer->local.vita_finished	=
			ePlayer->local.vita_time		= 0;
		}
	}
}
#endif

// [23/3/2013] Countdown is now done for cooperative mode too! ~hogsy
void Player_DeathThink(ServerEntity_t *entity)
{
	if (entity->v.button[0] ||
		entity->v.button[1] ||
		entity->v.button[2])
	{
		// [25/8/2012] Simplified ~hogsy
		Math_VectorClear(entity->v.button);

		if (!entity->local.fSpawnDelay)
		{
			// [25/8/2012] We don't respawn in singleplayer ~hogsy
			// [23/3/2013] Oops! Fixed, we were checking for the wrong case here :) ~hogsy
			if(bIsMultiplayer)
				// [16/10/2013] Swapped out for the more "correct" Player_Spawn rather than via Monster_Respawn ~hogsy
				Player_Spawn(entity);
			else
			{
				Engine.Server_Restart();
				return;
			}
		}
	}

	// [25/8/2012] If it's multiplayer and not coop, countdown ~hogsy
	// [15/8/2013] Countdown for both singleplayer and multiplayer ~hogsy
	if (entity->local.fSpawnDelay)
		// [25/8/2012] TODO: Force respawn if timer runs out? ~hogsy
		entity->local.fSpawnDelay -= 0.5f;
}

void Player_Use(ServerEntity_t *entity)
{
	if (entity->v.iHealth <= 0 || entity->local.dAttackFinished > Server.dTime)
		return;

	// If nothing usable is being aimed at then play sound...
	// TODO: Find a more appropriate sound :)
	Sound(entity, CHAN_VOICE, "player/playerpain3.wav", 255, ATTN_NORM);

	entity->local.dAttackFinished = Server.dTime + 0.5;
}

/*
	Movement
*/

/*	Called per-frame to handle player movement for each client.
*/
void Player_MoveThink(ServerEntity_t *ePlayer)
{
#if 0
	vec3_t	vViewAngle;
	float	fLength,*fPlayerAngles;

	if(ePlayer->v.movetype == MOVETYPE_NONE)
		return;

	fLength = plVectorNormalize(ePlayer->v.punchangle);
	fLength -= 10*(float)Engine.Server_GetFrameTime();
	if(fLength < 0)
		fLength = 0;

	Math_VectorScale(ePlayer->v.punchangle,fLength,ePlayer->v.punchangle);

	// If dead, behave differently
	if(!ePlayer->v.iHealth)
		return;

#ifdef IMPLEMENT_ME
	cmd = host_client->cmd;
#endif
	fPlayerAngles = ePlayer->v.angles;

	Math_VectorAdd(ePlayer->v.v_angle,ePlayer->v.punchangle,vViewAngle);
#endif
}
