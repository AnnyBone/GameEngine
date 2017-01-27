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

#include "server_weapon.h"
#include "server_item.h"
#include "server_player.h"

/*
	Base code for the weapon system.
*/

Weapon_t Weapons[] =
{
	{
		ITEM_WEAPON_NONE
	},

#ifdef GAME_OPENKATANA
	{
		WEAPON_KATANA,
		DAIKATANA_MODEL_VIEW,
		Daikatana_Deploy,

		// Primary
		AM_MELEE,
		Daikatana_PrimaryAttack
	},

	{
		WEAPON_DAIKATANA,
		DAIKATANA_MODEL_VIEW,
		Daikatana_Deploy,

		// Primary
		AM_MELEE,
		Daikatana_PrimaryAttack
	},

	{
		WEAPON_IONRIFLE,
		"models/weapons/v_ionblaster.md2",
		IonBlaster_Deploy,

		// Primary
		AM_IONS,
		IonBlaster_PrimaryAttack,
	},

	{
		WEAPON_C4VIZATERGO,
		"models/weapons/v_c4.md2",
		C4Vizatergo_Deploy,

		// Primary
		AM_C4BOMB,
		C4Vizatergo_PrimaryAttack,

		// Secondary
		AM_NONE,
		C4Vizatergo_SecondaryAttack
	},

	{
		WEAPON_SHOTCYCLER,
		"models/weapons/v_shotcycler.md2",
		Shotcycler_Deploy,

		// Primary
		AM_SHELL,
		Shotcycler_PrimaryAttack
	},

	{
		WEAPON_SIDEWINDER,
		"models/weapons/v_sidewinder.md2",
		SideWinder_Deploy,

		// Primary
		AM_ROCKET,
		SideWinder_PrimaryAttack
	},

	{
		WEAPON_SHOCKWAVE,
		"models/weapons/v_shockwave.md2",
		Shockwave_Deploy,

		// Primary
		AM_LASER,
		Shockwave_PrimaryAttack
	},

	// AI Weapons
	{
		ITEM_WEAPON_LASERS,
		NULL,
		NULL,

		// Primary
		AM_LASER,
		IonBlaster_PrimaryAttack	// [31/3/2013] Temporary ~hogsy
	},
#elif GAME_ADAMAS
#endif

	{	0,	NULL,	NULL,	AM_NONE, NULL,	AM_NONE,	NULL	}
};

static Weapon_t *LAST_WEAPON = Weapons + (plArrayElements(Weapons) - 2);

/*	Returns the entities current active weapon.
*/
Weapon_t *Weapon_GetCurrentWeapon(ServerEntity_t *eEntity)
{
	Weapon_t	*wCurrentWeapon;

	for(wCurrentWeapon = Weapons; wCurrentWeapon->iItem; wCurrentWeapon++)
		if(eEntity->v.iActiveWeapon == wCurrentWeapon->iItem)
			return wCurrentWeapon;

	return Weapon_GetWeapon(0);
}

Weapon_t *Weapon_GetWeapon(int iWeaponID)
{
	Weapon_t *wWeapon;

	for(wWeapon = Weapons; wWeapon->iItem; wWeapon++)
		if(iWeaponID == wWeapon->iItem)
			return wWeapon;

	return NULL;
}

