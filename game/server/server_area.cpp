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

#include <shared_game.h>
#include "server_main.h"

/*
	Brush-based entities that span across an area.
	This document contains the code for the following
	entities and its sub-methods:

	area_breakable 		- Brush that breaks and spawns debris
	area_rotate			- Rotates around specified axis
	area_door			- Opens and closes in specified position
	area_changelevel	- Changes the current level
	area_trigger		- Triggers a point entity
	area_pushable		- Brush that can be pushed around
	area_wall			- Brush that casts no shadow and can appear/disappear on use
	area_button			- Trigger a point entity when moved into place
	area_platform		- Platform that travels between two destinations
	area_climb			- Substitute for a ladder
	area_noclip			- Brush that casts no light and no collision
	area_push			- Pushes entities into the specified direction on touch

TODO:
	area_monsterclip ?	- Blocks ways off for monsters
*/

void Area_SetMoveDirection(PLVector3D &angles, PLVector3D &direction) {
	if(angles == PLVector3D(0, -1, 0)) {
        direction = PLVector3D(0, 0, 1);
	} else if(angles == PLVector3D(0, -2, 0)) {
        direction = PLVector3D(0, 0, -1);
    } else {
        Math_AngleVectors(angles, &direction, nullptr, nullptr);
    }

    plClearVector3D(&angles);
}

void Area_CalculateMovementDone(ServerEntity_t *area) {
	Entity_SetOrigin(area, area->local.finaldest);

	area->v.velocity = 0;

	if(area->local.think1) {
        area->local.think1(area, area);
    }
}

void Area_Move(ServerEntity_t *area, PLVector3D dest, float speed, void(*Function)(ServerEntity_t *entity, ServerEntity_t *other)) {
    if(speed == 0) {
        // Very unlikely, but prevent a division by 0...
        return;
    }

    area->local.finaldest = dest;
    PLVector3D delta = dest - area->v.origin;

	float traveltime = delta.Length() / speed;
    area->v.velocity = delta * (1 / traveltime);

	area->local.think1 = Function;

	area->v.think = Area_CalculateMovementDone;
	area->v.nextthink = area->v.ltime + traveltime;
}

/*	area_breakable	*/

#define BREAKABLE_GLASS	0
#define	BREAKABLE_WOOD	1
#define	BREAKABLE_ROCK	2
#define	BREAKABLE_METAL	3

void Area_BreakableBounce(ServerEntity_t *gib, ServerEntity_t *other) {
	if(gib->v.flags & FL_ONGROUND) {
        return;
    }

    char sound[128] = { 0 };
	switch(gib->local.style) {
	case BREAKABLE_GLASS:
		PHYSICS_SOUND_GLASS(sound);
		break;
	case BREAKABLE_WOOD:
		PHYSICS_SOUND_WOOD(sound);
		break;
	case BREAKABLE_ROCK:
		PHYSICS_SOUND_ROCK(sound);
		break;
	case BREAKABLE_METAL:
		PHYSICS_SOUND_METAL(sound);
		break;
	default:
		g_engine->Con_Warning("Unknown breakable type! (%i)\n", gib->local.style);
		return;
	}

	Sound(gib,CHAN_AUTO,sound,30,ATTN_NORM);
}

void Area_CreateGib(ServerEntity_t *area, const char *model) {
	ServerEntity_t *gib = Entity_Spawn();
	if (gib) {
		gib->v.classname = "entity_gib";
		gib->v.movetype = MOVETYPE_BOUNCE;
		gib->v.TouchFunction = Area_BreakableBounce;
		gib->v.think = Entity_Remove;
		gib->v.nextthink = Server.time + 20;
		gib->v.takedamage = false;

		gib->Physics.solid = SOLID_TRIGGER;

		gib->local.style = area->local.style;

		gib->v.velocity.x = gib->v.avelocity.x = (float)(rand() % 5 * area->v.health * 5);
        gib->v.velocity.y = gib->v.avelocity.y = (float)(rand() % 5 * area->v.health * 5);
        gib->v.velocity.z = gib->v.avelocity.z = (float)(rand() % 5 * area->v.health * 5);

		Entity_SetModel(gib, model);
		Entity_SetOrigin(gib, area->v.oldorigin);
		Entity_SetSizeVector(gib, PLVector3D(), PLVector3D());
	}
}

void Area_BreakableDie(ServerEntity_t *area, ServerEntity_t *other, EntityDamageType_t type) {
	char sound[128], model[PL_SYSTEM_MAX_PATH];
	switch (area->local.style) {
	case BREAKABLE_GLASS:
		PHYSICS_SOUND_GLASS(sound);
		PHYSICS_MODEL_GLASS(model);
		break;
	case BREAKABLE_WOOD:
		PHYSICS_SOUND_WOOD(sound);
		PHYSICS_MODEL_WOOD(model);
		break;
	case BREAKABLE_ROCK:
		PHYSICS_SOUND_ROCK(sound);
		PHYSICS_MODEL_ROCK(model);
		break;
	case BREAKABLE_METAL:
		PHYSICS_SOUND_METAL(sound);
		PHYSICS_MODEL_METAL(model);
		break;
    default: return;
	}

	Sound(area, CHAN_AUTO, sound, 255, ATTN_STATIC);

	for (int i = 0; i < area->local.count; i++) {
        Area_CreateGib(area, model);
    }

	// Needs to be set to prevent a recursion.
	area->v.takedamage = false;

	if (area->v.targetname) { // Trigger doors, etc. ~eukos
        UseTargets(area, other);
    }

	Entity_Remove(area);
}

