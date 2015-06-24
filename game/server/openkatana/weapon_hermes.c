#include "server_weapon.h"

void Hermes_Deploy(ServerEntity_t *ent)
{
	//Weapon_Animate(ent,FALSE,1,11,0.1f,0,0,0,FALSE);
}

// [17/7/2012] Renamed to Hermes_CloudThink ~hogsy
void Hermes_CloudThink(ServerEntity_t *ent)
{
	vec3_t vel;

	if(!ent->local.hit)
		ENTITY_REMOVE(ent);

	ent->local.hit -= 2;

	Math_VectorClear(vel);

	Engine.Particle(ent->v.origin,vel,12,"poison",8);

	ent->v.think		= Hermes_CloudThink;
	ent->v.dNextThink	= Server.dTime+0.3;
}

void HermesCloudTouch(ServerEntity_t *ent, ServerEntity_t *other)
{
	// [13/4/2012] Fixed (please remember to use strcmp!) ~hogsy
	// [8/6/2012] Removed classname check for clients ~hogsy
	if(other->Monster.iType != MONSTER_PLAYER)
		return;

	if(other->v.iHealth > 0 && other->v.movetype == MOVETYPE_STEP)
	{
		Entity_Damage(other, ent, 5, 0);
		//other->local.poisoned = 1; TODO: MAKE IT WORK
	}
}

void Hermes_PrimaryAttack(ServerEntity_t *ent)
{
	ServerEntity_t *cloud = Entity_Spawn();

	cloud->v.cClassname = "cloud";
	cloud->v.movetype	= MOVETYPE_FLYMISSILE;
	cloud->Physics.iSolid		= SOLID_TRIGGER;

	cloud->local.hit	= 10;
	cloud->local.eOwner	= ent;

	//SetSize(cloud,-16,-16,-16,16,16,16);
	Entity_SetOrigin(cloud,ent->v.origin);

	Weapon_Projectile(ent, cloud, 100.0f);

	cloud->v.dNextThink		= Server.dTime+0.3;
	cloud->v.think			= Hermes_CloudThink;
	cloud->v.TouchFunction	= HermesCloudTouch;

	Math_VectorAddValue(cloud->v.avelocity,300.0f,cloud->v.avelocity);

	if(ent->local.attackb_finished > Server.dTime)	// No attack boost...
		ent->local.dAttackFinished = Server.dTime+0.35;
	else
		ent->local.dAttackFinished = Server.dTime+0.7;

#if 0
	if(rand()%3 == 1)
		//Weapon_Animate(ent,FALSE,12,17,0.07f,10,19,0,FALSE);
	else if(rand()%3 == 2)
		//Weapon_Animate(ent,FALSE,18,23,0.07f,10,19,0,FALSE);
	else
		//Weapon_Animate(ent,FALSE,24,29,0.07f,10,19,0,FALSE);
#endif
}