void Weapon_Precache(void)
{
#ifdef GAME_OPENKATANA
	Server_PrecacheSound(SOUND_WEAPON_SWITCH);

	Server_PrecacheModel(DAIKATANA_MODEL_VIEW);
	Server_PrecacheModel("models/weapons/v_ionblaster.md2");
	Server_PrecacheModel("models/weapons/v_c4.md2");
	Server_PrecacheModel("models/weapons/v_shotcycler.md2");
	Server_PrecacheModel("models/weapons/v_sidewinder.md2");
	Server_PrecacheModel("models/weapons/v_shockwave.md2");
	// TEMP START
	Server_PrecacheModel("models/weapons/w_ionblaster.md2");
	Server_PrecacheModel("models/weapons/w_c4.md2");
	Server_PrecacheModel("models/weapons/w_shotcycler.md2");
	Server_PrecacheModel("models/weapons/w_sidewinder.md2");
	Server_PrecacheModel("models/weapons/w_shockwave.md2");
	Server_PrecacheModel("models/weapons/w_daikatana.md2");
	// TEMP END
	Server_PrecacheModel("models/slaser.md2");
	Server_PrecacheModel("models/c4ammo.md2");
	Server_PrecacheModel("models/ionball.md2");
	Server_PrecacheModel("models/sidewinder_missile.md2");

	// Ion Rifle
	Server_PrecacheModel("models/weapons/v_ionrifle.md2");
	Server_PrecacheSound("weapons/laser.wav");

	// C4
	Server_PrecacheSound("weapons/c4/beep.wav");
	Server_PrecacheSound("weapons/c4/c4cock.wav");
	Server_PrecacheSound("weapons/c4/c4fire.wav");
	Server_PrecacheSound("weapons/c4/c4stick.wav");

	// Ion Blaster
	Server_PrecacheSound("weapons/ionblaster/explode.wav");
	Server_PrecacheSound("weapons/ionblaster/fire.wav");
	Server_PrecacheSound("weapons/ionblaster/bounce.wav");
	Server_PrecacheModel("models/ionball.md2");

	// Shockwave
	Server_PrecacheSound("weapons/shockwave/fade.wav");
	Server_PrecacheSound("weapons/shockwave/fire.wav");
	Server_PrecacheSound("weapons/shockwave/ready.wav");
	Server_PrecacheSound("weapons/shockwave/warmup.wav");

	Server_PrecacheSound("weapons/shotcycler6/fire0.wav");
	Server_PrecacheSound("weapons/shotcycler6/fire1.wav");
	Server_PrecacheSound("weapons/shotcycler6/barrelspin.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotcyclerready.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotcyclerclick.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotclick.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotpull.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotslap.wav");

	// Sidewinder
	Server_PrecacheSound(SOUND_SIDEWINDER_FIRE);
	Server_PrecacheSound("weapons/sidewinder/sidewinderflyby.wav");
	Server_PrecacheSound("weapons/sidewinder/sidewindersplash.wav");
	Server_PrecacheSound("weapons/sidewinder/sidewinderunderwaterfire.wav");
	Server_PrecacheSound("weapons/sidewinder/sidewinderunderwaterflyby.wav");
#elif GAME_ADAMAS
#endif
}