void Area_BreakableUse(ServerEntity_t *area) {
	Area_BreakableDie(area, area->local.activator, DAMAGE_TYPE_NONE);
}

void Area_BreakableSpawn(ServerEntity_t *area) {
	if (area->v.health <= 0) {
        area->v.health = 1;
    }

	switch (area->local.style) {
	case BREAKABLE_GLASS:
		Server_PrecacheSound( PHYSICS_SOUND_GLASS0);
		Server_PrecacheSound( PHYSICS_SOUND_GLASS1);
		Server_PrecacheSound( PHYSICS_SOUND_GLASS2);
		Server_PrecacheModel(PHYSICS_MODEL_GLASS0);
		Server_PrecacheModel(PHYSICS_MODEL_GLASS1);
		Server_PrecacheModel(PHYSICS_MODEL_GLASS2);
		break;
	case BREAKABLE_WOOD:
		Server_PrecacheSound( PHYSICS_SOUND_WOOD0);
		Server_PrecacheSound( PHYSICS_SOUND_WOOD1);
		Server_PrecacheSound( PHYSICS_SOUND_WOOD2);
		Server_PrecacheModel(PHYSICS_MODEL_WOOD0);
		Server_PrecacheModel(PHYSICS_MODEL_WOOD1);
		Server_PrecacheModel(PHYSICS_MODEL_WOOD2);
		break;
	case BREAKABLE_ROCK:
		Server_PrecacheSound( PHYSICS_SOUND_ROCK0);
		Server_PrecacheSound( PHYSICS_SOUND_ROCK1);
		Server_PrecacheSound( PHYSICS_SOUND_ROCK2);
		Server_PrecacheModel(PHYSICS_MODEL_ROCK0);
		Server_PrecacheModel(PHYSICS_MODEL_ROCK1);
		Server_PrecacheModel(PHYSICS_MODEL_ROCK2);
		break;
	case BREAKABLE_METAL:
		Server_PrecacheSound( PHYSICS_SOUND_METAL0);
		Server_PrecacheSound( PHYSICS_SOUND_METAL1);
		Server_PrecacheSound( PHYSICS_SOUND_METAL2);
		Server_PrecacheModel(PHYSICS_MODEL_METAL0);
		Server_PrecacheModel(PHYSICS_MODEL_METAL1);
		Server_PrecacheModel(PHYSICS_MODEL_METAL2);
		break;
	default:
		g_engine->Con_Warning("area_breakable: Unknown style\n");
	}

	// If we've been given a name, then set our use function.
	if (area->v.name) {
        area->v.use = Area_BreakableUse;
    }

	area->Physics.solid = SOLID_BSP;

	area->v.movetype = MOVETYPE_PUSH;
	area->v.takedamage = true;

	area->local.bleed = false;

	Entity_SetKilledFunction(area, Area_BreakableDie);

	Entity_SetModel(area, area->v.model);
	Entity_SetOrigin(area, area->v.origin);
	Entity_SetSizeVector(area, area->v.mins, area->v.maxs);

	area->v.oldorigin.x = (area->v.mins.x + area->v.maxs.x) * 0.5f;
	area->v.oldorigin.y = (area->v.mins.y + area->v.maxs.y) * 0.5f;
	area->v.oldorigin.z = (area->v.mins.z + area->v.maxs.z) * 0.5f;
}

/*	area_rotate	*/

void Area_RotateBlocked(ServerEntity_t *area, ServerEntity_t *other) {
	Entity_Damage(other, area, area->local.damage, DAMAGE_TYPE_CRUSH);
}

void Area_RotateTouch(ServerEntity_t *area, ServerEntity_t *other) {
	if(!other) {
		return;
	}
}

void Area_RotateThink(ServerEntity_t *eArea) {
	eArea->v.nextthink	= Server.time + 1000000000.0;
}

#define STYLE_ROTATE_DOOR	1

#define	SPAWNFLAG_ROTATE_X			2
#define	SPAWNFLAG_ROTATE_Y			4
#define	SPAWNFLAG_ROTATE_Z			8
#define	SPAWNFLAG_ROTATE_REVERSE	64

void Area_RotateSpawn(ServerEntity_t *area) {
	if(!area->local.speed) {
        area->local.speed = 100;
    }

#if 0
	// [26/7/2012] Check our spawn flags ~hogsy
	if(area->local.style == STYLE_ROTATE_DOOR)
	{
		if(area->v.spawnflags & SPAWNFLAG_ROTATE_REVERSE)
		{
		}

		if(area->v.spawnflags & SPAWNFLAG_ROTATE_X)
			area->v.movedir[0] = 1.0f;
		else if(area->v.spawnflags & SPAWNFLAG_ROTATE_Y)
			area->v.movedir[1] = 1.0f;
		else
			area->v.movedir[2] = 1.0f;

		Math_VectorCopy(area->v.angles,area->local.pos1);
		area->local.pos2[0] = area->local.pos1[0]+area->v.movedir[0]*area->local.distance;

		area->v.TouchFunction = Area_RotateTouch;

		area->local.dMoveFinished = 0;
	}
	else
#endif
	{
		if(area->v.spawnflags & SPAWNFLAG_ROTATE_REVERSE) {
            area->local.speed *= -1;
        }

		if(area->v.spawnflags & SPAWNFLAG_ROTATE_X) {
            area->v.avelocity.x = area->local.speed;
        }

		if(area->v.spawnflags & SPAWNFLAG_ROTATE_Y) {
            area->v.avelocity.y = area->local.speed;
        }

		if(area->v.spawnflags & SPAWNFLAG_ROTATE_Z) {
            area->v.avelocity.z = area->local.speed;
        }
	}

	Entity_SetBlockedFunction(area, Area_RotateBlocked);

	area->v.movetype    = MOVETYPE_PUSH;
	area->v.think       = Area_RotateThink;
	area->v.nextthink  = Server.time + 1000000000.0;	// TODO: This is a hack. A dirty filthy hack. Curse it and its family!

	area->Physics.solid = SOLID_BSP;

	Entity_SetModel(area,area->v.model);
	Entity_SetSizeVector(area,area->v.mins,area->v.maxs);
	Entity_SetOrigin(area,area->v.origin);
}

