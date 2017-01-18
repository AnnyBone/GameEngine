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

using namespace game::server;

// Create a new entity instance.
Entity::Entity() : instance_(nullptr) {
}

Entity::~Entity() {
    Free();
}

void Entity::Spawn() {
    if (instance_) {
        Free();
    }

    // Set physics properties to their defaults!
    instance_->Physics.mass = 1.0f;
    instance_->Physics.friction = 1.0f;
    instance_->Physics.gravity = SERVER_GRAVITY;

    // Spawn the entity.
    instance_ = g_engine->Spawn();
}

void Entity::Free()
{
    if (!instance_) {
        return;
    }

    g_engine->FreeEntity(instance_);
    instance_ = 0;
}

void Entity::Link(bool touchtriggers) {
    g_engine->LinkEntity(instance_, touchtriggers);
}

void Entity::Unlink() {
    g_engine->UnlinkEntity(instance_);
}

/*
Set the size of the bounding box for the entity.
Should be called AFTER setting model!
*/
void Entity::SetSize(PLVector3D mins, PLVector3D maxs) {
    // Ensure the model hasn't already been set.
    if (!instance_->v.model) {
        g_engine->Con_Warning("Setting entity size before model! (%s)\n", instance_->v.classname);
    }

    // Ensure the mins/maxs are the right way round.
    if ((mins.x > maxs.x) || (mins.y > maxs.y) || (mins.z > maxs.z)) {
        g_engine->Con_Warning("Backwards mins/maxs! (%s)\n", instance_->v.classname);
        return;
    }

    instance_->v.mins = mins;
    instance_->v.maxs = maxs;
    instance_->v.size = maxs - mins;

    Link(false);
}

/*
Set the size of the bounding box for the entity.
Should be called AFTER setting model!
*/
void Entity::SetSize(
        float mina, float minb, float minc,
        float maxa, float maxb, float maxc) {
    SetSize(PLVector3D(mina, minb, minc), PLVector3D(maxa, maxb, maxc));
}

/*
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
void Entity::SetOrigin(PLVector3D origin) {
    instance_->v.origin = origin;

    Link(false);
}

// Sets the angle of the given entity.
void Entity::SetAngles(PLVector3D angles) {
    instance_->v.angles = angles;

    // TODO: Link?
}

// Sets the model of the given entity.
void Entity::SetModel(const char *path) {
    g_engine->SetModel(instance_, path);
}

/*	Effects	*/

void Entity::AddEffects(int effects) {
    instance_->v.effects |= effects;
}

void Entity::ClearEffects() {
    instance_->v.effects = 0;
}

void Entity::RemoveEffects(int effects) {
    instance_->v.effects &= ~effects;
}

/*	Flags	*/

void Entity::AddFlags(int flags) {
    instance_->v.flags |= flags;
}

void Entity::ClearFlags() {
    instance_->v.flags = 0;
}

void Entity::RemoveFlags(int flags) {
    instance_->v.flags &= ~flags;
}

/*	Damage	*/

void Entity::Damage(Entity *inflictor, int damage, EntityDamageType_t damagetype) {
    // Don't bother if there's no actual damage inflicted.
    if (damage <= 0) {
        return;
    }

    // Only continue if we can damage the entity.
    if (!CanDamage(inflictor, damagetype)) {
        return;
    }

    instance_->v.health -= damage;
    if (instance_->v.health <= 0) {
        Killed(inflictor);
        return;
    }

    // TODO: Pass amount of damage?
    Damaged(inflictor);
}

bool Entity::CanDamage(Entity *target, EntityDamageType_t damagetype) {
    if (!target->GetInstance()->v.takedamage) {
        return false;
    }

    // Can't damage players on the same team.
    // Ensure we have an assigned team before checking this!
    if (instance_->local.pTeam && (instance_->local.pTeam == instance_->local.pTeam)) {
        return false;
    }

    // See if we have a supported damage type.
    return !target->GetInstance()->local.iDamageType || (target->GetInstance()->local.iDamageType == damagetype);
}

/*	Physics	*/

// Returns true if two entities are intersecting.
bool Entity::IsTouching(Entity *other) {
    // todo, this doesn't take origin positions into account, duh...
    return Math_IsIntersecting(
            instance_->v.mins, instance_->v.maxs,
            other->GetInstance()->v.mins, other->GetInstance()->v.maxs
    );
}

/*	Attempts to drop the entity to the floor.
 *	Returns true on success and false on failure.
 */
bool Entity::DropToFloor() {
    PLVector3D end = instance_->v.origin;
    end.z -= 256;

    trace_t ground = g_engine->Server_Move(instance_->v.origin, instance_->v.mins, instance_->v.maxs, end, false, instance_);
    if ((ground.fraction == 1) || ground.bAllSolid) {
        g_engine->Con_Warning("Entity is stuck in world! (%s) (%i %i %i)\n",
                              instance_->v.classname,
                              (int)instance_->v.origin.x,
                              (int)instance_->v.origin.y,
                              (int)instance_->v.origin.z);
        return false;
    }

    // Use SetOrigin so that it's automatically linked.
    SetOrigin(ground.endpos);

    AddFlags(FL_ONGROUND);

    instance_->v.groundentity = ground.ent;

    return true;
}