/*	Enable automatic aim assitance.
*/
PLVector3D Weapon_Aim(ServerEntity_t *entity)
{
	int				i, j;
	float			fDistance, fBestDistance;
	trace_t			tAimLine;
	ServerEntity_t	*eCheck, *eBest;
	PLVector3D	mvEnd, mvBestDirection;

	// Copy the entity's origin as our starting position.
    PLVector3D start = entity->v.origin;

	// Move the starting position up a little.
	start[2] += 20.0f;

	Entity_MakeVectors(entity);

	// Try tracing straight.
    PLVector3D direction = entity->local.forward;
	Math_VectorMA(start, 2048.0f, direction, mvEnd);

	tAimLine = Engine.Server_Move(start, PLVector3D(0, 0, 0), PLVector3D(0, 0, 0), mvEnd, 0, entity);
	// See if we encountered anything we can damage.
	if (tAimLine.ent && tAimLine.ent->v.takedamage) {
		// Check the teams.
		if ((entity->local.team <= TEAM_NEUTRAL) || (entity->local.team != tAimLine.ent->local.team)) {
			return entity->local.forward;
		}
	}

	// Try every possible entity.
    mvBestDirection = direction;
	fBestDistance = cvServerAim.value;
	eBest = NULL;

	eCheck = SERVER_ENTITY_NEXT(Engine.Server_GetEdicts());
	for (i = 1; i < Engine.Server_GetNumEdicts(); i++, eCheck = SERVER_ENTITY_NEXT(eCheck))
	{
		if (eCheck == entity)
			continue;

		if (!eCheck->v.takedamage)
			continue;

		for (j = 0; j < 3; j++)
			mvEnd[j] = eCheck->v.origin[j] + 0.5f * (eCheck->v.mins[j] + eCheck->v.maxs[j]);

        direction = mvEnd - start;
        direction.Normalize();

		fDistance = Math_DotProduct(direction, entity->local.forward);
		if (fDistance < fBestDistance) {
            // Too far to turn.
            continue;
        }

		tAimLine = Engine.Server_Move(start, pl_origin3f, pl_origin3f, mvEnd, 0, entity);
		if (tAimLine.ent == eCheck) {
			// Can shoot at this one.
			fBestDistance = fDistance;
			eBest = eCheck;
		}
	}

	MathVector_t mvResult;

	if (eBest)
	{
		plVectorSubtract3fv(eBest->v.origin, entity->v.origin, direction);

		fDistance = Math_DotProduct(direction, entity->local.forward);

		plVectorScalef(entity->local.forward, fDistance, mvEnd);

		mvEnd[2] = direction[2];

		plVectorNormalize(mvEnd);
		Math_VectorToMV(mvEnd, mvResult);
	}
	else
		Math_VectorToMV(mvBestDirection, mvResult);

	return mvResult;
}

// Kicks the view a little.
void Weapon_ViewPunch(ServerEntity_t *entity, float intensity, bool add) {
	if(add) {
        entity->v.punchangle[0] -= intensity + (float) (rand() % 2);
    } else {
        entity->v.punchangle[0] = -intensity + (float) (rand() % 2);
    }
}

PLVector3D mvTraceMaxs = { 4, 4, 4 };
PLVector3D mvTraceMins = { -4, -4, -4 };

/*	Runs a trace to see if a projectile can be casted.
*/
bool Weapon_CheckTrace(ServerEntity_t *owner)
{
	int i, iTraceContents;
	PLVector3D mvTarget;
	trace_t tCheck;

	// Copy over the players origin.
    PLVector3D mvSource = owner->v.origin;

	// Update the origin to the correct view offset.
	mvSource.z += owner->v.view_ofs.z;

	// Apply the distance to the target for the trace.
	for (i = 0; i < 3; i++)
		mvTarget[i] = mvSource[i] * 128.0f;

	// Check that there's enough space for projectile.
	tCheck = Engine.Server_Move(mvSource, mvTraceMins, mvTraceMaxs, mvTarget, MOVE_NOMONSTERS, owner);
	if (!tCheck.bOpen)
		return false;
	
	// Check to see if there's a target, and it's not the world!
	if ((tCheck.ent != Server.world) && (tCheck.ent != owner)) {
		// Are we intersecting with it?
		if (Math_IsIntersecting(mvTraceMins, mvTraceMaxs, tCheck.ent->v.mins, tCheck.ent->v.maxs)) {
            return false;
        }
	}

	// Ensure that we're not inside the sky or within a solid.
	iTraceContents = Engine.Server_PointContents(tCheck.endpos);
    return !((iTraceContents == BSP_CONTENTS_SKY) || (iTraceContents == BSP_CONTENTS_SOLID));
}

// Attempts to throw out a projectile.
void Weapon_Projectile(ServerEntity_t *owner, ServerEntity_t *projectile, float fVelocity) {
	// Scale with the speed and copy over the angled velocity.
    projectile->v.velocity = Weapon_Aim(owner) * fVelocity;
}