/*	area_triggerfield	*/

ServerEntity_t *Area_SpawnTriggerField(ServerEntity_t *owner,
                                       PLVector3D mins, PLVector3D maxs,
                                       void (*TriggerFunction)(ServerEntity_t *entity, ServerEntity_t *other)) {
	if ((mins == 0) || (maxs == 0)) {
		g_engine->Con_Warning("Invalid size for trigger field!");
		return NULL;
	}

	ServerEntity_t *field = Entity_Spawn();
	field->v.movetype		= MOVETYPE_NONE;
	field->local.eOwner		= owner;

	Entity_SetTouchFunction(field, TriggerFunction);
	Entity_SetPhysics(field, SOLID_TRIGGER, 0, 0);
	
	// Set the size of it.
	PLVector3D tmins = mins, tmaxs = maxs;
	tmins.x -= 60;	tmins.y -= 60;	tmins.z -= 8;
	tmaxs.x += 60;	tmaxs.y += 60;	tmaxs.z += 8;
	Entity_SetSizeVector(field, tmins, tmaxs);

	return field;
}

/*	area_door	*/

// Spawn flags.
#define	DOOR_FLAG_TRIGGERTOUCH		1	// Triggered by touch.
#define	DOOR_FLAG_TRIGGERUSE		2	// Triggered by use command.
#define	DOOR_FLAG_TRIGGERAUTO		4	// Automatically open door on approach.
#define	DOOR_FLAG_TRIGGERDAMAGE		8	// Triggered by damage.
#define	DOOR_FLAG_LINK				16	// Link the door with others in prox.

// Various states for doors.
#define STATE_TOP			0	// Brush is at the top.
#define STATE_BOTTOM		1	// Brush is at the bottom.
#define STATE_UP			2	// Brush is moving up.
#define STATE_DOWN			3	// Brush is moving down.

void Area_DoorDone(ServerEntity_t *area, ServerEntity_t *other) {
	if (area->local.sound_stop) {
        Sound(area, CHAN_VOICE, area->local.sound_stop, 255, ATTN_NORM);
    }
}

void Area_DoorReturn(ServerEntity_t *area) {
	area->local.flags = STATE_DOWN;

	Area_Move(area, area->local.pos1, area->local.speed, Area_DoorDone);

	if(area->local.cSoundReturn) {
        Sound(area, CHAN_BODY, area->local.cSoundReturn, 255, ATTN_NORM);
    }
	if(area->local.cSoundMoving) {
        Sound(area, CHAN_VOICE, area->local.cSoundMoving, 255, ATTN_NORM);
    }
}

void Area_DoorWait(ServerEntity_t *door, ServerEntity_t *other) {
	UseTargets(door, other);

	door->local.flags = STATE_TOP;

	if (door->local.wait >= 0) {
        door->v.nextthink = Server.time + door->local.wait;
    }

	door->v.think = Area_DoorReturn;

	if (door->local.sound_stop) {
        Sound(door, CHAN_VOICE, door->local.sound_stop, 255, ATTN_NORM);
    }
}

void Area_DoorUse(ServerEntity_t *area) {
	if(area->local.flags == STATE_UP || area->local.flags == STATE_TOP) {
        return;
    }

	area->local.flags = STATE_UP;

	Area_Move(area, area->local.pos2, area->local.speed, Area_DoorWait);

	if(area->local.cSoundStart) {
        Sound(area, CHAN_BODY, area->local.cSoundStart, 255, ATTN_NORM);
    }
	if(area->local.cSoundMoving) {
        Sound(area, CHAN_VOICE, area->local.cSoundMoving, 255, ATTN_NORM);
    }
}

void Area_DoorTouch(ServerEntity_t *door, ServerEntity_t *other) {
	if (door->local.flags == STATE_UP || door->local.flags == STATE_TOP) {
        return;
    }
	if ((other->Monster.type != MONSTER_PLAYER) && other->v.health <= 0) {
        return;
    }

	// Door is linked!
	if (door->v.spawnflags & DOOR_FLAG_LINK)
	{
		ServerEntity_t *linked = door->local.activator;
		if (linked) {
			do {
				Area_DoorUse(linked);
				linked = linked->local.activator;
			} while (linked);
		} else {
            g_engine->Con_Warning("No doors linked! (%i %i %i)\n",
                                  (int) door->v.origin.x,
                                  (int) door->v.origin.y,
                                  (int) door->v.origin.z);
        }
	}

	door->local.flags = STATE_UP;
	Area_Move(door, door->local.pos2, door->local.speed, Area_DoorWait);

	if (door->local.cSoundStart) {
        Sound(door, CHAN_BODY, door->local.cSoundStart, 255, ATTN_NORM);
    }
	if (door->local.cSoundMoving) {
        Sound(door, CHAN_VOICE, door->local.cSoundMoving, 255, ATTN_NORM);
    }
}

