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

#include "server_main.h"

// TODO: Rename funcs to ServerEntity_*! This is in consideration of CL implementation.

/*	Entity Spawning	*/

char *Entity_AllocateString(char *string)
{
	char	*newstring, *new_p;
	int		i, l;

	l = strlen(string) + 1;
	newstring = (char*)Engine.Hunk_Alloc(l);
	new_p = newstring;

	for (i = 0; i< l; i++)
	{
		if (string[i] == '\\' && i < l - 1)
		{
			i++;
			if (string[i] == 'n')
				*new_p++ = '\n';
			else
				*new_p++ = '\\';
		}
		else
			*new_p++ = string[i];
	}

	return newstring;
}

typedef struct
{
	const char *varname;

	int	offset;

	DataType_t	datatype;

	int	flags;
} ServerEntityField_t;

ServerEntityField_t	entity_fields[] =
{
	{ "classname", SERVER_ENTITY_FIELD(v.classname), DATA_STRING },
	{ "name", SERVER_ENTITY_FIELD(v.name), DATA_STRING },
	{ "model", SERVER_ENTITY_FIELD(v.model), DATA_STRING },
	{ "targetname", SERVER_ENTITY_FIELD(v.targetname), DATA_STRING },
	{ "noise", SERVER_ENTITY_FIELD(v.noise), DATA_STRING },
	{ "message", SERVER_ENTITY_FIELD(v.message), DATA_STRING },
	{ "origin", SERVER_ENTITY_FIELD(v.origin), DATA_VECTOR3 },
	{ "angles", SERVER_ENTITY_FIELD(v.angles), DATA_VECTOR3 },
	{ "light", SERVER_ENTITY_FIELD(v.vLight), DATA_VECTOR4 },
	{ "health", SERVER_ENTITY_FIELD(v.health), DATA_INTEGER },
	{ "spawnflags", SERVER_ENTITY_FIELD(v.spawnflags), DATA_INTEGER },
	{ "takedamage", SERVER_ENTITY_FIELD(v.takedamage), DATA_BOOLEAN },
	{ "alpha", SERVER_ENTITY_FIELD(alpha), DATA_INTEGER },
	{ "frame", SERVER_ENTITY_FIELD(v.frame), DATA_INTEGER },

	// Model properties
	{ "model_skin", SERVER_ENTITY_FIELD(Model.skin), DATA_INTEGER },
	{ "scale", SERVER_ENTITY_FIELD(Model.scale), DATA_FLOAT },

	// Physical properties
	{ "physics_solid", SERVER_ENTITY_FIELD(Physics.solid), DATA_INTEGER },
	{ "physics_mass", SERVER_ENTITY_FIELD(Physics.mass), DATA_FLOAT },
	{ "physics_gravity", SERVER_ENTITY_FIELD(Physics.gravity), DATA_FLOAT },

	// Local (move these at some point)
	{ "sound", SERVER_ENTITY_FIELD(local.sound), DATA_STRING },
	{ "soundstart", SERVER_ENTITY_FIELD(local.cSoundStart), DATA_STRING },
	{ "soundstop", SERVER_ENTITY_FIELD(local.sound_stop), DATA_STRING },
	{ "soundmoving", SERVER_ENTITY_FIELD(local.cSoundMoving), DATA_STRING },
	{ "soundreturn", SERVER_ENTITY_FIELD(local.cSoundReturn), DATA_STRING },
	{ "target1", SERVER_ENTITY_FIELD(local.cTarget1), DATA_STRING },
	{ "target2", SERVER_ENTITY_FIELD(local.cTarget2), DATA_STRING },
	{ "speed", SERVER_ENTITY_FIELD(local.speed), DATA_FLOAT },
	{ "delay", SERVER_ENTITY_FIELD(local.delay), DATA_FLOAT },
	{ "lip", SERVER_ENTITY_FIELD(local.lip), DATA_FLOAT },
	{ "wait", SERVER_ENTITY_FIELD(local.wait), DATA_DOUBLE },
	{ "damage", SERVER_ENTITY_FIELD(local.damage), DATA_INTEGER },
	{ "volume", SERVER_ENTITY_FIELD(local.volume), DATA_INTEGER },
	{ "style", SERVER_ENTITY_FIELD(local.style), DATA_INTEGER },
	{ "count", SERVER_ENTITY_FIELD(local.count), DATA_INTEGER },
	{ "team", SERVER_ENTITY_FIELD(local.team), DATA_INTEGER },
	{ "attack_finished", SERVER_ENTITY_FIELD(local.dAttackFinished), DATA_DOUBLE },

	// hacks
	{ "angle", SERVER_ENTITY_FIELD(v.angles), DATA_VECTOR3, FL_ANGLEHACK },

	// Ignore these global fields.
	{ "wad", 0, EV_NONE },
	{ "ambient", 0, EV_NONE },
	{ "sky", 0, EV_NONE },
	{ "cloud", 0, EV_NONE },
	{ "scrollspeed", 0, EV_NONE },
	{ "mapversion", 0, EV_NONE },

	{ 0 }
};