void Weapon_BulletProjectile(ServerEntity_t *owner, float spread, int damage, PLVector3D direction) {
    PLVector3D source = owner->v.origin;
	source.z += owner->v.view_ofs.z;

    PLVector3D targ(
            source.x + (direction.x * 2048.0f) + (spread * plCRandom() * 20.0f),
            source.y + (direction.y * 2048.0f) + (spread * plCRandom() * 20.0f),
            source.z + (direction.z * 2048.0f) + (spread * plCRandom() * 20.0f)
    );

    trace_t trace = Traceline(owner,source,targ,0);
	if ((trace.fraction == 1.0f) || (Engine.Server_PointContents(trace.endpos) == BSP_CONTENTS_SKY)) {
        return;
    } else {
		if(trace.ent && trace.ent->v.takedamage) {
            Entity_Damage(trace.ent, owner, damage, DAMAGE_TYPE_NONE);
        } else {
			ServerEntity_t *puff = Entity_Spawn();
			if(puff) {
				puff->v.think       = Entity_Remove;
				puff->v.nextthink   = Server.time + 0.5;

				Entity_SetOrigin(puff,trace.endpos);

                char sound[128];
				PHYSICS_SOUND_RICOCHET(sound);
				Sound(puff,CHAN_BODY,sound,255,ATTN_NORM);
			}

			char smoke[6];
			PARTICLE_SMOKE(smoke);
			g_engine->Particle(trace.endpos, PLVector3D(0, 0, 0), 15, smoke, 15);
		}
	}
}

void Weapon_UpdateCurrentAmmo(Weapon_t *wWeapon, ServerEntity_t *eEntity)
{
	switch (wWeapon->primary_type) {
#ifdef GAME_OPENKATANA
	case AM_IONS:
		eEntity->v.primary_ammo = eEntity->local.ionblaster_ammo;
		break;
	case AM_ROCKET:
		eEntity->v.primary_ammo = eEntity->local.sidewinder_ammo;
		break;
	case AM_BULLET:
		eEntity->v.primary_ammo = eEntity->local.glock_ammo;
		break;
	case AM_SHELL:
		eEntity->v.primary_ammo = eEntity->local.shotcycler_ammo;
		break;
	case AM_LASER:
		eEntity->v.primary_ammo = eEntity->local.shockwave_ammo;
		break;
	case AM_C4BOMB:
		eEntity->v.primary_ammo = eEntity->local.c4_ammo;
		break;
#elif GAME_ADAMAS
	case AM_BULLET:
		eEntity->v.primary_ammo = eEntity->local.iBulletAmmo;
		break;
#endif
	case AM_MELEE:
	case AM_SWITCH:
	case AM_NONE:
		eEntity->v.primary_ammo = 1;
		break;
	default:
		Engine.Con_Warning("Failed to set primary ammo! (%i)\n", wWeapon->primary_type);

		eEntity->v.primary_ammo = 0;
	}

	switch (wWeapon->iSecondaryType)
	{
	case AM_MELEE:
	case AM_SWITCH:
	case AM_NONE:
		eEntity->v.secondary_ammo = 1;
		break;
	default:
		Engine.Con_Warning("Failed to set secondary ammo! (%i)\n", wWeapon->iSecondaryType);
		eEntity->v.secondary_ammo = 0;
	}
};

/*	Sets an active weapon for the specified entity.
*/
void Weapon_SetActive(Weapon_t *wWeapon,ServerEntity_t *eEntity, bool bDeploy)
{
	bool bPrimaryAmmo,bSecondaryAmmo;

	// Clear everything out.
	eEntity->v.cViewModel		= "";
	eEntity->v.secondary_ammo	=
	eEntity->v.primary_ammo		=
	eEntity->v.iWeaponFrame		= 0;

	bPrimaryAmmo	= Weapon_CheckPrimaryAmmo(wWeapon,eEntity);
	bSecondaryAmmo	= Weapon_CheckSecondaryAmmo(wWeapon,eEntity);
	if(!bPrimaryAmmo && !bSecondaryAmmo)
		return;

	Weapon_UpdateCurrentAmmo(wWeapon, eEntity);

	eEntity->v.iActiveWeapon	= wWeapon->iItem;
	eEntity->v.cViewModel		= wWeapon->model;

	eEntity->local.iFireMode		=
	eEntity->local.iWeaponIdleFrame	= 0;

	if(wWeapon->Deploy && bDeploy)
		wWeapon->Deploy(eEntity);
}