void Area_DoorBlocked(ServerEntity_t *door, ServerEntity_t *other) {
	Entity_Damage(other, door, door->local.damage, DAMAGE_TYPE_CRUSH);
}

/*
Links doors together into a list.

This is based on the original QC code;
https://github.com/maikmerten/zernichter/blob/master/id1/src/doors.qc#L316
*/
void Area_DoorLink(ServerEntity_t *door) {
	// Don't link if we've already done so.
	if (door->local.activator) {
        return;
    }

	// TODO: calculate overall scale of door?
	ServerEntity_t *link = g_engine->Server_FindRadius(door->v.origin, 700.0f);
	if (!link) {
        return;
    }

	// We're the master, yo.
	door->local.activator   = door;
    ServerEntity_t *prev    = door;

    // Copy the size over, which we'll use for the trigger field.
	PLVector3D smaxs = door->v.maxs, smins = door->v.mins;
	do {
		// Only link doors which will support it!
		if (!(link->v.spawnflags & DOOR_FLAG_LINK)) {
            // Skip to the next one.
            goto SKIP;
        }

		if (Entity_IsTouching(prev, link)) {
			if (link->local.activator) {
				g_engine->Con_Warning("Door already has activator assigned! (%i %i %i)\n", 
					(int)link->v.origin.x, (int)link->v.origin.y, (int)link->v.origin.z);
				goto SKIP;
			}

			link->local.activator	= prev;
			prev					= link;

			// Update the bounds, which we'll use for the trigger field.
			if (link->v.mins.x < smins.x) {
                smins.x = link->v.mins.x;
            }
			if (link->v.mins.y < smins.y) {
                smins.y = link->v.mins.y;
            }
			if (link->v.mins.z < smins.z) {
                smins.z = link->v.mins.z;
            }

			if (link->v.maxs.x > smaxs.x) {
                smaxs.x = link->v.maxs.x;
            }
			if (link->v.maxs.y > smaxs.y) {
                smaxs.y = link->v.maxs.y;
            }
			if (link->v.maxs.z > smaxs.z) {
                smaxs.z = link->v.maxs.z;
            }
		}

SKIP:
		link = link->v.chain;
	} while (link);

	door->local.trigger_field = Area_SpawnTriggerField(door, smins, smaxs, Area_DoorTouch);
}

void Area_DoorSpawn(ServerEntity_t *door)
{
	PLVector3D	movedir;

	if (door->local.cSoundStart) {
        Server_PrecacheSound(door->local.cSoundStart);
    }
	if (door->local.sound_stop) {
        Server_PrecacheSound(door->local.sound_stop);
    }
	if (door->local.cSoundMoving) {
        Server_PrecacheSound(door->local.cSoundMoving);
    }
	if (door->local.cSoundReturn) {
        Server_PrecacheSound(door->local.cSoundReturn);
    }

	door->v.movetype = MOVETYPE_PUSH;

	Entity_SetPhysics(door, SOLID_BSP, 1.0f, 1.0f);
	Entity_SetModel(door, door->v.model);
	Entity_SetOrigin(door, door->v.origin);
	Entity_SetSizeVector(door, door->v.mins, door->v.maxs);

	if (door->local.lip == 0) {
        door->local.lip = 4;
    }

    door->local.pos1 = door->v.origin;

	Area_SetMoveDirection(door->v.angles, door->v.movedir);

	movedir.x = std::fabs(door->v.movedir.x);
	movedir.y = std::fabs(door->v.movedir.y);
	movedir.z = std::fabs(door->v.movedir.z);

	float movedist =
		movedir.x * door->v.size.x +
		movedir.y * door->v.size.y +
		movedir.z * door->v.size.z -
		door->local.lip;

#if 0
	Math_VectorMake(door->local.pos1, movedist, door->v.movedir, door->local.pos2);
#else
    door->local.pos2 = door->local.pos1 + (door->v.movedir * movedist);
#endif

	door->local.flags = STATE_BOTTOM;

	// Set the spawn flags up.
	if (door->v.spawnflags & DOOR_FLAG_TRIGGERUSE) {
        door->v.use = Area_DoorUse;
    }
	if (door->v.spawnflags & DOOR_FLAG_TRIGGERTOUCH) {
        door->v.TouchFunction = Area_DoorTouch;
    }
	if (door->v.spawnflags & DOOR_FLAG_TRIGGERAUTO) {

	}
	if (door->v.spawnflags & DOOR_FLAG_TRIGGERDAMAGE) {
		// Give it at least one HP.
		if (!door->v.health) {
            door->v.health = 1;
        }

		door->v.takedamage = true;
		// TODO: add handler for this!
//		Entity_SetDamagedFunction(door, Area_DoorTouch);
	}

	if (door->local.damage) {
        Entity_SetBlockedFunction(door, Area_DoorBlocked);
    }
}

/*	area_changelevel	*/

void Area_ChangeLevelTouch(ServerEntity_t *area, ServerEntity_t *other) {
	// [2/1/2013] TODO: If coop wait for other players? ~hogsy

	if(!Entity_IsPlayer(other)) {
        return;
    }

#if 0
	// [2/1/2013] Because we don't want to trigger it multiple times within the delay!!! ~hogsy
	area->v.solid		= SOLID_NOT;
	area->v.think		= Area_ChangelevelStart;
	area->v.nextthink	= Server.time+area->local.delay;
#else
	area->Physics.solid = SOLID_NOT;

	// [2/1/2013] Change the level! ~hogsy
	g_engine->Server_ChangeLevel(area->v.targetname);
#endif
}

