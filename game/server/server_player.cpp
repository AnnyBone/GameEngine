/*
Copyright (C) 1996-1997 Id Software, Inc.
Copyright (C) 2011-2015 Marco 'eukara' Hladik <eukos@oldtimes-software.com>
Copyright (C) 2011-2017 Mark E Sowden <markelswo@gmail.com>

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
//#include "server_vehicle.h"
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

int playermodelindex;

ServerEntityFrame_t PlayerAnimation_Idle[] =
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

ServerEntityFrame_t PlayerAnimation_Fire[] =
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

ServerEntityFrame_t PlayerAnimation_Walk[] =
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

ServerEntityFrame_t PlayerAnimation_Death1[] =
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

ServerEntityFrame_t PlayerAnimation_Death2[] =
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

ServerEntityFrame_t PlayerAnimation_Jump[] =
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

ServerEntityFrame_t PlayerAnimation_RunJump[] =
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

ServerEntityFrame_t PlayerAnimation_KatanaIdle[] =
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

ServerEntityFrame_t PlayerAnimation_KatanaAttack1[] =
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

ServerEntityFrame_t PlayerAnimation_KatanaAttack2[] =
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

ServerEntityFrame_t PlayerAnimation_KatanaDeath1[] =
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
    // Also check movetype so we don't do steps while noclipping/flying.
    if ((player->v.movetype == MOVETYPE_WALK) && player->v.flags & FL_ONGROUND) {
        if (player->local.steptime > Server.time) {
            return;
        } else if (
            // Ensure there's enough movement that calls for us to produce a footstep.
                (player->v.velocity.x < 5) && (player->v.velocity.x > -5) &&
                (player->v.velocity.y < 5) && (player->v.velocity.y > -5)) {
            return;
        }

        PLVector3D step;
        step[0] = player->v.velocity[0];
        if(step[0] < 0) {
            step[0] *= -1.0f;
        }

        step[1] = player->v.velocity[1];
        if(step[1] < 0) {
            step[1] *= -1.0f;
        }

        float force = step.x + step.y;

        // Base this on our velocity.
        double delay = Math_Clamp(0.1, (double)(1.0f / (force / 100.0f)), 1.0);

        // TODO: Check if we're in water or not and change this accordingly :)
        Sound(player, CHAN_BODY, va("physics/concrete%i_footstep.wav", rand() % 4), 150, ATTN_NORM);

        player->local.steptime = Server.time + delay;
    }
}

void Player_CheckWater(ServerEntity_t *player)
{
    // [2/8/2014] Basic Drowning... ~eukos
    if(player->v.waterlevel != 3) {
        player->local.air_finished = Server.time + 12;
    } else if(player->local.air_finished < Server.time) {
        if(player->local.pain_finished < Server.time) {
            Entity_Damage(player, player, 10, DAMAGE_TYPE_DROWN);

            player->local.pain_finished = Server.time + 1;
        }
    }
}

void Player_PostThink(ServerEntity_t *player) {
    // If round has not started then don't go through this!
    if ((player->Monster.state == AI_STATE_DEAD) || !Server.round_started) {
        return;
    }
#if 0 // todo, rewrite
    // Check if we're in a vehicle.
	else if(ePlayer->local.vehicle)
	{
		switch(ePlayer->local.vehicle->Vehicle.iSlot[ePlayer->local.iVehicleSlot])
		{
		case SLOT_DRIVER:
			Math_VectorCopy(ePlayer->local.vehicle->v.origin,ePlayer->v.origin);
			break;
		case SLOT_PASSENGER:
			break;
		default:
			Engine.Con_Warning("Player (%s) is occupying an unknown vehicle slot (%i)!\n",
				ePlayer->v.netname,ePlayer->local.iVehicleSlot);

			Vehicle_Exit(ePlayer->local.vehicle,ePlayer);
		}
		return;
	}
#endif

    Weapon_CheckInput(player);

    if ((player->local.jump_velocity < -300.0f) &&
        (player->v.flags & FL_ONGROUND)			&&
        (player->v.health > 0)) {
        char snd[32];
        if(player->v.watertype == BSP_CONTENTS_WATER) {
            sprintf(snd, "player/h2ojump.wav");
        } else if (player->local.jump_velocity < -650.0f) {
            // TODO: Figure out if we were pushed by an enemy.
            // TODO: Base damage on velocity.
            Entity_Damage(player, player, 10, DAMAGE_TYPE_FALL);

            // todo, client start
            if(rand()%2 == 1) {
                sprintf(snd, "player/playerlandhurt.wav");
            } else {
                sprintf(snd, "player/playerlandhurt2.wav");
            }
            // todo, client end

            // todo, client start
            player->v.punchangle.x += (float)(rand()%5+2)*7.0f;	// [13/4/2013] Give him a big punch... ~eukos
            // todo, client end
        } else {
            // todo, client start
            // Land sounds DO NOT use CHAN_VOICE otherwise they get horribly cut out!
            strncpy(snd, "player/land0.wav", sizeof(snd));

            // Give him a little punch...
            player->v.punchangle.x -= player->local.jump_velocity / 100.0f;
            // todo, client end
        }

        // todo, client start
        Sound(player, CHAN_VOICE, snd, 255, ATTN_NORM);
        // todo, client end

        player->local.jump_velocity = 0;
    }

    if(!(player->v.flags & FL_ONGROUND)) {
        player->local.jump_velocity = player->v.velocity.z;
    } else if((	(player->v.velocity.x < -4.0f || player->v.velocity.x > 4.0f)	||
                (player->v.velocity.y < -4.0f || player->v.velocity.y > 4.0f))	&&
                (!player->local.animation_time || player->local.animation_end == 9)) {
        Entity_Animate(player, PlayerAnimation_Walk);
    } else if(  (player->v.velocity.x == 0 || player->v.velocity.y == 0) &&
                (!player->local.animation_time || player->local.animation_end == 46)) {
#ifdef GAME_OPENKATANA
        if(player->v.iActiveWeapon == WEAPON_DAIKATANA) {
            Entity_Animate(player, PlayerAnimation_KatanaIdle);
        } else
#endif
            Entity_Animate(player,PlayerAnimation_Idle);
    }

    Player_CheckFootsteps(player);
#ifdef GAME_OPENKATANA
    Player_CheckPowerups(player);
#endif
}

void Player_PreThink(ServerEntity_t *player) {
    if (!Server.round_started)
        return;

    if (Server.round_started && !Server.players_spawned) {
        // Spawn the player!
        Player_Spawn(player);
    }

    Weapon_CheckFrames(player);
    Entity_CheckFrames(player);
    Player_CheckWater(player);

    if (player->Monster.state == AI_STATE_DEAD) {
        Player_DeathThink(player);
        return;
    }

    // ladders only work in ZeroG ~eukara
    if (player->local.zerogtime < Server.time) {
        player->Physics.gravity = cv_server_gravity.value;
    } else {
        player->Physics.gravity = 0;
    }

    if(player->v.waterlevel == 2) {
        // Check if we can jump onto an edge, was originally in a seperate function but merged here instead.
        Math_AngleVectors(player->v.angles, &player->local.forward, &player->local.right, &player->local.vUp);

        PLVector3D trace_start = player->v.origin;
        trace_start[2] += 8.0f;

        player->local.forward[2] = 0;
        player->local.forward.Normalize();

        PLVector3D trace_end;
        for(int i = 0; i < 3; i++) {
            trace_end[i] = trace_start[i] + player->local.forward[i] * 24;
        }

        trace_t trace = Engine.Server_Move(trace_start, trace_end, PLVector3D(0, 0, 0), PLVector3D(0, 0, 0), true, player);
        if(trace.fraction < 1) {
            trace_start[2] += player->v.maxs[2] - 8;

            for(int i = 0; i < 3; i++) {
                trace_end[i] = trace_start[i] + player->local.forward[i] * 24;
            }

            player->v.movedir = trace.plane.normal - 5;

            trace = g_engine->Server_Move(trace_start, trace_end, PLVector3D(0, 0, 0), PLVector3D(0, 0, 0), true, player);
            if(trace.fraction == 1.0f) {
                player->v.flags         |= FL_WATERJUMP;
                player->v.flags		    -= (player->v.flags & FL_JUMPRELEASED);
                player->v.velocity[2]   = 225.0f;
            }
        }
    }

    if(player->v.button[2]) {
        Player_Jump(player);
    } else {
        player->v.flags |= FL_JUMPRELEASED;
    }

    // Crouch
    if(player->v.button[1]) {
        if (!(player->v.flags & FL_CROUCHING)) {
            Entity_SetSize(player, -16.0f, -16.0f, -18.0f, 16.0f, 16.0f, 18.0f);

            player->v.view_ofs.z = 10.0f;

            player->v.flags |= FL_CROUCHING;
        }
    } else if (player->v.flags & FL_CROUCHING) { // Uncrouch
        PLVector3D maxs = { 16.0f, 16.0f, 36.0f }, mins = { -16.0f, -16.0f, -36.0f };

        PLVector3D origin = player->v.origin;
        origin[2] += 20.0f;

        // Trace to ensure we have enough room to stand.
        trace_t stand_check = g_engine->Server_Move(origin, mins, maxs, origin, 0, player);
        if (!stand_check.all_solid) {
            Entity_SetSizeVector(player, mins, maxs);

            player->v.view_ofs.z = 28.0f;

            // Not crouching anymore.
            player->v.flags -= (player->v.flags & FL_CROUCHING);
        }
    }

    if(cvServerWaypointSpawn.value && (Server.time >= Server.waypointspawndelay)) {
        if(player->v.movetype != MOVETYPE_WALK) {
            return;
        }

        if(player->v.flags & FL_ONGROUND) {
            // Create a waypoint at our current position.
            Waypoint_Spawn(player->v.origin, WAYPOINT_TYPE_DEFAULT);
        } else if((player->v.flags & FL_SWIM) /*&& !(player->v.flags & FL_ONGROUND)*/) {
            Waypoint_Spawn(player->v.origin, WAYPOINT_TYPE_SWIM);
            // Create waypoints in the air.
        } else if(!(player->v.flags & FL_ONGROUND)) {
            Waypoint_Spawn(player->v.origin, WAYPOINT_TYPE_JUMP);
        }

        Server.waypointspawndelay = Server.time+((double)cvServerWaypointDelay.value);
    }
}