// Checks for primary ammo.
bool Weapon_CheckPrimaryAmmo(Weapon_t *weapon,ServerEntity_t *entity) {
	switch(weapon->primary_type) {
#ifdef GAME_OPENKATANA
	case AM_IONS:
		if(entity->local.ionblaster_ammo)
			return true;
		break;
	case AM_ROCKET:
		if(entity->local.sidewinder_ammo)
			return true;
		break;
	case AM_BULLET:
		if(entity->local.glock_ammo)
			return true;
		break;
	case AM_SHELL:
		if(entity->local.shotcycler_ammo)
			return true;
		break;
	case AM_LASER:
		if(entity->local.shockwave_ammo)
			return true;
		break;
	case AM_C4BOMB:
		if(entity->local.c4_ammo)
			return true;
		break;
#elif GAME_ADAMAS
	case AM_BULLET:
		if(entity->local.iBulletAmmo)
			return true;
		break;
#endif
	case AM_PROJECTILE:
	case AM_MELEE:
	case AM_SWITCH:
	case AM_NONE:
		return true;
	default:
		break;
	}

	return false;
}

/*	Check for secondary ammo.
*/
bool Weapon_CheckSecondaryAmmo(Weapon_t *weapon,ServerEntity_t *entity) {
	switch(weapon->iSecondaryType)
	{
	case AM_MELEE:
	case AM_SWITCH:
	case AM_NONE:
		return true;
	default:
		break;
	}

	return false;
}

/*
	Animation Code
*/

void Weapon_ResetAnimation(ServerEntity_t *ent)
{
	// Allow for custom idle frames after animation.
	if(ent->local.iWeaponIdleFrame)
		ent->v.iWeaponFrame					=
		ent->local.iWeaponAnimationCurrent	= ent->local.iWeaponIdleFrame;
	else
		ent->v.iWeaponFrame					=
		ent->local.iWeaponAnimationCurrent	= 0;

	ent->local.iWeaponAnimationEnd	= 0;
	ent->local.fWeaponAnimationTime = 0;
}

void Weapon_CheckFrames(ServerEntity_t *eEntity)
{
	// If something isn't active and Animationtime is over
	if(!eEntity->local.iWeaponAnimationEnd || Server.time < eEntity->local.fWeaponAnimationTime)
		return;
	// Reset the animation in-case we die!
	else if((eEntity->local.iWeaponAnimationCurrent > eEntity->local.iWeaponAnimationEnd) || (eEntity->v.health <= 0))
	{
		Weapon_ResetAnimation(eEntity);
		return;
	}

	eEntity->v.iWeaponFrame = eEntity->local.iWeaponFrames[eEntity->local.iWeaponAnimationCurrent].frame;

#ifdef GAME_OPENKATANA
	if(eEntity->local.attackb_finished > Server.time)
		eEntity->local.fWeaponAnimationTime = ((float)Server.time)+eEntity->local.iWeaponFrames[eEntity->local.iWeaponAnimationCurrent].speed * 0.2f;
	else
#endif
		eEntity->local.fWeaponAnimationTime = ((float)Server.time)+eEntity->local.iWeaponFrames[eEntity->local.iWeaponAnimationCurrent].speed;

	if(eEntity->local.iWeaponFrames[eEntity->local.iWeaponAnimationCurrent].Event)
		eEntity->local.iWeaponFrames[eEntity->local.iWeaponAnimationCurrent].Event(eEntity);

	eEntity->local.iWeaponAnimationCurrent++;
}