/*	Automatically called by the engine during entity parsing.
*/
void ServerEntity_ParseField(char *key, char *value, ServerEntity_t *entity)
{
	for (ServerEntityField_t *field = entity_fields; field->varname; field++)
	{
		if (!strncmp(field->varname, key, sizeof(field->varname)))
		{
			switch (field->datatype)
			{
			case DATA_STRING:
				*(char**)((uint8_t*)entity + field->offset) = Entity_AllocateString(value);
				break;
			case DATA_VECTOR3:
				if (field->flags & FL_ANGLEHACK)
				{
					((float*)((uint8_t*)entity + field->offset))[0] = 0;
					((float*)((uint8_t*)entity + field->offset))[1] = strtof(value, NULL);
					((float*)((uint8_t*)entity + field->offset))[2] = 0;
				}
				else
				{
					PLVector3D vector = { 0, 0, 0 };
					if (sscanf(value, "%f %f %f", &vector.x, &vector.y, &vector.z) < 3)
						Engine.Con_Warning("Field did not return expected number of arguments! (%s)\n", value);
					((float*)((uint8_t*)entity + field->offset))[0] = vector.x;
					((float*)((uint8_t*)entity + field->offset))[1] = vector.y;
					((float*)((uint8_t*)entity + field->offset))[2] = vector.z;
				}
				break;
			case DATA_VECTOR4:
			{
				float vector[4] = { 0, 0, 0 };
				if (sscanf(value, "%f %f %f %f", &vector[0], &vector[1], &vector[2], &vector[3]) < 4)
					Engine.Con_Warning("Field did not return expected number of arguments! (%s)\n", value);
				((float*)((uint8_t*)entity + field->offset))[0] = vector[0];
				((float*)((uint8_t*)entity + field->offset))[1] = vector[1];
				((float*)((uint8_t*)entity + field->offset))[2] = vector[2];
				((float*)((uint8_t*)entity + field->offset))[3] = vector[3];
			}
			break;
			case DATA_FLOAT:
				*(float*)((uint8_t*)entity + field->offset) = strtof(value, NULL);
				break;
			case DATA_DOUBLE:
				*(double*)((uint8_t*)entity + field->offset) = strtod(value, NULL);
				break;
			case DATA_BOOLEAN:
				if (!strcmp(value, "true"))
					value = "1";
				else if (!strcmp(value, "false"))
					value = "0";
				// Booleans are handled in the same way as integers, so don't break here!
			case DATA_INTEGER:
				*(int*)((uint8_t*)entity + field->offset) = atoi(value);
				break;
			case EV_NONE:
				// Just ignore anything that has none set.
				break;
			default:
				Engine.Con_Warning("Unknown entity field type! (%i)\n", field->datatype);
			}
			return;
		}
	}

	Engine.Con_Warning("Invalid field! (%s)\n", key);
}

/*
	General Entity Management
*/

/*	Create a new entity instance.
*/
ServerEntity_t *Entity_Spawn(void)
{
	ServerEntity_t *eSpawn = Engine.Spawn();

	// Set physics properties to their defaults!
	eSpawn->Physics.mass		= 1.0f;
	eSpawn->Physics.friction	= 1.0f;
	eSpawn->Physics.gravity		= SERVER_GRAVITY;

	return eSpawn;
}