void Area_ChangeLevel(ServerEntity_t *area) {
	if(!area->v.targetname) {
		g_engine->Con_Warning("No targetname set for area_changelevel! (%i %i %i)\n",
			(int)area->v.origin.x,
			(int)area->v.origin.y,
			(int)area->v.origin.z);
		return;
	}

	area->v.TouchFunction	= Area_ChangeLevelTouch;

	area->Physics.solid = SOLID_TRIGGER;

	Entity_SetModel(area,area->v.model);
	Entity_SetOrigin(area,area->v.origin);

	area->v.model = 0;
}

/*	area_trigger	*/

void Area_TriggerTouch(ServerEntity_t *area, ServerEntity_t *other) {
	if(area->v.nextthink || ((area->Monster.type != MONSTER_PLAYER) && other->v.health <= 0)) {
        return;
    }

	area->v.nextthink = Server.time + area->local.wait;

	UseTargets(area,other);

	if(area->local.wait < 0) {
        Entity_Remove(area);
    }
}

void Area_TriggerSpawn(ServerEntity_t *area) {
	if(!area->v.targetname) {
		g_engine->Con_Warning("'targetname' not set for trigger! (%i %i %i)\n",
			(int)area->v.origin.x,
			(int)area->v.origin.y,
			(int)area->v.origin.z);
		return;
	}

	area->v.TouchFunction = Area_TriggerTouch;

	area->Physics.solid = SOLID_TRIGGER;

	Entity_SetModel(area,area->v.model);
	Entity_SetOrigin(area,area->v.origin);

	area->v.model = 0;
}

/*	area_pushable	*/

bool _Area_IsOnTop(ServerEntity_t *top, ServerEntity_t *bottom) {
	return !(
            (top->v.absmin.z < bottom->v.absmax.z - 3) ||
            (top->v.absmin.z > bottom->v.absmax.z + 2) ||
            (top->v.absmin.y > bottom->v.absmax.y) ||
            (top->v.absmax.y < bottom->v.absmin.y) ||
            (top->v.absmin.x > bottom->v.absmax.x) ||
            (top->v.absmax.x < bottom->v.absmin.x)
    );
}

void Area_PushableThink(ServerEntity_t *area) {
	// TODO: sort the physics stuff out engine side perhaps? BSP physics are hard.
	// only update when something happens ~eukara
	if(area->v.velocity != 0) {
        area->v.velocity *= 0.9f;
		area->v.velocity.z = -300;	// fake physics... FIXME
		area->v.nextthink = area->v.ltime + 0.5f;
	}
}

void Area_PushableTouch(ServerEntity_t *area, ServerEntity_t *other) {
	if (_Area_IsOnTop(other, area)) {
		// So the player can jump off the object.
        Entity_AddFlags(other, FL_ONGROUND);
		return;
	}
	
	if (area->v.flags & FL_ONGROUND) {
        Entity_RemoveFlags(area, FL_ONGROUND);
    }
	
	// Get the right player angle.
	float yaw = other->v.velocity.y * ((float)PL_PI) * 2 / 360;

    area->v.velocity.x = std::cos(yaw) * 80.0f;
    area->v.velocity.y = std::sin(yaw) * 80.0f;
	// Don't affect the height when pushing... ever.
    area->v.velocity.z = 0;

    area->v.avelocity = area->v.velocity;

	area->v.nextthink = area->v.ltime + 0.5f;
}

void Area_PushableSpawn(ServerEntity_t *area) {
	// TODO: If designed to be breakable, make breakable? ~eukara
/*	if (area->v.health) {
		Area_BreakableSpawn(area);
	}
*/
	//area->Physics.fGravity = cv_server_gravity.value;
    area->v.angles = 0;

	area->Physics.solid = SOLID_SLIDEBOX;
	area->v.movetype = MOVETYPE_STEP;
	Entity_SetModel(area,area->v.model);
	Entity_SetOrigin(area,area->v.origin);
	Entity_SetSizeVector(area,area->v.mins,area->v.maxs);

	area->v.TouchFunction = Area_PushableTouch;
	area->v.think = Area_PushableThink;
}

/*	area_wall	*/

void Area_WallUse(ServerEntity_t *area) {
	if(area->Physics.solid == SOLID_BSP) {
		area->local.oldmodel	= area->v.model;
		area->local.value		= 0;

		area->Physics.solid = SOLID_NOT;

		area->v.model = 0;
		return;
	}

	area->Physics.solid = SOLID_BSP;
	area->local.value = 1;
	Entity_SetModel(area,area->local.oldmodel);
}

void Area_WallSpawn(ServerEntity_t *area) {
	if(!area->v.model) {
		g_engine->Con_Warning("Area entity with no model!\n");

		Entity_Remove(area);
		return;
	}

	if(area->v.name) {
        area->v.use = Area_WallUse;
    }

	area->v.movetype = MOVETYPE_PUSH;

	area->local.value = 1;

	Entity_SetModel(area,area->v.model);
	Entity_SetOrigin(area,area->v.origin);
}

/*	area_detail	*/

// Just for the compiler ~eukara
void Area_DetailSpawn(ServerEntity_t *area) {
	if(!area->v.model) {
		g_engine->Con_Warning("Area entity with no model!\n");

		Entity_Remove(area);
		return;
	}

	Entity_SetModel(area,area->v.model);
	Entity_SetOrigin(area,area->v.origin);
}

