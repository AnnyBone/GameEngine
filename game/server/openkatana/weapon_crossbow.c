#include "server_weapon.h"

void Crossbow_Deploy(ServerEntity_t *ent)
{
	//Weapon_Animate(ent,FALSE,53,64,0.1f,0,0,0,FALSE);
}

void arrow_touch(ServerEntity_t *ent, ServerEntity_t *other)
{
	// [25/6/2012] Cleaned up ~hogsy
	// [7/4/2012] Cleaned up ~hogsy
	char	snd[64];

	if(other == ent->local.eOwner)
		return;

	// We hit an enemy! Stick with 'em
	if(other->v.takedamage)
	{
		// Fleshy sound plz
		sprintf(snd,"weapons/crossbow/arrowwetimpact%i.wav",rand()%5+1);

		Entity_Damage(other, ent, 25, 0);

		ent->v.think		= WEAPON_StickThink;
		ent->v.nextthink	= Server.time+0.1;
	}
	// [25/6/2012] Moved so we don't set this before checking what we're hitting ~hogsy
	else
		sprintf(snd,"weapons/crossbow/arrowimpact%i.wav",rand()%5+1);

	Sound(ent,CHAN_ITEM,snd,255,ATTN_NORM);

	ent->v.velocity[0] = ent->v.velocity[1] = ent->v.velocity[2] = 0;
	ent->v.avelocity[0] = ent->v.avelocity[1] = ent->v.avelocity[2] = 0;
	ent->v.enemy = other;
}

void Crossbow_Projectile(ServerEntity_t *ent)
{
	// [11/2/2012] Revised and fixed ~hogsy
	MathVector3f_t mvDirection;
	ServerEntity_t *eArrow;

	eArrow = Entity_Spawn();
	if(eArrow)
	{
		eArrow->local.eOwner = ent;

		eArrow->v.movetype	= MOVETYPE_FLY;

		eArrow->Physics.solid	= SOLID_BBOX;

		Math_MVToVector(Weapon_Aim(ent), mvDirection);
		Math_VectorScale(mvDirection, 2000.0f, eArrow->v.velocity);

		Entity_SetModel(eArrow,"models/arrow.md2");
		Entity_SetSizeVector(eArrow, pl_origin3f, pl_origin3f);

		// [25/6/2012] Simplified ~hogsy
		Math_VectorCopy(ent->v.origin,eArrow->v.origin);
		eArrow->v.origin[2] += 15.0f;

		Math_MVToVector(plVectorToAngles(ent->v.velocity), ent->v.angles);

		eArrow->v.TouchFunction = arrow_touch;
	}
}

void Crossbow_PrimaryAttack(ServerEntity_t *ent)
{
	if (ent->v.waterlevel >= 2)
		Sound(ent,CHAN_WEAPON,"weapons/crossbow/cbwaterfire.wav",255,ATTN_NORM);
	else
		Sound(ent,CHAN_WEAPON,"weapons/crossbow/cbfire.wav",255,ATTN_NORM);

	ent->v.punchangle[0] -= 5.0f;
	ent->v.iPrimaryAmmo = ent->local.iCrossbowAmmo -= 1;

	//Weapon_Animate(ent,FALSE,1,52,0.043f,10,19,0,FALSE);

	Crossbow_Projectile(ent);

	if(ent->local.attackb_finished > Server.time)	// No attack boost...
		ent->local.dAttackFinished = Server.time+1.0;
	else
		ent->local.dAttackFinished = Server.time+2.0;
}