/*	Alternative to SetOrigin.
	This is the only valid way to
	move an object without using
	the physics of the world
	(setting velocity and waiting).
	Directly changing origin will
	not set internal links correctly,
	so clipping would be messed up.
	This should be called when an
	object is spawned, and then only
	if it is teleported.
*/
void Entity_SetOrigin(ServerEntity_t *eEntity, PLVector3D vOrigin) {
	eEntity->v.origin = vOrigin;

	Entity_Link(eEntity, false);
}

/*	Sets the angle of the given entity.
*/
void Entity_SetAngles(ServerEntity_t *eEntity, PLVector3D vAngles)
{
    eEntity->v.angles = vAngles;

	// TODO: Link?
}

/*	Sets the model of the given entity.
*/
void Entity_SetModel(ServerEntity_t *eEntity, const char *modelpath)
{
	if (!modelpath)
	{
		Engine.Con_Warning("Invalid path for model! (%s)", eEntity->v.classname);
		return;
	}

	Engine.SetModel(eEntity,modelpath);
}

/*	Sets the size of the given entity; requires that the model has been applied first.
	Alternative to SetSize.
*/
void Entity_SetSizeVector(ServerEntity_t *eEntity, PLVector3D vMin, PLVector3D vMax)
{
	// Check if the model is set yet, if not give us a little warning.
	if(!eEntity->v.model)
		Engine.Con_Warning("Setting entity size before model! (%s)\n",eEntity->v.classname);

    if(vMin.x > vMax.x || vMin.y > vMax.y || vMin.z > vMax.z) {
        Engine.Con_Warning("Backwards mins/maxs! (%s)\n",eEntity->v.classname);
        return;
    }

    eEntity->v.mins = vMin;
    eEntity->v.maxs = vMax;

    plSubtractVector3D(&vMin, vMax);
    eEntity->v.size = vMin;

	Entity_Link(eEntity, false);
}

/*	Set the size of the bounding box for the entity.
	Should be called AFTER setting model!
*/
void Entity_SetSize(ServerEntity_t *eEntity,
	float fMinA,float fMinB,float fMinC,
	float fMaxA,float fMaxB,float fMaxC)
{
	PLVector3D	vMin,vMax;
	vMin.x = fMinA; vMin.y = fMinB; vMin.z = fMinC;
	vMax.x = fMaxA; vMax.y = fMaxB; vMax.z = fMaxC;

	Entity_SetSizeVector(eEntity,vMin,vMax);
}

/*	Can be used for convenience.
*/
void Entity_AddEffects(ServerEntity_t *eEntity,int iEffects)
{
	if (eEntity->v.effects & iEffects) {
        return;
    }
	eEntity->v.effects |= iEffects;
}

/*	Can be used for convenience.
*/
void Entity_RemoveEffects(ServerEntity_t *eEntity,int iEffects)
{
	if (!(eEntity->v.effects & iEffects)) {
        return;
    }
	eEntity->v.effects &= ~iEffects;
}

/*	Can be used for convenience.
*/
void Entity_ClearEffects(ServerEntity_t *eEntity)
{
	eEntity->v.effects = 0;
}

/*	Can be used for convenience.
*/
void Entity_AddFlags(ServerEntity_t *eEntity, int iFlags)
{
	if (eEntity->v.flags & iFlags)
		return;
	eEntity->v.flags |= iFlags;
}

/*	Can be used for convenience.
*/
void Entity_RemoveFlags(ServerEntity_t *eEntity, int iFlags)
{
	if (!(eEntity->v.flags & iFlags))
		return;
	eEntity->v.flags &= ~iFlags;
}

/*	Can be used for convenience.
*/
void Entity_ClearFlags(ServerEntity_t *eEntity) {
	eEntity->v.flags = 0;
}

/*	Simple function for checking if an entity can be damaged or not.
*/
bool Entity_CanDamage(ServerEntity_t *eEntity, ServerEntity_t *eTarget, EntityDamageType_t iDamageType)
{
	// Can't damage people on the same team.
	if(eEntity->local.team && (eEntity->local.team == eTarget->local.team))
		return false;

	if(eTarget->v.takedamage && (!eTarget->local.iDamageType || (eTarget->local.iDamageType == iDamageType)))
		return true;

	return false;
}

