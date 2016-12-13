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

char *Entity_AllocateString(char *string) {
    char *newstring, *new_p;
    int i, l;

    l = strlen(string) + 1;
    newstring = (char *) Engine.Hunk_Alloc(l);
    new_p = newstring;

    for (i = 0; i < l; i++) {
        if (string[i] == '\\' && i < l - 1) {
            i++;
            if (string[i] == 'n')
                *new_p++ = '\n';
            else
                *new_p++ = '\\';
        } else
            *new_p++ = string[i];
    }

    return newstring;
}

typedef struct {
    const char *varname;

    int offset;

    DataType_t datatype;

    int flags;
} ServerEntityField_t;

ServerEntityField_t entity_fields[] =
        {
                {"classname",       SERVER_ENTITY_FIELD(v.cClassname),          DATA_STRING},
                {"name",            SERVER_ENTITY_FIELD(v.cName),               DATA_STRING},
                {"model",           SERVER_ENTITY_FIELD(v.model),               DATA_STRING},
                {"targetname",      SERVER_ENTITY_FIELD(v.targetname),          DATA_STRING},
                {"noise",           SERVER_ENTITY_FIELD(v.noise),               DATA_STRING},
                {"message",         SERVER_ENTITY_FIELD(v.message),             DATA_STRING},
                {"origin",          SERVER_ENTITY_FIELD(v.origin),              DATA_VECTOR3},
                {"angles",          SERVER_ENTITY_FIELD(v.angles),              DATA_VECTOR3},
                {"light",           SERVER_ENTITY_FIELD(v.vLight),              DATA_VECTOR4},
                {"health",          SERVER_ENTITY_FIELD(v.iHealth),             DATA_INTEGER},
                {"spawnflags",      SERVER_ENTITY_FIELD(v.spawnflags),          DATA_INTEGER},
                {"bTakeDamage",     SERVER_ENTITY_FIELD(v.bTakeDamage),         DATA_BOOLEAN},
                {"takedamage",      SERVER_ENTITY_FIELD(v.bTakeDamage),         DATA_BOOLEAN},
                {"alpha",           SERVER_ENTITY_FIELD(alpha),                 DATA_INTEGER},
                {"frame",           SERVER_ENTITY_FIELD(v.frame),               DATA_INTEGER},

                // Model properties
                {"model_skin",      SERVER_ENTITY_FIELD(Model.skin),            DATA_INTEGER},
                {"scale",           SERVER_ENTITY_FIELD(Model.scale),           DATA_FLOAT},

                // Physical properties
                {"physics_solid",   SERVER_ENTITY_FIELD(Physics.solid),         DATA_INTEGER},
                {"physics_mass",    SERVER_ENTITY_FIELD(Physics.mass),          DATA_FLOAT},
                {"physics_gravity", SERVER_ENTITY_FIELD(Physics.gravity),       DATA_FLOAT},

                // Local (move these at some point)
                {"sound",           SERVER_ENTITY_FIELD(local.sound),           DATA_STRING},
                {"soundstart",      SERVER_ENTITY_FIELD(local.cSoundStart),     DATA_STRING},
                {"soundstop",       SERVER_ENTITY_FIELD(local.sound_stop),      DATA_STRING},
                {"soundmoving",     SERVER_ENTITY_FIELD(local.cSoundMoving),    DATA_STRING},
                {"soundreturn",     SERVER_ENTITY_FIELD(local.cSoundReturn),    DATA_STRING},
                {"target1",         SERVER_ENTITY_FIELD(local.cTarget1),        DATA_STRING},
                {"target2",         SERVER_ENTITY_FIELD(local.cTarget2),        DATA_STRING},
                {"speed",           SERVER_ENTITY_FIELD(local.speed),           DATA_FLOAT},
                {"delay",           SERVER_ENTITY_FIELD(local.delay),           DATA_FLOAT},
                {"lip",             SERVER_ENTITY_FIELD(local.lip),             DATA_FLOAT},
                {"wait",            SERVER_ENTITY_FIELD(local.dWait),           DATA_DOUBLE},
                {"damage",          SERVER_ENTITY_FIELD(local.iDamage),         DATA_INTEGER},
                {"volume",          SERVER_ENTITY_FIELD(local.volume),          DATA_INTEGER},
                {"style",           SERVER_ENTITY_FIELD(local.style),           DATA_INTEGER},
                {"count",           SERVER_ENTITY_FIELD(local.count),           DATA_INTEGER},
                {"pTeam",           SERVER_ENTITY_FIELD(local.pTeam),           DATA_INTEGER},
                {"attack_finished", SERVER_ENTITY_FIELD(local.dAttackFinished), DATA_DOUBLE},

                // hacks
                {"angle",           SERVER_ENTITY_FIELD(v.angles),              DATA_VECTOR3, FL_ANGLEHACK},

                // Ignore these global fields.
                {"wad",         0,                                              EV_NONE},
                {"ambient",     0,                                              EV_NONE},
                {"sky",         0,                                              EV_NONE},
                {"cloud",       0,                                              EV_NONE},
                {"scrollspeed", 0,                                              EV_NONE},
                {"mapversion",  0,                                              EV_NONE},

                {0}
        };