/*	area_button	*/

void Area_ButtonDone(ServerEntity_t *eArea, ServerEntity_t *other) {
	eArea->local.flags = STATE_DOWN;
	eArea->local.value = 0;

	if (eArea->local.sound_stop) {
        Sound(eArea, CHAN_VOICE, eArea->local.sound_stop, 255, ATTN_NORM);
    }
}

void Area_ButtonReturn(ServerEntity_t *eArea)
{
	Area_Move(eArea, eArea->local.pos1, eArea->local.speed, Area_ButtonDone);

	if(eArea->local.cSoundReturn)
		Sound(eArea,CHAN_BODY,eArea->local.cSoundReturn,255,ATTN_NORM);
	if(eArea->local.cSoundMoving)
		Sound(eArea,CHAN_VOICE,eArea->local.cSoundMoving,255,ATTN_NORM);
}

void Area_ButtonWait(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	UseTargets(eArea, eOther);

	eArea->local.flags = STATE_TOP;
	eArea->local.value = 1;

	eArea->v.think		= Area_ButtonReturn;
	eArea->v.nextthink	= eArea->v.ltime + 4;

	if (eArea->local.sound_stop)
		Sound(eArea, CHAN_VOICE, eArea->local.sound_stop, 255, ATTN_NORM);
}

void Area_ButtonTouch(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	if(eArea->local.flags == STATE_UP || eArea->local.flags == STATE_TOP)
		return;
	if((eOther->Monster.type != MONSTER_PLAYER) && eOther->v.health <= 0)
		return;

	eArea->local.flags = STATE_UP;

	Area_Move(eArea, eArea->local.pos2, eArea->local.speed, Area_ButtonWait);

	if(eArea->local.cSoundStart)
		Sound(eArea,CHAN_BODY,eArea->local.cSoundStart,255,ATTN_NORM);
	if(eArea->local.cSoundMoving)
		Sound(eArea,CHAN_VOICE,eArea->local.cSoundMoving,255,ATTN_NORM);
}

void Area_ButtonUse(ServerEntity_t *area) {
	if(area->local.flags == STATE_UP || area->local.flags == STATE_TOP) {
        return;
    }

	area->local.flags = STATE_UP;

	Area_Move(area, area->local.pos2, area->local.speed, Area_ButtonWait);

	if(area->local.cSoundStart) {
        Sound(area, CHAN_BODY, area->local.cSoundStart, 255, ATTN_NORM);
    }
	if(area->local.cSoundMoving) {
        Sound(area, CHAN_VOICE, area->local.cSoundMoving, 255, ATTN_NORM);
    }
}

void Area_ButtonBlocked(ServerEntity_t *eArea, ServerEntity_t *eOther)
{
	Entity_Damage(eOther, eArea, eArea->local.damage, DAMAGE_TYPE_CRUSH);
}

void Area_ButtonSpawn(ServerEntity_t *eArea)
{
	float	fDist;
	PLVector3D vMoveDir;

	if(!eArea->v.spawnflags)
		eArea->v.spawnflags = 0;

	if(eArea->local.cSoundStart)
		Server_PrecacheSound(eArea->local.cSoundStart);
	if (eArea->local.sound_stop)
		Server_PrecacheSound(eArea->local.sound_stop);
	if(eArea->local.cSoundMoving)
		Server_PrecacheSound(eArea->local.cSoundMoving);
	if(eArea->local.cSoundReturn)
		Server_PrecacheSound(eArea->local.cSoundReturn);

	eArea->v.movetype = MOVETYPE_PUSH;

	eArea->Physics.solid = SOLID_BSP;

	// [18/5/2013] Changed to use ! check instead since it's safer here ~hogsy
	if(eArea->local.lip == 0) {
        eArea->local.lip = 4;
    }

	eArea->local.value = 0;
	eArea->local.flags = STATE_BOTTOM;

	Entity_SetModel(eArea,eArea->v.model);
	Entity_SetOrigin(eArea,eArea->v.origin);
	Entity_SetSizeVector(eArea,eArea->v.mins,eArea->v.maxs);

    eArea->local.pos1 = eArea->v.origin;

	Area_SetMoveDirection(eArea->v.angles,eArea->v.movedir);

    vMoveDir.x = std::fabs(eArea->v.movedir.x);
    vMoveDir.y = std::fabs(eArea->v.movedir.y);
    vMoveDir.z = std::fabs(eArea->v.movedir.z);

	fDist = vMoveDir.x*eArea->v.size.x+
			vMoveDir.y*eArea->v.size.y+
			vMoveDir.z*eArea->v.size.z-
			eArea->local.lip;

	Math_VectorMake(eArea->local.pos1,fDist,eArea->v.movedir,&eArea->local.pos2);

	if(eArea->v.spawnflags != 32) { // Toggle
        eArea->v.TouchFunction = Area_ButtonTouch;
    }

	if (eArea->local.damage) {
        Entity_SetBlockedFunction(eArea, Area_ButtonBlocked);
    }

	eArea->v.use = Area_ButtonUse;
}

/*	area_platform	*/

void Area_PlatformDone(ServerEntity_t *area, ServerEntity_t *other) {
	if(area->local.wait >= 0) {
        area->v.nextthink = area->v.ltime + area->local.wait;
    }

	area->local.flags = STATE_DOWN;
	area->local.value = 0;
	area->v.think	= NULL;

	if (area->local.sound_stop) {
        Sound(area, CHAN_VOICE, area->local.sound_stop, 255, ATTN_NORM);
    }
}