void Player_Gib(ServerEntity_t *player) {
    ThrowGib(player->v.origin, player->v.velocity, PHYSICS_MODEL_GIB0, (float)player->v.health*-1, true);
    ThrowGib(player->v.origin, player->v.velocity, PHYSICS_MODEL_GIB1, (float)player->v.health*-1, true);
    ThrowGib(player->v.origin, player->v.velocity, PHYSICS_MODEL_GIB2, (float)player->v.health*-1, true);
    ThrowGib(player->v.origin, player->v.velocity, PHYSICS_MODEL_GIB3, (float)player->v.health*-1, true);

    // todo, client start
    Sound(player, CHAN_VOICE, "misc/gib1.wav", 255, ATTN_NORM);
    // todo, client end
    ServerEffect_BloodCloud(player->v.origin, BLOOD_TYPE_RED);

    Entity_SetModel(player, "");
}

void Player_Die(ServerEntity_t *player, ServerEntity_t *other, EntityDamageType_t type) {
    /*	TODO:
        Slowly fade our screen to red (or black?)
        Camera should follow the entity that killed us?
    */

    player->v.view_ofs = 0;

    player->v.modelindex    = playermodelindex;
    player->v.view_ofs[2]   = -8.0f;
    player->v.flags		-= (player->v.flags & FL_ONGROUND);
    player->v.movetype  = MOVETYPE_TOSS;
    player->v.angles[0]	= player->v.angles[2] = 0;

    player->Monster.state = AI_STATE_DEAD;

    player->Physics.solid	= SOLID_NOT;

#ifdef GAME_OPENKATANA
    // Detonate all C4 bombs we've laid out!
    C4Vizatergo_SecondaryAttack(player);
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

    char s[32];
    if(player->v.waterlevel == 3) {
        sprintf(s, "player/playerwaterdeath.wav");
    } else {
        sprintf(s, "player/playerdeath%i.wav", rand() % 4 + 1);
    }

    Sound(player, CHAN_VOICE, s, 255, ATTN_NONE);

#ifdef GAME_OPENKATANA
    if(player->v.iActiveWeapon == WEAPON_DAIKATANA) {
        Entity_Animate(player, PlayerAnimation_KatanaDeath1);
    } else
#endif
    {
        if(rand()%2 == 1) {
            Entity_Animate(player, PlayerAnimation_Death1);
        } else {
            Entity_Animate(player, PlayerAnimation_Death2);
        }
    }
}

