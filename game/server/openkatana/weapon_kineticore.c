#include "server_weapon.h"

void Kineticore_Deploy(ServerEntity_t *ent)
{
	//Weapon_Animate(ent,FALSE,1,14,0.1f,0,0,0,FALSE);
}

void Kineticore_IceballExplode(ServerEntity_t *ent)
{
	PLVector3f vel;

	vel[0] = ent->v.velocity[0]*-1;
	vel[1] = ent->v.velocity[1]*-1;
	vel[2] = ent->v.velocity[2]*-1;

	Engine.Particle(ent->v.origin,vel,5,"ice",17);

	Math_VectorClear(ent->v.velocity);

	Entity_Remove(ent);
}
void IceballTouch(ServerEntity_t *ent, ServerEntity_t *other)
{
	// [26/2/2012] Revised and fixed ~hogsy
	// [25/6/2012] Revised ~hogsy
	PLVector3f vel;

	// [25/6/2012] Simplified ~hogsy
	Math_VectorCopy(ent->v.velocity,vel);
	Math_VectorInverse(vel);

	if(other->v.takedamage)
		Entity_Damage(other, ent, 50, DAMAGE_TYPE_NONE);
	else if (ent->local.hit < 9)
	{
		if(!ent->v.angles[1] || !ent->v.angles[0])
			ent->v.velocity[2] += 200;

		ent->v.angles[2] -= 180;

		ent->local.hit += 1;

		Engine.Particle(ent->v.origin,vel,5,"ice",13);

		return;
	}

	Engine.Particle(ent->v.origin,vel,5,"ice",17);
	Sound(ent,CHAN_ITEM,"weapons/freeze.wav",255,ATTN_NORM);

	// [25/6/2012] Simplified ~hogsy
	Math_VectorClear(ent->v.velocity);

	Entity_Remove(ent);
}

void projectile_iceball(ServerEntity_t *ent, PLVector3f orig)
{
	// [26/2/2012] Revised and fixed ~hogsy
	ServerEntity_t *ionball = Entity_Spawn();

	ionball->v.classname	= "iceball";
	ionball->v.movetype		= MOVETYPE_FLYBOUNCE;

	ionball->Physics.solid	= SOLID_BBOX;

	ionball->local.eOwner = ent;

	Weapon_Projectile(ent, ionball, 2500.0f);

	Entity_SetModel(ionball,"models/iceball.md2");

	Math_MVToVector(plVectorToAngles(ionball->v.velocity), ionball->v.angles);

	Entity_SetSize(ionball,0,0,0,0,0,0);
	Entity_SetOrigin(ionball, orig);

	ionball->v.TouchFunction	= IceballTouch;
	ionball->v.nextthink		= Server.time+3.0;
	ionball->v.think			= Kineticore_IceballExplode;
}

void Kineticore_PrimaryAttack(ServerEntity_t *ent)
{
	if(!ent->local.kineticore_ammo2)
	{
		ent->local.kineticore_ammo2 = 5;
		ent->local.dAttackFinished = Server.time+0.7;
		return;
	}

	//Weapon_Animate(ent,FALSE,27,37,0.3f,10,19,0,FALSE);

	ent->v.iPrimaryAmmo = ent->local.kineticore_ammo--;
	ent->local.kineticore_ammo2--;

	projectile_iceball(ent, ent->v.origin);

	if(ent->local.attackb_finished > Server.time)	// No attack boost...
		ent->local.dAttackFinished = Server.time+0.1;
	else
		ent->local.dAttackFinished = Server.time+0.25;
}