void Area_PlatformReturn(ServerEntity_t *area) {
	Area_Move(area, area->local.pos1, area->local.speed, Area_PlatformDone);

	if(area->local.cSoundReturn) {
        Sound(area, CHAN_BODY, area->local.cSoundReturn, 255, ATTN_NORM);
    }
	if(area->local.cSoundMoving) {
        Sound(area, CHAN_VOICE, area->local.cSoundMoving, 255, ATTN_NORM);
    }
}

void Area_PlatformWait(ServerEntity_t *area, ServerEntity_t *other) {
	UseTargets(area, other);

	area->local.flags = STATE_TOP;
	area->local.value = 1;

	area->v.think = Area_PlatformReturn;

	if(area->local.wait >= 0) {
        area->v.nextthink = area->v.ltime + area->local.wait;
    }

	if (area->local.sound_stop) {
        Sound(area, CHAN_VOICE, area->local.sound_stop, 255, ATTN_NORM);
    }
}

void Area_PlatformTouch(ServerEntity_t *eArea, ServerEntity_t *eOther) {
	if(eArea->local.flags == STATE_UP || eArea->local.flags == STATE_TOP)
		return;
	if((eOther->Monster.type != MONSTER_PLAYER) && eOther->v.health <= 0)
		return;
	if(eArea->v.nextthink > eArea->v.ltime)
		return;

	eArea->local.flags = STATE_UP;

	Area_Move(eArea, eArea->local.pos2, eArea->local.speed, Area_PlatformWait);

	if(eArea->local.cSoundStart)
		Sound(eArea,CHAN_BODY,eArea->local.cSoundStart,255,ATTN_NORM);
	if(eArea->local.cSoundMoving)
		Sound(eArea,CHAN_VOICE,eArea->local.cSoundMoving,255,ATTN_NORM);
}

void Area_PlatformUse(ServerEntity_t *eArea)
{
	if(eArea->local.flags == STATE_UP || eArea->local.flags == STATE_TOP) {
        return;
    }

	eArea->local.flags = STATE_UP;

	Area_Move(eArea,eArea->local.pos2,eArea->local.speed,Area_PlatformWait);

	if(eArea->local.cSoundStart) {
        Sound(eArea, CHAN_BODY, eArea->local.cSoundStart, 255, ATTN_NORM);
    }
	if(eArea->local.cSoundMoving) {
        Sound(eArea, CHAN_VOICE, eArea->local.cSoundMoving, 255, ATTN_NORM);
    }
}

void Area_PlatformBlocked(ServerEntity_t *area, ServerEntity_t *other) {
	Entity_Damage(other, area, area->local.damage, DAMAGE_TYPE_CRUSH);
}

void Area_PlatformSpawn(ServerEntity_t *area) {
	if(!area->v.spawnflags) {
        area->v.spawnflags = 0;
    }

	if(area->local.cSoundStart) {
        Server_PrecacheSound(area->local.cSoundStart);
    }
	if (area->local.sound_stop) {
        Server_PrecacheSound(area->local.sound_stop);
    }
	if(area->local.cSoundMoving) {
        Server_PrecacheSound(area->local.cSoundMoving);
    }
	if(area->local.cSoundReturn) {
        Server_PrecacheSound(area->local.cSoundReturn);
    }

	area->v.movetype = MOVETYPE_PUSH;

	area->Physics.solid = SOLID_BSP;

	if(area->local.count == 0) {
        area->local.count = 100;
    }
	if(area->local.wait == 0) {
        area->local.wait = 3;
    }
	if(area->local.damage == 0) {
        area->local.damage = 20;
    }

	area->local.value = 0;
	area->local.flags = STATE_BOTTOM;

	Entity_SetModel(area,area->v.model);
	Entity_SetOrigin(area,area->v.origin);
	Entity_SetSizeVector(area,area->v.mins,area->v.maxs);

    area->local.pos1 = area->v.origin;
	Area_SetMoveDirection(area->v.angles, area->v.movedir);

	float dist = (float)area->local.count;

	Math_VectorMake(area->local.pos1, dist, area->v.movedir, &area->local.pos2);

	if(area->v.spawnflags != 32) { // Toggle
        area->v.TouchFunction = Area_PlatformTouch;
    }

	if (area->local.damage) {
        Entity_SetBlockedFunction(area, Area_PlatformBlocked);
    }

	area->v.use = Area_PlatformUse;
}

/*	area_climb	*/