void Entity_Damage(ServerEntity_t *entity, ServerEntity_t *seInflictor, int iDamage, EntityDamageType_t type)
{
	// Don't bother if there's no actual damage inflicted.
	if (iDamage <= 0)
		return;

	// Only continue if we can damage the entity.
	if (!Entity_CanDamage(seInflictor, entity, type))
		return;

	// If it's a monster or player, hand it over to the monster code.
	if (Entity_IsMonster(entity) || Entity_IsPlayer(entity))
	{
		Monster_Damage(entity, seInflictor, iDamage, type);
		return;
	}

	// Otherwise we'll do our own thing here...

	entity->v.health -= iDamage;
	if (entity->v.health <= 0)
	{
		if (entity->local.KilledFunction)
			entity->local.KilledFunction(entity, seInflictor, type);
		return;
	}

	if (entity->local.DamagedFunction)
		entity->local.DamagedFunction(entity, seInflictor, type);
}

/*	Damage entities within a specific radius.
*/
void Entity_RadiusDamage(ServerEntity_t *eInflictor, float fRadius, int iDamage, EntityDamageType_t iDamageType)
{
	float	fDistance;
	PLVector3D	vOrigin;
	ServerEntity_t *eTarget = Engine.Server_FindRadius(eInflictor->v.origin, fRadius);

	do
	{
		if(eTarget->v.takedamage)
		{
			vOrigin.x = eTarget->v.origin.x+(eTarget->v.mins.x+eTarget->v.maxs.x)*0.5f;
            vOrigin.y = eTarget->v.origin.y+(eTarget->v.mins.y+eTarget->v.maxs.y)*0.5f;
            vOrigin.z = eTarget->v.origin.z+(eTarget->v.mins.z+eTarget->v.maxs.z)*0.5f;

            plSubtractVector3D(&vOrigin, eInflictor->v.origin);

			fDistance = 0.5f * plVector3DLength(vOrigin);
			if(fDistance > 0)
			{
                plInverseVector3D(&vOrigin);
                plAddVector3D(&eTarget->v.velocity, vOrigin);

				// Reduce the damage by distance.
				fDistance = (float)iDamage-(100.0f/fDistance);

				// Less damage for the inflictor.
				if(eTarget == eInflictor)
					fDistance = fDistance/2.0f;

				if(fDistance > 0)
					Entity_Damage(eTarget,eInflictor,(int)fDistance,iDamageType);
			}
		}

		eTarget = eTarget->v.chain;
	}
	while(eTarget);
}

/*	Link an entity.
*/
void Entity_Link(ServerEntity_t *eEntity,bool bTouchTriggers) {
	Engine.LinkEntity(eEntity, bTouchTriggers);
}

void Entity_Unlink(ServerEntity_t *eEntity) {
	Engine.UnlinkEntity(eEntity);
}

/*	Remove an entity from the world.
*/
void Entity_Remove(ServerEntity_t *eEntity) {
	Engine.FreeEntity(eEntity);
}

/*
	Animation
*/

/*	Checks and updates the entity's current frame.
*/
void Entity_CheckFrames(ServerEntity_t *eEntity)
{
	// If something isn't active and Animationtime is over
	if(!eEntity->local.animation_end || (Server.time < eEntity->local.animation_time))
		return;
	else if(eEntity->local.iAnimationCurrent > eEntity->local.animation_end)
	{
		eEntity->local.animation_time = 0;
		return;
	}

	eEntity->v.frame = eEntity->local.iFrames[eEntity->local.iAnimationCurrent].frame;

	eEntity->local.animation_time = Server.time+((double)eEntity->local.iFrames[eEntity->local.iAnimationCurrent].speed);

	if(eEntity->local.iFrames[eEntity->local.iAnimationCurrent].Event)
		eEntity->local.iFrames[eEntity->local.iAnimationCurrent].Event(eEntity);

	eEntity->local.iAnimationCurrent++;
}

/*	Resets the currently active animation.
*/
void Entity_ResetAnimation(ServerEntity_t *eEntity)
{
	eEntity->v.frame					=
	eEntity->local.iAnimationCurrent	=
	eEntity->local.animation_end		= 0;

	// Reset the animation time.
	eEntity->local.animation_time = 0;
}