void Player_Pain(ServerEntity_t *ent, ServerEntity_t *other, EntityDamageType_t type) {
    // todo, client start
    char sound[32];
    if(type == DAMAGE_TYPE_DROWN) {
        // todo, drowning sound...
        sprintf(sound, "player/playerdeath%i.wav", rand() % 4 + 1);
    } else {
        PLAYER_SOUND_PAIN(sound);
    }
    Sound(ent, CHAN_VOICE, sound, 255, ATTN_NORM);
    // todo, client end
}

int	slot;

/*	Find a random spawn point for the entity (point_start).
*/
ServerEntity_t *Player_GetSpawnEntity(ServerEntity_t *player, int type) {
    const char *startname;
    switch (type)
    {
        case INFO_PLAYER_DEATHMATCH:
            startname = "point_deathmatch";
            break;
        case INFO_PLAYER_CTF:
            if (player->local.team == TEAM_RED) {
                startname = "point_start_red";
            } else if (player->local.team == TEAM_BLUE) {
                startname = "point_start_blue";
            } else {
                Engine.Con_Warning("Unknown team type for spawn point! (%i)", player->local.team);
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

    ServerEntity_t *spawn = g_engine->Server_FindEntity(Server.world, startname, true);
    if (spawn) {
        if (type == spawn->local.style) {
            return spawn;
        }
    }

    return NULL;
}

void Player_Spawn(ServerEntity_t *player) {
    player->Monster.type = MONSTER_PLAYER;

    player->v.classname = "player";
    player->v.health = cvServerDefaultHealth.iValue;
    player->v.movetype = MOVETYPE_WALK;
    player->v.takedamage = true;
    player->v.model = cvServerPlayerModel.string;
    player->v.effects = 0;

    player->Physics.solid = SOLID_SLIDEBOX;
    player->Physics.mass = 1.4f;
    player->Physics.gravity = SERVER_GRAVITY;
    player->Physics.friction = 4.0f;

    player->local.maxhealth = PLAYER_MAX_HEALTH;				// Set the players default maximum health.
    player->local.spawndelay = cvServerRespawnDelay.value;	// Set the delay before we spawn.
    player->local.team = TEAM_NEUTRAL;						// Set the default team.
    player->local.bleed = true;								// The player bleeds!

    // Clear the velocity and current view offset.
    player->v.velocity = 0;
    player->v.view_ofs = 0;

    player->v.bFixAngle = true;
    player->v.view_ofs.z = 28.0f;

    // Set our think functions.
    Entity_SetDamagedFunction(player, Player_Pain);
    Entity_SetKilledFunction(player, Player_Die);

    // Clear out the players inventory.
    Item_ClearInventory(player);

    // Let the server know that a player has spawned.
    Server.players_spawned = true;

    if(g_ismultiplayer) {
#ifdef GAME_OPENKATANA
        switch(cvServerGameMode.iValue) {
            // TODO: Check what model this player has set in a cvar
            // TODO: Set texture function for team based skins?
            case MODE_CAPTURETHEFLAG:
                if(!player->local.team) {
                    if(!slot) {
                        player->local.team = TEAM_BLUE;
                        g_engine->Server_BroadcastPrint("%s has been assigned to the blue team!\n",player->v.netname);

                        slot++;
                    } else {
                        player->local.team = TEAM_RED;
                        g_engine->Server_BroadcastPrint("%s has been assigned to the red team!\n",player->v.netname);

                        slot = 0;
                    }

#if 0
                    {
					ent->local.team = TEAM_SPECTATOR;
					Game.Server_BroadcastPrint("%s is spectating.\n",player->v.netname);
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
                switch(player->local.team) {
                    case TEAM_BLUE:
                        player->v.model = "models/sprfly.mdl";
                        break;
                    case TEAM_RED:
                        player->v.model = "models/mikiko.mdl";
                        break;
                    case TEAM_SPECTATOR:
                        player->v.model = "";
                        break;
                    default:
                        player->v.model = "models/player.md2";
                        break;
                }
                break;
            case MODE_DEATHMATCH:
                Deathmatch_Spawn(player);
                break;
            default: break;
        }
#endif
    } else { // SINGLEPLAYER
        ServerMenu_UpdateClient(player, MENU_STATE_HUD, true);

#ifdef GAME_OPENKATANA
        Item_t *item_daikatana = Item_GetItem(WEAPON_DAIKATANA);
        if(item_daikatana) {
            Item_AddInventory(item_daikatana, player);

            Weapon_t *weapon_daikatana = Weapon_GetWeapon(WEAPON_DAIKATANA);
            if(weapon_daikatana) {
                Weapon_SetActive(weapon_daikatana,player,false);
            }
        }
#endif
    }

    ServerEntity_t *spawn = Player_GetSpawnEntity(player, INFO_PLAYER_START);
    if(spawn) {
        // Just copy our position and angle.
        player->v.origin = spawn->v.origin;
        player->v.angles = spawn->v.angles;
    } else {
        g_engine->Con_Warning("Failed to find spawn point for player! (%s)\n",player->v.netname);

        player->v.angles = 0;
        player->v.origin = 0;
    }

    // Ensure we haven't spawned within the world.
    if(g_engine->Server_PointContents(player->v.origin) == BSP_CONTENTS_SOLID) {
        // Let us know if we have done, just so it's logged.
        g_engine->Con_Warning("Player spawned inside world! (%s)\n", player->v.netname);
    }

    // Reset the players animation, so the animation from their previous lives don't continue.
    Entity_ResetAnimation(player);

    Entity_SetModel(player, player->v.model);
    Entity_SetSize(player,-16.0f,-16.0f,-36.0f,16.0f,16.0f,36.0f);
    Entity_SetAngles(player, player->v.angles);
    Entity_SetOrigin(player, player->v.origin);

    playermodelindex = player->v.modelindex;
}

void Player_Jump(ServerEntity_t *player) {
    // better ladder stuff ~eukara
    if ((player->local.laddertime > Server.time) && (player->local.ladderjump < Server.time)) {
        if (!(player->v.flags & FL_ONGROUND))
            player->v.flags = player->v.flags + FL_ONGROUND;

        player->local.ladderjump = Server.time + 0.4;
        player->v.velocity.z = 0;

        Math_AngleVectors(player->v.angles, &player->local.forward, &player->local.right, &player->local.vUp);
        player->v.velocity.x += (player->local.forward.x * 100);
        player->v.velocity.y += (player->local.forward.y * 100);
        player->v.velocity.z += (player->local.forward.z * 100);
    }

    // Don't let us jump while inside a vehicle.
    if(player->v.flags & FL_WATERJUMP || player->local.vehicle) {
        return;
    } else if(player->v.waterlevel >= 2) {
        switch(player->v.watertype) {
            case BSP_CONTENTS_WATER:
                player->v.velocity.z = 100.0f;
                break;
            case BSP_CONTENTS_SLIME:
                player->v.velocity.z = 80.0f;
                break;
            default:
                player->v.velocity.z = 50.0f;
        }

        if(player->local.swim_flag < Server.time) {
            player->local.swim_flag = (float)(Server.time+1.0);

            Sound(player,CHAN_BODY,"player/playerswim1.wav",255,ATTN_NORM);
        }
        return;
    } else if(!(player->v.flags & FL_ONGROUND) || !(player->v.flags & FL_JUMPRELEASED)) {
        return;
    }

    player->v.flags		-= (player->v.flags & FL_JUMPRELEASED);
    player->v.flags		-= FL_ONGROUND;
    player->v.button[2]	= 0;

    char jump_sound[32];
#ifdef GAME_OPENKATANA
    if(player->local.acro_finished > Server.time) {
        player->v.velocity.z += 440.0f;

        sprintf(jump_sound,"player/acroboost.wav");
    } else
#endif
    {
        player->v.velocity.z += 250.0f;

        PLAYER_SOUND_JUMP(jump_sound);
    }

    Sound(player,CHAN_VOICE,jump_sound,255,ATTN_NORM);

    // Play a step sound too, so it sounds like they're pushing off the ground.
    Sound(player, CHAN_AUTO, va("physics/concrete%i_footstep.wav", rand() % 4), 250, ATTN_NORM);

    player->v.punchangle.x += 3.0f;

    if((player->v.velocity.x == 0) && (player->v.velocity.y == 0)) {
        Entity_Animate(player, PlayerAnimation_Jump);
    } else {
        Entity_Animate(player, PlayerAnimation_RunJump);
    }
}

#ifdef GAME_OPENKATANA
void Player_CheckPowerups(ServerEntity_t *player) {
    if(player->v.health <= 0) {
        return;
    }

    Item_t *powerboost = Item_GetInventory(ITEM_POWERBOOST, player);
    if(powerboost && player->local.power_finished) {
        if(player->local.power_time == 1) {
            if (player->local.power_finished < Server.time + 3.0) {
                Engine.CenterPrint(player, "Your power boost is running out.\n");

                player->local.power_time = Server.time + 1.0;
            }
        }

        if(player->local.power_finished < Server.time) {
            Item_RemoveInventory(powerboost, player);

            player->local.power_finished	=
            player->local.power_time		= 0;
        }
    }

    Item_t *speedboost = Item_GetInventory(ITEM_SPEEDBOOST, player);
    if(speedboost && player->local.speed_finished) {
        if(player->local.speed_time == 1) {
            if (player->local.speed_finished < Server.time + 3.0) {
                Engine.CenterPrint(player, "Your speed boost is running out.\n");

                player->local.speed_time = Server.time + 1.0;
            }
        }

        if(player->local.speed_finished < Server.time) {
            Item_RemoveInventory(speedboost, player);

            player->local.speed_finished =
            player->local.speed_time = 0;
        }
    }

    Item_t *attackboost = Item_GetInventory(ITEM_ATTACKBOOST, player);
    if(attackboost && player->local.attackb_finished) {
        if(player->local.attackb_time == 1.0f) {
            if (player->local.attackb_finished < Server.time + 3.0) {
                Engine.CenterPrint(player, "Your attack boost is running out.\n");

                player->local.attackb_time = Server.time + 1.0;
            }
        }

        if(player->local.attackb_finished < Server.time) {
            Item_RemoveInventory(attackboost, player);

            player->local.attackb_finished =
            player->local.attackb_time		= 0;
        }
    }

    Item_t *acroboost = Item_GetInventory(ITEM_ACROBOOST, player);
    if(acroboost && player->local.acro_finished) {
        if(player->local.acro_time == 1) {
            if (player->local.acro_finished < Server.time + 3.0) {
                Engine.CenterPrint(player, "Your acro boost is running out.\n");

                player->local.acro_time = Server.time + 1.0;
            }
        }

        if(player->local.acro_finished < Server.time) {
            Item_RemoveInventory(acroboost, player);

            player->local.acro_finished	=
            player->local.acro_time		= 0;
        }
    }

    Item_t *vitalityboost = Item_GetInventory(ITEM_VITABOOST, player);
    if(vitalityboost && player->local.vita_finished) {
        if(player->local.vita_time == 1) {
            if (player->local.vita_finished < Server.time + 3.0) {
                g_engine->CenterPrint(player, "Your vitality boost is running out.\n");

                player->local.vita_time = Server.time + 1.0;
            }
        }

        if(player->local.vita_finished < Server.time) {
            Item_RemoveInventory(vitalityboost, player);

            player->local.vita_finished	=
            player->local.vita_time		= 0;
        }
    }
}
#endif

void Player_DeathThink(ServerEntity_t *player) {
    if (player->v.button[0] ||
        player->v.button[1] ||
        player->v.button[2])
    {
        memset(player->v.button, 0, 3);

        if (!player->local.spawndelay) {
            if(g_ismultiplayer) {
                Player_Spawn(player);
            } else {
                g_engine->Server_Restart();
                return;
            }
        }
    }

    // If it's multiplayer and not coop, countdown.
    if (player->local.spawndelay) {
        // [25/8/2012] TODO: Force respawn if timer runs out? ~hogsy
        player->local.spawndelay -= 0.5f;
    }
}

void Player_Use(ServerEntity_t *entity) {
    if (entity->v.health <= 0 || entity->local.dAttackFinished > Server.time) {
        return;
    }

    // If nothing usable is being aimed at then play sound...
    // TODO: Find a more appropriate sound :)
    // todo, client start
    Sound(entity, CHAN_VOICE, "player/playerpain3.wav", 255, ATTN_NORM);
    // todo, client end

    entity->local.dAttackFinished = Server.time + 0.5;
}

/*
	Movement
*/

// Called per-frame to handle player movement for each client.
void Player_MoveThink(ServerEntity_t *player) {
#if 0
    plVector3f_t	vViewAngle;
	float			fLength,*fPlayerAngles;

	if(ePlayer->v.movetype == MOVETYPE_NONE)
		return;

	fLength = plVectorNormalize(ePlayer->v.punchangle);
	fLength -= 10*(float)Engine.Server_GetFrameTime();
	if(fLength < 0)
		fLength = 0;

	Math_VectorScale(ePlayer->v.punchangle,fLength,ePlayer->v.punchangle);

	// If dead, behave differently
	if(!ePlayer->v.health)
		return;

#ifdef IMPLEMENT_ME
	cmd = host_client->cmd;
#endif
	fPlayerAngles = ePlayer->v.angles;

	Math_VectorAdd(ePlayer->v.v_angle,ePlayer->v.punchangle,vViewAngle);
#endif
}
