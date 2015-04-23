#include "server_weapon.h"

/*
	Base code for the weapon system.
*/

#include "server_item.h"
#include "server_player.h"

Weapon_t Weapons[] =
{
	{
		WEAPON_NONE
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
		WEAPON_IONBLASTER,
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

	// Secret Weapons
	{
		WEAPON_IONRIFLE,
		"models/weapons/v_ionrifle.md2",	// TEMP
		IonRifle_Deploy,

		// Primary
		AM_IONS,
		IonRifle_PrimaryAttack,

		// Secondary
		AM_NONE,
		IonRifle_SecondaryAttack
	},

	// Monster Weapons
	{
		WEAPON_LASERS,
		NULL,
		NULL,

		// Primary
		AM_LASER,
		IonBlaster_PrimaryAttack	// [31/3/2013] Temporary ~hogsy
	},

	// Unused Weapons
	{
		WEAPON_GLOCK,
		"models/weapons/v_glock.md2",
		Glock_Deploy,

		// Primary
		AM_BULLET,
		Glock_PrimaryAttack
	},
#elif GAME_ADAMAS
	{
		WEAPON_BLAZER,
		BLAZER_MODEL_VIEW,
		Blazer_Deploy,

		// Primary
		AM_BULLET,
		Blazer_PrimaryAttack
	},
#endif

	{	0,	NULL,	NULL,	AM_NONE, NULL,	AM_NONE,	NULL	}
};

/*	Returns the entities current active weapon.
*/
Weapon_t *Weapon_GetCurrentWeapon(edict_t *eEntity)
{
	Weapon_t	*wCurrentWeapon;

	for(wCurrentWeapon = Weapons; wCurrentWeapon->iItem; wCurrentWeapon++)
		if(eEntity->v.iActiveWeapon == wCurrentWeapon->iItem)
			return wCurrentWeapon;

	// [20/5/2013] Changed to WEAPON_NONE ~hogsy
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
#ifdef OPENKATANA
	// [11/5/2013] Model precaches ~eukos
	Engine.Server_PrecacheResource(RESOURCE_MODEL,DAIKATANA_MODEL_VIEW);
	Engine.Server_PrecacheResource(RESOURCE_MODEL,"models/weapons/v_ionblaster.md2");
	Engine.Server_PrecacheResource(RESOURCE_MODEL,"models/weapons/v_c4.md2");
	Engine.Server_PrecacheResource(RESOURCE_MODEL,"models/weapons/v_shotcycler.md2");
	Engine.Server_PrecacheResource(RESOURCE_MODEL,"models/weapons/v_sidewinder.md2");
	Engine.Server_PrecacheResource(RESOURCE_MODEL,"models/weapons/v_shockwave.md2");
	// TEMP START
	Engine.Server_PrecacheResource(RESOURCE_MODEL,"models/weapons/w_ionblaster.md2");
	Engine.Server_PrecacheResource(RESOURCE_MODEL,"models/weapons/w_c4.md2");
	Engine.Server_PrecacheResource(RESOURCE_MODEL,"models/weapons/w_shotcycler.md2");
	Engine.Server_PrecacheResource(RESOURCE_MODEL,"models/weapons/w_sidewinder.md2");
	Engine.Server_PrecacheResource(RESOURCE_MODEL,"models/weapons/w_shockwave.md2");
	// TEMP END
	Engine.Server_PrecacheResource(RESOURCE_MODEL,"models/slaser.md2");
	Engine.Server_PrecacheResource(RESOURCE_MODEL,"models/c4ammo.md2");
	Engine.Server_PrecacheResource(RESOURCE_MODEL,"models/ionball.md2");
	Engine.Server_PrecacheResource(RESOURCE_MODEL,"models/sidewinder_missile.md2");

	// Ion Rifle
	Engine.Server_PrecacheResource(RESOURCE_MODEL,"models/weapons/v_ionrifle.md2");
	Server_PrecacheSound("weapons/laser.wav");

	// C4
	Server_PrecacheSound("weapons/c4/c4beep.wav");
	Server_PrecacheSound("weapons/c4/c4cock.wav");
	Server_PrecacheSound("weapons/c4/c4fire.wav");
	Server_PrecacheSound("weapons/c4/c4stick.wav");

	// Ion Blaster
	Server_PrecacheSound("weapons/ionblaster/explode.wav");
	Server_PrecacheSound("weapons/ionblaster/ionfire.wav");
	Server_PrecacheSound("weapons/ionblaster/ionflyby.wav");
	Server_PrecacheSound("weapons/ionblaster/ionready.wav");
	Server_PrecacheSound("weapons/ionblaster/bounce.wav");
	Engine.Server_PrecacheResource(RESOURCE_MODEL,"models/ionball.md2");

	// Shockwave
	Server_PrecacheSound("weapons/shockwave/fade.wav");
	Server_PrecacheSound("weapons/shockwave/fire.wav");
	Server_PrecacheSound("weapons/shockwave/ready.wav");
	Server_PrecacheSound("weapons/shockwave/warmup.wav");

	Server_PrecacheSound("weapons/shotcycler6/shotcyclerfire1.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotcyclerfire2.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotcyclerfire3.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotcyclerfire4.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotcyclerfire5.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotcyclerfire6.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotcyclerfire7.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotcyclerfire8.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotcyclerfire9.wav");
	Server_PrecacheSound("weapons/shotcycler6/barrelspin.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotcyclerready.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotcyclerclick.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotclick.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotpull.wav");
	Server_PrecacheSound("weapons/shotcycler6/shotslap.wav");

	// Sidewinder
	Server_PrecacheSound("weapons/sidewinder/sidewinderfire.wav");
	Server_PrecacheSound("weapons/sidewinder/sidewinderflyby.wav");
	Server_PrecacheSound("weapons/sidewinder/sidewindersplash.wav");
	Server_PrecacheSound("weapons/sidewinder/sidewinderunderwaterfire.wav");
	Server_PrecacheSound("weapons/sidewinder/sidewinderunderwaterflyby.wav");
#elif GAME_ADAMAS
	Engine.Server_PrecacheResource(RESOURCE_MODEL,BLAZER_MODEL_VIEW);
#endif
}

/*	Enable automatic aim assitance.
*/
MathVector_t Weapon_Aim(edict_t *eEntity)
{
	int				i, j;
	float			fDistance, fBestDistance;
	trace_t			tAimLine;
	edict_t			*eCheck, *eBest;
	MathVector3f_t	mvStart, mvEnd, mvDirection, mvBestDirection;

	// Copy the entity's origin as our starting position.
	Math_VectorCopy(eEntity->v.origin, mvStart);

	// Move the starting position up a little.
	mvStart[2] += 20.0f;

	Entity_MakeVectors(eEntity);

	// Try tracing straight.
	Math_VectorCopy(eEntity->local.vForward, mvDirection);
	Math_VectorMA(mvStart, 2048.0f, mvDirection, mvEnd);

	tAimLine = Engine.Server_Move(mvStart, mv3Origin, mv3Origin, mvEnd, 0, eEntity);
	// See if we encountered anything we can damage.
	if (tAimLine.ent && tAimLine.ent->v.bTakeDamage)
	{
		// Check the teams.
		if ((eEntity->local.pTeam <= TEAM_NEUTRAL) || (eEntity->local.pTeam != tAimLine.ent->local.pTeam))
		{
			MathVector_t mvResult;

			// Convert to the appropriate vector type.
			Math_VectorToMV(eEntity->local.vForward, mvResult);

			// Return the result.
			return mvResult;
		}
	}

	// Try every possible entity.
	Math_VectorCopy(mvDirection, mvBestDirection);
	fBestDistance = cvServerAim.value;
	eBest = NULL;

	eCheck = NEXT_EDICT(Engine.Server_GetEdicts());
	for (i = 1; i < Engine.Server_GetNumEdicts(); i++, eCheck = NEXT_EDICT(eCheck))
	{
		if (eCheck == eEntity)
			continue;

		if (eCheck->v.bTakeDamage == false)
			continue;

		for (j = 0; j < 3; j++)
			mvEnd[j] = eCheck->v.origin[j] + 0.5f * (eCheck->v.mins[j] + eCheck->v.maxs[j]);

		Math_VectorSubtract(mvEnd, mvStart, mvDirection);
		Math_VectorNormalize(mvDirection);

		fDistance = Math_DotProduct(mvDirection, eEntity->local.vForward);
		if (fDistance < fBestDistance)
			// Too far to turn.
			continue;

		tAimLine = Engine.Server_Move(mvStart, mv3Origin, mv3Origin, mvEnd, 0, eEntity);
		if (tAimLine.ent == eCheck)
		{
			// Can shoot at this one.
			fBestDistance = fDistance;
			eBest = eCheck;
		}
	}

	MathVector_t mvResult;

	if (eBest)
	{
		Math_VectorSubtract(eBest->v.origin, eEntity->v.origin, mvDirection);

		fDistance = Math_DotProduct(mvDirection, eEntity->local.vForward);

		Math_VectorScale(eEntity->local.vForward, fDistance, mvEnd);

		mvEnd[2] = mvDirection[2];

		Math_VectorNormalize(mvEnd);
		Math_VectorToMV(mvEnd, mvResult);
	}
	else
		Math_VectorToMV(mvBestDirection, mvResult);

	return mvResult;
}

MathVector3_t mvTraceMaxs = { 8, 8, 8 };
MathVector3_t mvTraceMins = { 8, 8, 8 };

/*	Runs a trace to see if a projectile can be casted.
*/
bool Weapon_CheckTrace(ServerEntity_t *eOwner)
{
	int i, iTraceContents;
	MathVector3f_t mvTarget, mvSource, mvDirection;
	trace_t tCheck;

	// Copy over the players origin.
	Math_VectorCopy(eOwner->v.origin, mvSource);

	// Update the origin to the correct view offset.
	mvSource[2] += eOwner->v.view_ofs[2];

	// Apply the distance to the target for the trace.
	for (i = 0; i < 3; i++)
		mvTarget[i] = mvSource[i] + mvDirection[i] * 2048.0f;

	// Check that there's enough space for projectile.
	tCheck = Traceline(eOwner, mvSource, mvTarget, 0);
	if (tCheck.fraction == 1.0f)
		return false;

	// Ensure that we're not inside the sky or within a solid.
	iTraceContents = Engine.Server_PointContents(tCheck.endpos);
	if ((iTraceContents == BSP_CONTENTS_SKY) || (iTraceContents == BSP_CONTENTS_SOLID))
		return false;

	// Check to see if there's a target, and it's not the world!
	if (tCheck.ent)
		// Are we intersecting with it?
		if (Math_IsIntersecting(mvTraceMins, mvTraceMaxs, tCheck.ent->v.mins, tCheck.ent->v.maxs))
			return false;

	return true;
}

/*	Attempts to throw out a projectile.
*/
void Weapon_Projectile(edict_t *eOwner, edict_t *eProjectile, float fVelocity)
{
	MathVector3_t mvDirection;

	// Figure out our aim direction.
	Math_MVToVector(Weapon_Aim(eOwner), mvDirection);

	// Scale with the speed and copy over the angled velocity.
	Math_VectorScale(mvDirection, fVelocity, eProjectile->v.velocity);
}

void Weapon_BulletProjectile(edict_t *eEntity,float fSpread,int iDamage,vec_t *vDirection)
{
	int	i;
	MathVector3_t vSource, vTarg;
	trace_t	tTrace;

	Math_VectorCopy(eEntity->v.origin,vSource);

	vSource[2] += eEntity->v.view_ofs[2];

	for(i = 0; i < 3; i++)
		vTarg[i] = vSource[i]+(vDirection[i]*2048.0f)+(fSpread*Math_CRandom()*20.0f);

	tTrace = Traceline(eEntity,vSource,vTarg,0);
	if ((tTrace.fraction == 1.0f) || (Engine.Server_PointContents(tTrace.endpos) == BSP_CONTENTS_SKY))
		return;
	else
	{
		char	cSmoke[6];

		if(tTrace.ent && tTrace.ent->v.bTakeDamage)
			MONSTER_Damage(tTrace.ent,eEntity,iDamage,DAMAGE_TYPE_NONE);
		else
		{
			edict_t *eSmoke = Entity_Spawn();
			if(eSmoke)
			{
				char cSound[32];

				eSmoke->v.think			= Entity_Remove;
				eSmoke->v.dNextThink	= Server.dTime+0.5;

				Entity_SetOrigin(eSmoke,tTrace.endpos);

				PHYSICS_SOUND_RICOCHET(cSound);

				Sound(eSmoke,CHAN_BODY,cSound,255,ATTN_NORM);
			}

			PARTICLE_SMOKE(cSmoke);

			Engine.Particle(tTrace.endpos,mv3Origin,15,cSmoke,15);
		}
	}
}

void Weapon_UpdateCurrentAmmo(Weapon_t *wWeapon, edict_t *eEntity)
{
	// [4/7/2012] Set ammo by type ~hogsy
	switch (wWeapon->iPrimaryType)
	{
#ifdef OPENKATANA
		// [12/8/2012] Added in AM_IONS ~hogsy
	case AM_IONS:
		eEntity->v.iPrimaryAmmo = eEntity->local.ionblaster_ammo;
		break;
	case AM_ROCKET:
		eEntity->v.iPrimaryAmmo = eEntity->local.sidewinder_ammo;
		break;
	case AM_DISCUS:
		eEntity->v.iPrimaryAmmo = eEntity->local.discus_ammo;
		break;
	case AM_BULLET:
		eEntity->v.iPrimaryAmmo = eEntity->local.glock_ammo;
		break;
	case AM_SHELL:
		eEntity->v.iPrimaryAmmo = eEntity->local.shotcycler_ammo;
		break;
	case AM_LASER:
		eEntity->v.iPrimaryAmmo = eEntity->local.shockwave_ammo;
		break;
	case AM_C4BOMB:
		eEntity->v.iPrimaryAmmo = eEntity->local.iC4Ammo;
		break;
#elif GAME_ADAMAS
	case AM_BULLET:
		eEntity->v.iPrimaryAmmo = eEntity->local.iBulletAmmo;
		break;
#endif
	case AM_MELEE:
	case AM_SWITCH:
	case AM_NONE:
		eEntity->v.iPrimaryAmmo = 1;
		break;
	default:
		Engine.Con_Warning("Failed to set primary ammo! (%i)\n", wWeapon->iPrimaryType);

		eEntity->v.iPrimaryAmmo = 0;
		}

	// [4/7/2012] Set ammo by type ~hogsy
	switch (wWeapon->iSecondaryType)
	{
	case AM_MELEE:
	case AM_SWITCH:
	case AM_NONE:
		eEntity->v.iSecondaryAmmo = 1;
		break;
	default:
		Engine.Con_Warning("Failed to set secondary ammo! (%i)\n", wWeapon->iSecondaryType);
		eEntity->v.iSecondaryAmmo = 0;
	}
};

/*	Sets an active weapon for the specified entity.
*/
void Weapon_SetActive(Weapon_t *wWeapon,edict_t *eEntity, bool bDeploy)
{
	bool	bPrimaryAmmo,bSecondaryAmmo;

	// [11/5/2013] Clear everything out ~hogsy
	eEntity->v.cViewModel		= "";
	eEntity->v.iSecondaryAmmo	=
	eEntity->v.iPrimaryAmmo		=
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

/*	Checks for primary ammo.
*/
bool Weapon_CheckPrimaryAmmo(Weapon_t *wWeapon,edict_t *eEntity)
{
	switch(wWeapon->iPrimaryType)
	{
#ifdef OPENKATANA
	// [12/8/2012] Added in AM_IONS ~hogsy
	case AM_IONS:
		if(eEntity->local.ionblaster_ammo)
			return true;
		break;
	case AM_ROCKET:
		if(eEntity->local.sidewinder_ammo)
			return true;
		break;
	case AM_DISCUS:
		if(eEntity->local.discus_ammo)
			return true;
		break;
	case AM_BULLET:
		if(eEntity->local.glock_ammo)
			return true;
		break;
	case AM_SHELL:
		if(eEntity->local.shotcycler_ammo)
			return true;
		break;
	case AM_LASER:
		if(eEntity->local.shockwave_ammo)
			return true;
		break;
	case AM_C4BOMB:
		if(eEntity->local.iC4Ammo)
			return true;
		break;
#elif GAME_ADAMAS
	case AM_BULLET:
		if(eEntity->local.iBulletAmmo)
			return true;
		break;
#endif
    case AM_PROJECTILE:
	case AM_MELEE:
	case AM_SWITCH:
	case AM_NONE:
		return true;
	}

	return false;
}

/*	Check for secondary ammo.
*/
bool Weapon_CheckSecondaryAmmo(Weapon_t *wWeapon,edict_t *eEntity)
{
	switch(wWeapon->iSecondaryType)
	{
	case AM_MELEE:
	case AM_SWITCH:
	case AM_NONE:
		return true;
	// [12/8/2013] Added to get rid of a silly warning in GCC ~hogsy
	default:
		return false;
	}
}

/*
	Animation Code
*/

// 06/02/2013 - Animation system = redone! ~eukos
void Weapon_ResetAnimation(edict_t *ent)
{
	// [3/2/2014] Allow for custom idle frames after animation ~hogsy
	if(ent->local.iWeaponIdleFrame)
		ent->v.iWeaponFrame					=
		ent->local.iWeaponAnimationCurrent	= ent->local.iWeaponIdleFrame;
	else
		ent->v.iWeaponFrame					=
		ent->local.iWeaponAnimationCurrent	= 0;

	ent->local.iWeaponAnimationEnd = 0;

	ent->local.fWeaponAnimationTime = 0;
}

void Weapon_CheckFrames(edict_t *eEntity)
{
	if(!eEntity->local.iWeaponAnimationEnd || Server.dTime < eEntity->local.fWeaponAnimationTime)	// If something isn't active and Animationtime is over
		return;
	// [2/10/2013] Reset the animation in-case we die! ~hogsy
	else if((eEntity->local.iWeaponAnimationCurrent > eEntity->local.iWeaponAnimationEnd) || (eEntity->v.iHealth <= 0))
	{
		Weapon_ResetAnimation(eEntity);
		return;
	}

	eEntity->v.iWeaponFrame = eEntity->local.iWeaponFrames[eEntity->local.iWeaponAnimationCurrent].iFrame;

#ifdef GAME_OPENKATANA
	if(eEntity->local.attackb_finished > Server.dTime)
		eEntity->local.fWeaponAnimationTime = ((float)Server.dTime)+eEntity->local.iWeaponFrames[eEntity->local.iWeaponAnimationCurrent].fSpeed * 0.2f;
	else
#endif
		eEntity->local.fWeaponAnimationTime = ((float)Server.dTime)+eEntity->local.iWeaponFrames[eEntity->local.iWeaponAnimationCurrent].fSpeed;

	if(eEntity->local.iWeaponFrames[eEntity->local.iWeaponAnimationCurrent].Function)
		eEntity->local.iWeaponFrames[eEntity->local.iWeaponAnimationCurrent].Function(eEntity);

	eEntity->local.iWeaponAnimationCurrent++;
}

void Weapon_Animate(edict_t *ent,EntityFrame_t *eFrames)
{
	int i;

	Weapon_ResetAnimation(ent);

	ent->local.iWeaponAnimationCurrent = 0;

	ent->v.iWeaponFrame = eFrames[0].iFrame;

	for(i = 0;; i++)
		if(eFrames[i].bIsEnd)
		{
			ent->local.iWeaponAnimationEnd = i;
			break;
		}

	ent->local.iWeaponFrames = eFrames;

#ifdef GAME_OPENKATANA
	if(ent->local.attackb_finished > Server.dTime)
		ent->local.fWeaponAnimationTime = ((float)Server.dTime)+eFrames[0].fSpeed*0.5f;
	else
#endif
		ent->local.fWeaponAnimationTime = ((float)Server.dTime)+eFrames[0].fSpeed;
}

/**/

// [19/8/2012] TODO: Finish ~hogsy
/*	Cycle through currently avaliable weapons.
*/
void Weapon_Cycle(edict_t *eEntity,bool bForward)
{
	Weapon_t	*wCurrentWeapon,*wNext;

	if(eEntity->local.dAttackFinished > Server.dTime || !eEntity->v.iActiveWeapon)
		return;

	eEntity->v.impulse = 0;

	wCurrentWeapon = Weapon_GetCurrentWeapon(eEntity);
	if(!wCurrentWeapon)
		return;

	if(bForward)
	{
	NEXTWEAPON:
		// [19/8/2012] Move up to our next slot... ~hogsy
		for(wNext = Weapons; wNext->iItem; wNext++)
			if(wNext > wCurrentWeapon)
			{
				eEntity->v.iActiveWeapon = wNext->iItem;
				break;
			}

		if(eEntity->v.items & eEntity->v.iActiveWeapon)
		{
			if(!Weapon_CheckPrimaryAmmo(wNext,eEntity) && !Weapon_CheckSecondaryAmmo(wNext,eEntity))
				goto NEXTWEAPON;

			Weapon_SetActive(wNext, eEntity, true);

			eEntity->local.dAttackFinished = Server.dTime+0.3;
		}
	}
	else
	{}	// [1/1/2013] Previous etc blah fa fakf ~hogsy
}

void Weapon_PrimaryAttack(edict_t *eEntity)
{
	Weapon_t *wCurrentWeapon = Weapon_GetCurrentWeapon(eEntity);
	if(!wCurrentWeapon || !Weapon_CheckPrimaryAmmo(wCurrentWeapon,eEntity) || eEntity->local.dAttackFinished > Server.dTime)
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

void Weapon_SecondaryAttack(edict_t *eEntity)
{
	Weapon_t *wCurrentWeapon = Weapon_GetCurrentWeapon(eEntity);
	if(!wCurrentWeapon || !Weapon_CheckSecondaryAmmo(wCurrentWeapon,eEntity) || eEntity->local.dAttackFinished > Server.dTime)
		return;

	if(wCurrentWeapon->Secondary)
	{
		wCurrentWeapon->Secondary(eEntity);
	}
}

void Weapon_CheatCommand(edict_t *eEntity)
{
#ifdef GAME_OPENKATANA
	Weapon_t *wWeapon;

	eEntity->local.shotcycler_ammo	=
	eEntity->local.ionblaster_ammo	=
	eEntity->local.sidewinder_ammo	=
	eEntity->local.shockwave_ammo	=
	eEntity->local.iC4Ammo			= 300;

	Item_AddInventory(Item_GetItem(WEAPON_DAIKATANA),eEntity);
	Item_AddInventory(Item_GetItem(WEAPON_IONBLASTER),eEntity);
	Item_AddInventory(Item_GetItem(WEAPON_C4VIZATERGO),eEntity);
	Item_AddInventory(Item_GetItem(WEAPON_SHOTCYCLER),eEntity);
	Item_AddInventory(Item_GetItem(WEAPON_SIDEWINDER),eEntity);
	Item_AddInventory(Item_GetItem(WEAPON_SHOCKWAVE),eEntity);
	Item_AddInventory(Item_GetItem(WEAPON_IONRIFLE),eEntity);
#if 0
	Item_AddInventory(Item_GetItem(WEAPON_GLOCK),eEntity);
#endif

	wWeapon = Weapon_GetWeapon(WEAPON_DAIKATANA);
	if(wWeapon)
		Weapon_SetActive(wWeapon, eEntity, true);
#endif

	eEntity->v.impulse = 0;
}

void Weapon_CheckInput(edict_t *eEntity)
{
	if(eEntity->v.impulse >= 1 && eEntity->v.impulse <= 7)
	{
		int         iNewWeapon = 0;
		Item_t      *iItem;
		Weapon_t	*wWeapon;

		if(eEntity->local.dAttackFinished > Server.dTime)
			return;

		switch(eEntity->v.impulse)
		{
#ifdef GAME_OPENKATANA
		case 1:
			iNewWeapon = WEAPON_IONBLASTER;
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
		case 7:
			iNewWeapon = WEAPON_IONRIFLE;
			break;
#endif
		default:
			iNewWeapon = WEAPON_NONE;
		}

		// [29/7/2013] Check our actual inventory! ~hogsy
		iItem = Item_GetInventory(iNewWeapon,eEntity);
		if(iItem)
		{
			// [11/5/2013] Check our new weapon against our active one ~hogsy
			if(iItem->iNumber != eEntity->v.iActiveWeapon)
			{
				wWeapon = Weapon_GetWeapon(iItem->iNumber);
				if(!wWeapon)
					Engine.Con_Warning("Failed to get new weapon! (%i)\n",iItem->iNumber);
				else if(!Weapon_CheckPrimaryAmmo(wWeapon,eEntity) && !Weapon_CheckSecondaryAmmo(wWeapon,eEntity))
				{
					Engine.Server_SinglePrint(eEntity,"Not enough ammo.\n");

					// [27/1/2013] TODO: Change to client-specific sound function ~hogsy
					Engine.Sound(eEntity,CHAN_AUTO,"misc/deny.wav",255,ATTN_NORM);
				}
				else
					Weapon_SetActive(wWeapon, eEntity, true);
			}
		}
	}
	else if(eEntity->v.impulse == 65)
		Weapon_CheatCommand(eEntity);
#if 0	// TODO: undo this once cycling is implemented, otherwise it has the potential to break things.
	else if(eEntity->v.impulse == 10)
		Weapon_Cycle(eEntity,true);		// Forwards cycling
	else if(eEntity->v.impulse == 12)
		Weapon_Cycle(eEntity,false);	// Backwards cycling
#endif
	else if(eEntity->v.impulse == 66)
		Player_Use(eEntity);

	if(eEntity->v.button[0])
		Weapon_PrimaryAttack(eEntity);
	// [28/7/2012] Added secondary attack ~hogsy
	else if(eEntity->v.impulse == 150)
		Weapon_SecondaryAttack(eEntity);

	eEntity->v.impulse = 0;
}