/*	Automatically called by the engine during entity parsing.
*/
void ServerEntity_ParseField(char *key, char *value, ServerEntity_t *entity) {
    for (ServerEntityField_t *field = entity_fields; field->varname; field++) {
        if (!strncmp(field->varname, key, sizeof(field->varname))) {
            switch (field->datatype) {
                case DATA_STRING:
                    *(char **) ((uint8_t *) entity + field->offset) = Entity_AllocateString(value);
                    break;
                case DATA_VECTOR3:
                    if (field->flags & FL_ANGLEHACK) {
                        ((float *) ((uint8_t *) entity + field->offset))[0] = 0;
                        ((float *) ((uint8_t *) entity + field->offset))[1] = strtof(value, NULL);
                        ((float *) ((uint8_t *) entity + field->offset))[2] = 0;
                    } else {
                        MathVector3f_t vector;
                        Math_VectorSet(0, vector);
                        if (sscanf(value, "%f %f %f", &vector[0], &vector[1], &vector[2]) < 3)
                            Engine.Con_Warning("Field did not return expected number of arguments! (%s)\n", value);
                        ((float *) ((uint8_t *) entity + field->offset))[0] = vector[0];
                        ((float *) ((uint8_t *) entity + field->offset))[1] = vector[1];
                        ((float *) ((uint8_t *) entity + field->offset))[2] = vector[2];
                    }
                    break;
                case DATA_VECTOR4: {
                    plVector4f_t vector;
                    Math_VectorSet(0, vector);
                    if (sscanf(value, "%f %f %f %f", &vector[0], &vector[1], &vector[2], &vector[3]) < 4)
                        Engine.Con_Warning("Field did not return expected number of arguments! (%s)\n", value);
                    ((float *) ((uint8_t *) entity + field->offset))[0] = vector[0];
                    ((float *) ((uint8_t *) entity + field->offset))[1] = vector[1];
                    ((float *) ((uint8_t *) entity + field->offset))[2] = vector[2];
                    ((float *) ((uint8_t *) entity + field->offset))[3] = vector[3];
                }
                    break;
                case DATA_FLOAT:
                    *(float *) ((uint8_t *) entity + field->offset) = strtof(value, NULL);
                    break;
                case DATA_DOUBLE:
                    *(double *) ((uint8_t *) entity + field->offset) = strtod(value, NULL);
                    break;
                case DATA_BOOLEAN:
                    if (!strcmp(value, "true"))
                        value = "1";
                    else if (!strcmp(value, "false"))
                        value = "0";
                    // Booleans are handled in the same way as integers, so don't break here!
                case DATA_INTEGER:
                    *(int *) ((uint8_t *) entity + field->offset) = atoi(value);
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
ServerEntity_t *Entity_Spawn(void) {
    ServerEntity_t *eSpawn = Engine.Spawn();

    // Set physics properties to their defaults!
    eSpawn->Physics.mass = 1.0f;
    eSpawn->Physics.friction = 1.0f;
    eSpawn->Physics.gravity = SERVER_GRAVITY;

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
void Entity_SetOrigin(ServerEntity_t *eEntity, MathVector3f_t vOrigin) {
    plVectorCopy(vOrigin, eEntity->v.origin);

    Entity_Link(eEntity, false);
}

/*	Sets the angle of the given entity.
*/
void Entity_SetAngles(ServerEntity_t *eEntity, MathVector3f_t vAngles) {
    plVectorCopy(vAngles, eEntity->v.angles);
    // TODO: Link?
}

/*	Sets the model of the given entity.
*/
void Entity_SetModel(ServerEntity_t *eEntity, const char *cModelPath) {
    if (!cModelPath) {
        Engine.Con_Warning("Invalid path for model! (%s)", eEntity->v.cClassname);
        return;
    }

    Engine.SetModel(eEntity, cModelPath);
}

/*	Sets the size of the given entity; requires that the model has been applied first.
	Alternative to SetSize.
*/
void Entity_SetSizeVector(ServerEntity_t *eEntity, MathVector3f_t vMin, MathVector3f_t vMax) {
    int i;

    // Check if the model is set yet, if not give us a little warning.
    if (!eEntity->v.model)
        Engine.Con_Warning("Setting entity size before model! (%s)\n", eEntity->v.cClassname);

    for (i = 0; i < 3; i++)
        if (vMin[i] > vMax[i]) {
            Engine.Con_Warning("Backwards mins/maxs! (%s)\n", eEntity->v.cClassname);
            return;
        }

    plVectorCopy(vMin, eEntity->v.mins);
    plVectorCopy(vMax, eEntity->v.maxs);
    Math_VectorSubtract(vMax, vMin, eEntity->v.size);

    Entity_Link(eEntity, false);
}

/*	Set the size of the bounding box for the entity.
	Should be called AFTER setting model!
*/
void Entity_SetSize(ServerEntity_t *eEntity,
                    float fMinA, float fMinB, float fMinC,
                    float fMaxA, float fMaxB, float fMaxC) {
    MathVector3f_t vMin = {0, 0, 0}, vMax = {0, 0, 0};

    vMin[0] = fMinA;
    vMin[1] = fMinB;
    vMin[2] = fMinC;
    vMax[0] = fMaxA;
    vMax[1] = fMaxB;
    vMax[2] = fMaxC;

    Entity_SetSizeVector(eEntity, vMin, vMax);
}

/*	Can be used for convenience.
*/
void Entity_AddEffects(ServerEntity_t *eEntity, int iEffects) {
    if (eEntity->v.effects & iEffects)
        return;
    eEntity->v.effects |= iEffects;
}

/*	Can be used for convenience.
*/
void Entity_RemoveEffects(ServerEntity_t *eEntity, int iEffects) {
    if (!(eEntity->v.effects & iEffects))
        return;
    eEntity->v.effects &= ~iEffects;
}

/*	Can be used for convenience.
*/
void Entity_ClearEffects(ServerEntity_t *eEntity) {
    eEntity->v.effects = 0;
}

/*	Can be used for convenience.
*/
void Entity_AddFlags(ServerEntity_t *eEntity, int iFlags) {
    if (eEntity->v.flags & iFlags)
        return;
    eEntity->v.flags |= iFlags;
}

/*	Can be used for convenience.
*/
void Entity_RemoveFlags(ServerEntity_t *eEntity, int iFlags) {
    if (!(eEntity->v.flags & iFlags))
        return;
    eEntity->v.flags &= ~iFlags;
}

/*	Can be used for convenience.
*/
void Entity_ClearFlags(ServerEntity_t *eEntity, int iFlags) {
    eEntity->v.flags = 0;
}

/*	Simple function for checking if an entity can be damaged or not.
*/
bool Entity_CanDamage(ServerEntity_t *eEntity, ServerEntity_t *eTarget, EntityDamageType_t iDamageType) {
    // Can't damage people on the same team.
    if (eEntity->local.pTeam && (eEntity->local.pTeam == eTarget->local.pTeam))
        return false;

    if (eTarget->v.bTakeDamage && (!eTarget->local.iDamageType || (eTarget->local.iDamageType == iDamageType)))
        return true;

    return false;
}

void Entity_Damage(ServerEntity_t *seEntity, ServerEntity_t *seInflictor, int iDamage, EntityDamageType_t type) {
    // Don't bother if there's no actual damage inflicted.
    if (iDamage <= 0)
        return;

    // Only continue if we can damage the entity.
    if (!Entity_CanDamage(seInflictor, seEntity, type))
        return;

    // If it's a monster or player, hand it over to the monster code.
    if (Entity_IsMonster(seEntity) || Entity_IsPlayer(seEntity)) {
        Monster_Damage(seEntity, seInflictor, iDamage, type);
        return;
    }

    // Otherwise we'll do our own thing here...

    seEntity->v.iHealth -= iDamage;
    if (seEntity->v.iHealth <= 0) {
        if (seEntity->local.KilledFunction)
            seEntity->local.KilledFunction(seEntity, seInflictor, type);
        return;
    }

    if (seEntity->local.DamagedFunction)
        seEntity->local.DamagedFunction(seEntity, seInflictor, type);
}

/*	Damage entities within a specific radius.
*/
void Entity_RadiusDamage(ServerEntity_t *eInflictor, float fRadius, int iDamage, EntityDamageType_t iDamageType) {
    int i;
    float fDistance;
    MathVector3f_t vOrigin = {0, 0, 0};
    ServerEntity_t *eTarget = Engine.Server_FindRadius(eInflictor->v.origin, fRadius);

    do {
        if (eTarget->v.bTakeDamage) {
            for (i = 0; i < 3; i++)
                vOrigin[i] = eTarget->v.origin[i] + (eTarget->v.mins[i] + eTarget->v.maxs[i]) * 0.5f;

            Math_VectorSubtract(eInflictor->v.origin, vOrigin, vOrigin);

            fDistance = 0.5f * plLengthf(vOrigin);
            if (fDistance > 0) {
                Math_VectorInverse(vOrigin);
                Math_VectorAdd(eTarget->v.velocity, vOrigin, eTarget->v.velocity);

                // Reduce the damage by distance.
                fDistance = (float) iDamage - (100.0f / fDistance);

                // Less damage for the inflictor.
                if (eTarget == eInflictor)
                    fDistance = fDistance / 2.0f;

                if (fDistance > 0)
                    Entity_Damage(eTarget, eInflictor, (int) fDistance, iDamageType);
            }
        }

        eTarget = eTarget->v.chain;
    } while (eTarget);
}

/*	Link an entity.
*/
void Entity_Link(ServerEntity_t *eEntity, bool bTouchTriggers) {
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
void Entity_CheckFrames(ServerEntity_t *eEntity) {
    // If something isn't active and Animationtime is over
    if (!eEntity->local.iAnimationEnd || (Server.dTime < eEntity->local.dAnimationTime))
        return;
    else if (eEntity->local.iAnimationCurrent > eEntity->local.iAnimationEnd) {
        eEntity->local.dAnimationTime = 0;
        return;
    }

    eEntity->v.frame = eEntity->local.iFrames[eEntity->local.iAnimationCurrent].frame;

    eEntity->local.dAnimationTime =
            Server.dTime + ((double) eEntity->local.iFrames[eEntity->local.iAnimationCurrent].speed);

    if (eEntity->local.iFrames[eEntity->local.iAnimationCurrent].Event)
        eEntity->local.iFrames[eEntity->local.iAnimationCurrent].Event(eEntity);

    eEntity->local.iAnimationCurrent++;
}

/*	Resets the currently active animation.
*/
void Entity_ResetAnimation(ServerEntity_t *eEntity) {
    eEntity->v.frame =
    eEntity->local.iAnimationCurrent =
    eEntity->local.iAnimationEnd = 0;

    // Reset the animation time.
    eEntity->local.dAnimationTime = 0;
}

/*	Start animating the entity.
*/
void Entity_Animate(ServerEntity_t *eEntity, ServerEntityFrame_t *efAnimation) {
    int i = 0;

    Entity_ResetAnimation(eEntity);

    eEntity->local.iAnimationCurrent = 0;

    eEntity->v.frame = efAnimation[0].frame;

    for (;;) {
        if (efAnimation[i].isend) {
            eEntity->local.iAnimationEnd = i;
            break;
        }

        i++;
    }

    eEntity->local.dAnimationTime = Server.dTime + ((double) efAnimation[1].speed);
    eEntity->local.iFrames = efAnimation;
}

/*	Returns true if an animation is currently playing.
*/
bool Entity_IsAnimating(ServerEntity_t *entity) {
    if (!entity->local.iAnimationEnd || (Server.dTime < entity->local.dAnimationTime))
        return false;

    return true;
}

/*
	Type Checks
*/

/*	Is the given monster a player?
*/
bool Entity_IsPlayer(ServerEntity_t *eEntity) {
    if (eEntity->Monster.iType == MONSTER_PLAYER)
        return true;

    return false;
}

/*	Is the given monster a monster? (this is dumb...)
*/
bool Entity_IsMonster(ServerEntity_t *eEntity) {
    if ((eEntity->Monster.iType != MONSTER_PLAYER) && (eEntity->Monster.iType > MONSTER_VEHICLE))
        return true;

    return false;
}

/*
	Physics
*/

/*	Sets up the physical properties for the entity.
*/
void Entity_SetPhysics(ServerEntity_t *seEntity, EntitySolidType_t pstSolidType, float fMass, float fFriction) {
    seEntity->Physics.solid = pstSolidType;
    seEntity->Physics.mass = fMass;
    seEntity->Physics.gravity = SERVER_GRAVITY;
    seEntity->Physics.friction = fFriction;
}

/*
	Math/Utility Functions
*/

void Entity_MakeVectors(ServerEntity_t *eEntity) {
    plAngleVectors(eEntity->v.v_angle, eEntity->local.vForward, eEntity->local.vRight, eEntity->local.vUp);
}

bool Entity_DropToFloor(ServerEntity_t *eEntity) {
    MathVector3f_t vEnd = {0, 0, 0};
    trace_t trGround;

    plVectorCopy(eEntity->v.origin, vEnd);

    vEnd[2] -= 256;

    trGround = Engine.Server_Move(eEntity->v.origin, eEntity->v.mins, eEntity->v.maxs, vEnd, false, eEntity);
    if (trGround.fraction == 1 || trGround.bAllSolid) {
        Engine.Con_Warning("Entity is stuck in world! (%s) (%i %i %i)\n", eEntity->v.cClassname,
                           (int) eEntity->v.origin[0],
                           (int) eEntity->v.origin[1],
                           (int) eEntity->v.origin[2]);
        return false;
    }

    // Use SetOrigin so that it's automatically linked.
    Entity_SetOrigin(eEntity, trGround.endpos);

    Entity_AddFlags(eEntity, FL_ONGROUND);

    eEntity->v.groundentity = trGround.ent;

    return true;
}

bool Entity_IsTouching(ServerEntity_t *eEntity, ServerEntity_t *eOther) {
    if (eEntity->v.mins[0] > eOther->v.maxs[0] ||
        eEntity->v.mins[1] > eOther->v.maxs[1] ||
        eEntity->v.mins[2] > eOther->v.maxs[2] ||
        eEntity->v.maxs[0] < eOther->v.mins[0] ||
        eEntity->v.maxs[1] < eOther->v.mins[1] ||
        eEntity->v.maxs[2] < eOther->v.mins[2])
        return false;

    return true;
}

bool Entity_IsOnGround(ServerEntity_t *entity) {
    if (entity->v.flags & FL_ONGROUND)
        return true;

    return false;
}

/*	IO System   */

/*	Host Content    */

Material_t *SEntity_GetMaterial(ServerEntity_t *entity) {
    // Try to grab the model.
    model_t *model = g_engine->GetServerEntityModel(entity);
    if (!model)
        return NULL;

    // Return the initial material.
    return &model->materials[0];
}

MaterialSkin_t *SEntity_GetSkin(ServerEntity_t *entity, unsigned int skin) {
    Material_t *material = SEntity_GetMaterial(entity);
    if (!material)
        return NULL;

    return &material->skin[skin];
}

MaterialSkin_t *SEntity_GetCurrentSkin(ServerEntity_t *entity) {
    return SEntity_GetSkin(entity, entity->Model.skin);
}

MaterialProperty_t seGetSkinPhysicsProperty(ServerEntity_t *entity, MaterialSkin_t *skin) {
    if (!skin) {
        g_engine->Con_Warning("Passed invalid skin!\n");
        return MATERIAL_TYPE_NONE;
    }

    return (MaterialProperty_t)skin->uiType;
}