/*	Start animating the entity.
*/
void Entity_Animate(ServerEntity_t *eEntity, ServerEntityFrame_t *efAnimation)
{
	int i = 0;

	Entity_ResetAnimation(eEntity);

	eEntity->local.iAnimationCurrent = 0;

	eEntity->v.frame = efAnimation[0].frame;

	for(;;)
	{
		if(efAnimation[i].isend)
		{
			eEntity->local.animation_end = i;
			break;
		}

		i++;
	}

	eEntity->local.animation_time	= Server.time+((double)efAnimation[1].speed);
	eEntity->local.iFrames			= efAnimation;
}

/*	Returns true if an animation is currently playing.
*/
bool Entity_IsAnimating(ServerEntity_t *entity)
{
	if (!entity->local.animation_end || (Server.time < entity->local.animation_time))
		return false;

	return true;
}

/*
	Type Checks
*/

/*	Is the given monster a player?
*/
bool Entity_IsPlayer(ServerEntity_t *eEntity)
{
	if(eEntity->Monster.type == MONSTER_PLAYER)
		return true;

	return false;
}

/*	Is the given monster a monster? (this is dumb...)
*/
bool Entity_IsMonster(ServerEntity_t *eEntity)
{
	if((eEntity->Monster.type != MONSTER_PLAYER) && (eEntity->Monster.type > MONSTER_VEHICLE))
		return true;

	return false;
}

/*
	Physics
*/

/*	Sets up the physical properties for the entity.
*/
void Entity_SetPhysics(ServerEntity_t *seEntity, EntitySolidType_t pstSolidType, float fMass, float fFriction)
{
	seEntity->Physics.solid			= pstSolidType;
	seEntity->Physics.mass			= fMass;
	seEntity->Physics.gravity		= SERVER_GRAVITY;
	seEntity->Physics.friction		= fFriction;
}

/*
	Math/Utility Functions
*/

void Entity_MakeVectors(ServerEntity_t *eEntity)
{
	Math_AngleVectors(eEntity->v.v_angle, &eEntity->local.forward, &eEntity->local.right, &eEntity->local.vUp);
}

bool Entity_DropToFloor(ServerEntity_t *eEntity)
{
	PLVector3D vEnd;
	trace_t	trGround;

    vEnd = eEntity->v.origin;
	vEnd.z -= 256;

	trGround = Engine.Server_Move(eEntity->v.origin, eEntity->v.mins, eEntity->v.maxs, vEnd, false, eEntity);
	if (trGround.fraction == 1 || trGround.all_solid)
	{
		Engine.Con_Warning("Entity is stuck in world! (%s) (%i %i %i)\n", eEntity->v.classname,
			(int)eEntity->v.origin.x,
			(int)eEntity->v.origin.y,
			(int)eEntity->v.origin.z);
		return false;
	}

	// Use SetOrigin so that it's automatically linked.
	Entity_SetOrigin(eEntity, trGround.endpos);

	Entity_AddFlags(eEntity, FL_ONGROUND);

	eEntity->v.groundentity = trGround.ent;

	return true;
}

// todo, this wouldn't entirely be true using intersection for our check, so multiply it out a little...
bool Entity_IsTouching(ServerEntity_t *eEntity, ServerEntity_t *eOther) {
    return Math_IsIntersecting(eEntity->v.mins, eEntity->v.maxs, eOther->v.mins, eOther->v.maxs);
}

bool Entity_IsOnGround(ServerEntity_t *entity)
{
	if (entity->v.flags & FL_ONGROUND)
		return true;

	return false;
}

/*
	IO System
*/

/*
	Host Content
*/

Material *seGetMaterial(ServerEntity_t *entity)
{
	// Try to grab the model.
	model_t *model = g_engine->GetServerEntityModel(entity);
	if (!model)
		return NULL;

	// Return the initial material.
	return &model->materials[0];
}

MaterialSkin *seGetSkin(ServerEntity_t *entity, unsigned int skin)
{
	Material *material = seGetMaterial(entity);
	if (!material)
		return NULL;

	return &material->skin[skin];
}

MaterialSkin *seGetCurrentSkin(ServerEntity_t *entity)
{
	return seGetSkin(entity, entity->Model.skin);
}

MaterialProperty_t seGetSkinPhysicsProperty(ServerEntity_t *entity, MaterialSkin *skin)
{
	if (!skin)
	{
		g_engine->Con_Warning("Passed invalid skin!\n");
		return MATERIAL_TYPE_NONE;
	}

	return (MaterialProperty_t)skin->uiType;
}