void Weapon_Animate(ServerEntity_t *ent, ServerEntityFrame_t *eFrames)
{
	int i;

	Weapon_ResetAnimation(ent);

	ent->local.iWeaponAnimationCurrent = 0;

	ent->v.iWeaponFrame = eFrames[0].frame;

	for(i = 0;; i++)
		if(eFrames[i].isend)
		{
			ent->local.iWeaponAnimationEnd = i;
			break;
		}

	ent->local.iWeaponFrames = eFrames;

#ifdef GAME_OPENKATANA
	if(ent->local.attackb_finished > Server.time)
		ent->local.fWeaponAnimationTime = ((float)Server.time)+eFrames[0].speed*0.5f;
	else
#endif
		ent->local.fWeaponAnimationTime = ((float)Server.time)+eFrames[0].speed;
}

/*	Cycle through currently avaliable weapons.
*/
void Weapon_Cycle(ServerEntity_t *eEntity, bool bForward)
{
	if(eEntity->local.dAttackFinished > Server.time)
		return;

	Weapon_t *cur_weapon = Weapon_GetCurrentWeapon(eEntity);
	if(!cur_weapon)
		return;

	if(cur_weapon < Weapons || cur_weapon > LAST_WEAPON)
	{
		Engine.Con_Warning("Weapon_Cycle: cur_weapon outside of Weapons array!\n");
		return;
	}

	/* Start at our current weapon... */
	Weapon_t *next_weapon = cur_weapon;

	do
	{
		/* ...move up/down the Weapons array... */
		if(bForward)
		{
			++next_weapon;
		}
		else{
			--next_weapon;
		}

		/* ...loop around if we run off the end... */
		if(next_weapon < Weapons)
		{
			next_weapon = LAST_WEAPON;
		}
		else if(next_weapon > LAST_WEAPON)
		{
			next_weapon = Weapons;
		}

		/* ...get the backing item... */
		Item_t *nw_item = Item_GetItem(next_weapon->iItem);
		if(nw_item == NULL)
		{
			Engine.Con_Warning("Weapon_Cycle: Found weapon with no matching item (iItem = %d)!\n", next_weapon->iItem);
			continue;
		}

		/* ...check the weapon is in our inventory and has some ammo... */
		if(Item_CheckInventory(nw_item, eEntity)
			&& (Weapon_CheckPrimaryAmmo(next_weapon, eEntity) || Weapon_CheckSecondaryAmmo(next_weapon, eEntity)))
		{
			/* ...everything's ok, switch to it! */
			Weapon_SetActive(next_weapon, eEntity, true);
			break;
		}

		/* ...keep looping until we come back to our current weapon... */
	} while(next_weapon != cur_weapon);
}

void Weapon_PrimaryAttack(ServerEntity_t *eEntity)
{
	Weapon_t *wCurrentWeapon = Weapon_GetCurrentWeapon(eEntity);
	if(!wCurrentWeapon || !Weapon_CheckPrimaryAmmo(wCurrentWeapon,eEntity) || eEntity->local.dAttackFinished > Server.time)
		return;

	if(wCurrentWeapon->Primary)
	{
#ifdef GAME_OPENKATANA
		// Daikatana handles this itself, since it's a special case.
		if (wCurrentWeapon->iItem != WEAPON_DAIKATANA)
			// Check to see if it's a player and ensure they're standing.
			if(Entity_IsPlayer(eEntity) && ((eEntity->v.velocity[0] == 0) && (eEntity->v.velocity[1] == 0)))
				// Tell them to run their fire animation.
				Entity_Animate(eEntity,PlayerAnimation_Fire);
#endif

		wCurrentWeapon->Primary(eEntity);
	}
}

void Weapon_SecondaryAttack(ServerEntity_t *eEntity)
{
	Weapon_t *wCurrentWeapon = Weapon_GetCurrentWeapon(eEntity);
	if(!wCurrentWeapon || !Weapon_CheckSecondaryAmmo(wCurrentWeapon,eEntity) || eEntity->local.dAttackFinished > Server.time)
		return;

	if(wCurrentWeapon->Secondary)
	{
		wCurrentWeapon->Secondary(eEntity);
	}
}

