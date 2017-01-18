#include "server_main.h"

#include "server_weapon.h"

void GreekFire_Deploy(ServerEntity_t *ent)
{
	//Weapon_Animate(ent,FALSE,1,7,0.1f,0,0,0,FALSE);
}

void GreekfireTouch(ServerEntity_t *ent, ServerEntity_t *other)
{
	PLVector3f vel;

	if (other == ent->local.eOwner)
		return;

	if(other->v.takedamage)
		Entity_Damage(other, ent, 50, 0);

	Math_VectorCopy(ent->v.velocity,vel);
	Math_VectorInverse(vel);

	Engine.Particle(ent->v.origin,vel,5,"spark",17);

	Math_VectorClear(ent->v.velocity);

	Entity_Remove(ent);
}

void GreekFire_Throw(ServerEntity_t *ent)
{
	ServerEntity_t *greekfire = Entity_Spawn();

	greekfire->v.classname	= "greekfire";
	greekfire->v.movetype	= MOVETYPE_BOUNCE;
	greekfire->v.effects	= EF_DIMLIGHT;

	greekfire->Physics.solid = SOLID_BBOX;

	greekfire->local.eOwner = ent;

	Weapon_Projectile(ent,greekfire,800.0f);

	Entity_SetModel(greekfire,"models/w_greekfire.md2");
	Math_MVToVector(plVectorToAngles(greekfire->v.velocity), greekfire->v.angles);

	// Use SetOrigin since it automatically links.
	Entity_SetOrigin(greekfire, ent->v.origin);

	Entity_SetSizeVector(greekfire, pl_origin3f, pl_origin3f);

	greekfire->v.TouchFunction = GreekfireTouch;
}

void GreekFire_PrimaryAttack(ServerEntity_t *ent)
{
#if 0
	//Weapon_Animate(ent,FALSE,8,25,0.08f,0,0,0,FALSE);

	ent->v.iPrimaryAmmo = ent->local.iGreekFireAmmo--;

	if(ent->local.attackb_finished > Server.time)	// No attack boost...
		ent->local.dAttackFinished = Server.time+0.75;
	else
		ent->local.dAttackFinished = Server.time+1.5;
#endif
}