void Area_ClimbTouch(ServerEntity_t *area, ServerEntity_t *other) {
	PLVector3D ladder_velocity, vPlayerVec;

	if ((other->local.ladderjump > Server.time) || (other->v.waterlevel > 1) || (other->v.flags & FL_WATERJUMP)) {
        return;
    }

    PLVector3D forward, right, up;
    Math_AngleVectors(other->v.angles, &forward, &right, &up);

    PLVector3D playervec = forward;
    playervec *= 250;

	if (other->v.button[2]) {
        playervec = other->v.velocity;
    }

	// ignore 8 units of the top edge
	if (other->v.origin.z + other->v.mins.z + 8 >= area->v.absmax.z) {
		if (!(other->v.flags & FL_ONGROUND)) {
            other->v.flags = other->v.flags + FL_ONGROUND;
        }
		return;
	}

	// null out gravity in PreThink
	other->local.laddertime = Server.time + 0.1;
	other->local.zerogtime = Server.time + 0.1;
	other->v.velocity.z = 0;

	if (other->v.velocity.DotProduct(right) > 25) {
        other->v.velocity = 0;

		other->v.origin.x += right.x * 0.5f;
		other->v.origin.y += right.y * 0.5f;
		other->v.origin.z += right.z * 0.5f;
	//	printf("right  ");
		return;
	} else if (other->v.velocity.DotProduct(right) < -25) {
        other->v.velocity = 0;

		other->v.origin.x -= right.x * 0.5f;
		other->v.origin.y -= right.y * 0.5f;
		other->v.origin.z -= right.z * 0.5f;
	//	printf("left  ");
		return;
	}

	float forwardspeed = forward.DotProduct(other->v.velocity);
	ladder_velocity = 0;

	if ((other->v.v_angle.x <= 15) && (forwardspeed > 0)) { // up (facing up/forward)
		//other->v.origin[0] -= area->v.movedir[0] * 0.36f;
		//other->v.origin[1] -= area->v.movedir[1] * 0.36f;
		other->v.origin.z -= area->v.movedir.z * 0.36f;
		ladder_velocity.z = other->v.v_angle.x * 6; // go faster when facing forward
		//printf("up (facing up/forward)  ");

		if (ladder_velocity.z < 90) {
            ladder_velocity.z = 90; // minimum speed
        }
	} else if ((other->v.v_angle.x >= 15) && (forwardspeed < 0)) { // up (facing down)
		//other->v.origin[0] += area->v.movedir[0] * 0.36f;
		//other->v.origin[1] += area->v.movedir[1] * 0.36f;
		other->v.origin.z += area->v.movedir.z * 0.36f;
		//printf("up (facing down)  ");

		ladder_velocity.z = other->v.v_angle.x * 4;
	} else if ((other->v.v_angle.x <= 15) && (forwardspeed < 0)) { // down (facing up/forward)
		//other->v.origin[0] += area->v.movedir[0] * 0.36f;
		//other->v.origin[1] += area->v.movedir[1] * 0.36f;
		other->v.origin.z += area->v.movedir.z * 0.36f;

		ladder_velocity.z = other->v.v_angle.x * -5;// go faster when facing forward
		//printf("down (facing up/forward)  ");

		if (ladder_velocity.z > -80) {
            ladder_velocity.z = -80; // minimum speed
        }
	} else if ((other->v.v_angle.x >= 15) && (forwardspeed > 0)) { // down (facing down)
		other->v.origin.x -= area->v.movedir.x * 0.36f;
		other->v.origin.y -= area->v.movedir.y * 0.36f;
		other->v.origin.z -= area->v.movedir.z * 0.36f;
		//printf("down (facing down)  ");

		ladder_velocity.z = other->v.v_angle.x * -4;
	}

	//printf("angle: %i; velo: %i\n", (int)other->v.v_angle[0], (int)ladder_velocity[2]);

	if (ladder_velocity.z > 100) {
        ladder_velocity.z = 100;
    } else if (ladder_velocity.z < -1 * 100) {
        ladder_velocity.z = -1 * 100;
    }

	other->v.velocity = ladder_velocity;
}

void Area_ClimbSpawn(ServerEntity_t *area) {
	Area_SetMoveDirection(area->v.angles, area->v.movedir);
	area->v.TouchFunction = Area_ClimbTouch;

	area->Physics.solid = SOLID_TRIGGER;

	Entity_SetModel(area,area->v.model);
	Entity_SetOrigin(area,area->v.origin);
	area->v.model = 0;
}

/*	area_noclip	*/

void Area_NoclipSpawn(ServerEntity_t *area) {
	area->v.movetype = MOVETYPE_PUSH;
	area->Physics.solid	= SOLID_NOT;

	Entity_SetModel(area, area->v.model);
	Entity_SetOrigin(area, area->v.origin);
}

/*	area_push	*/

#define AREA_PUSH_ONCE 32

void Area_PushTouch(ServerEntity_t *area, ServerEntity_t *other) {
	// [9/12/2013] TODO: Make this optional? Would be cool to throw monsters and other crap around... ~hogsy
	if(!Entity_IsPlayer(other)) {
        return;
    }

    other->v.velocity = area->v.movedir * (area->local.speed * 10);

	if(area->v.spawnflags & AREA_PUSH_ONCE) {
        Entity_Remove(area);
    }
}

void Area_PushSpawn(ServerEntity_t *area) {
	if(!area->local.speed)
		area->local.speed = 500.0f;

	Area_SetMoveDirection(area->v.angles, area->v.movedir);

	area->v.TouchFunction = Area_PushTouch;
	area->Physics.solid	= SOLID_TRIGGER;

	Entity_SetModel(area,area->v.model);
	Entity_SetOrigin(area,area->v.origin);
	Entity_SetSizeVector(area,area->v.mins,area->v.maxs);

	area->v.model = 0;
}

/*
	area_kill
	Kills anything that enters the area.
*/

void Area_KillTouch(ServerEntity_t *area, ServerEntity_t *other) {
	// Damage it, until it gibs and gets removed.
	Entity_Damage(other, area, other->v.health, DAMAGE_TYPE_NORMAL);
}

void Area_KillSpawn(ServerEntity_t *area) {
	area->Physics.solid = SOLID_TRIGGER;

	Entity_SetModel(area, area->v.model);
	Entity_SetOrigin(area, area->v.origin);
	Entity_SetTouchFunction(area, Area_KillTouch);

	area->v.model = 0;
}