void Weapon_CheatCommand(ServerEntity_t *eEntity)
{
#ifdef GAME_OPENKATANA
	Weapon_t *wWeapon;

	eEntity->local.shotcycler_ammo	=
	eEntity->local.ionblaster_ammo	=
	eEntity->local.sidewinder_ammo	=
	eEntity->local.shockwave_ammo	=
	eEntity->local.c4_ammo			= 300;

	Item_AddInventory(Item_GetItem(WEAPON_DAIKATANA),eEntity);
	Item_AddInventory(Item_GetItem(WEAPON_IONRIFLE), eEntity);
	Item_AddInventory(Item_GetItem(WEAPON_C4VIZATERGO),eEntity);
	Item_AddInventory(Item_GetItem(WEAPON_SHOTCYCLER),eEntity);
	Item_AddInventory(Item_GetItem(WEAPON_SIDEWINDER),eEntity);
	Item_AddInventory(Item_GetItem(WEAPON_SHOCKWAVE),eEntity);

	wWeapon = Weapon_GetWeapon(WEAPON_DAIKATANA);
	if(wWeapon)
		Weapon_SetActive(wWeapon, eEntity, true);
#endif

	eEntity->v.impulse = 0;
}

void Weapon_CheckInput(ServerEntity_t *eEntity)
{
	if(eEntity->v.impulse >= 1 && eEntity->v.impulse <= 7)
	{
		int         iNewWeapon = 0;
		Item_t      *iItem;
		Weapon_t	*wWeapon;

		if(eEntity->local.dAttackFinished > Server.time)
			return;

		switch(eEntity->v.impulse)
		{
#ifdef GAME_OPENKATANA
		case 1:
			iNewWeapon = WEAPON_IONRIFLE;
			break;
		case 2:
			iNewWeapon = WEAPON_C4VIZATERGO;
			break;
		case 3:
			iNewWeapon = WEAPON_SHOTCYCLER;
			break;
		case 4:
			iNewWeapon = WEAPON_SIDEWINDER;
			break;
		case 5:
			iNewWeapon = WEAPON_SHOCKWAVE;
			break;
		case 6:
			iNewWeapon = WEAPON_DAIKATANA;
			break;
#endif
		default:
			iNewWeapon = ITEM_WEAPON_NONE;
		}

		// Check our actual inventory!
		iItem = Item_GetInventory(iNewWeapon,eEntity);
		if(iItem)
		{
			// Check our new weapon against our active one.
			if(iItem->iNumber != eEntity->v.iActiveWeapon)
			{
				wWeapon = Weapon_GetWeapon(iItem->iNumber);
				if(!wWeapon)
					Engine.Con_Warning("Failed to get new weapon! (%i)\n",iItem->iNumber);
				else if(!Weapon_CheckPrimaryAmmo(wWeapon,eEntity) && !Weapon_CheckSecondaryAmmo(wWeapon,eEntity))
				{
					Engine.Server_SinglePrint(eEntity,"Not enough ammo.\n");

					// TODO: Change to client-specific sound function.
					Engine.Sound(eEntity,CHAN_AUTO,"misc/deny.wav",255,ATTN_NORM);
				}
				else
					Weapon_SetActive(wWeapon, eEntity, true);
			}
		}
	}
	else if(eEntity->v.impulse == 65)
		Weapon_CheatCommand(eEntity);
	else if(eEntity->v.impulse == 10)
		Weapon_Cycle(eEntity,true);		// Forwards cycling
	else if(eEntity->v.impulse == 12)
		Weapon_Cycle(eEntity,false);	// Backwards cycling

	else if(eEntity->v.impulse == 66)	// TODO: Button?
		Player_Use(eEntity);

	if(eEntity->v.button[0])
		Weapon_PrimaryAttack(eEntity);
	else if(eEntity->v.impulse == 150)	// TODO: Change to a button?
		Weapon_SecondaryAttack(eEntity);

	eEntity->v.impulse = 0;
}
