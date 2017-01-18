#include "server_main.h"

#include "server_weapon.h"

void Discus_Deploy(ServerEntity_t *ent)
{
	//Weapon_Animate(ent,FALSE,1,10,0.1f,0,0,0,FALSE);
}

void Discus_Catch(ServerEntity_t *ent, ServerEntity_t *discus)
{
	// [4/7/2012] Simplified ~hogsy
	ent->v.iPrimaryAmmo = ent->local.discus_ammo2++;

	Sound(ent,CHAN_WEAPON,"weapons/discus/discuscatch.wav",255,ATTN_NORM);

	Entity_Remove(discus);
}

void Discus_Follow(ServerEntity_t *ent)
{
	PLVector3f	vtemp;

	// [23/5/2012] Quick fix for possible issue ~hogsy
	if(!ent || !ent->v.enemy)
		return;

	vtemp[0] = ent->v.enemy->v.origin[0]-ent->v.origin[0];
	vtemp[1] = ent->v.enemy->v.origin[1]-ent->v.origin[1];
	vtemp[2] = ent->v.enemy->v.origin[2]+22.0f-ent->v.origin[2];

	plVectorNormalize(vtemp);
	ent->v.velocity[0] = vtemp[0]*3000.0f;
	ent->v.velocity[1] = vtemp[1]*3000.0f;
	ent->v.velocity[2] = vtemp[2]*3000.0f;

	// [25/6/2012] Simplified ~hogsy
	Math_MVToVector(plVectorToAngles(ent->v.velocity), ent->v.angles);

	ent->v.think		= Discus_Follow;
	ent->v.dNextThink	= Server.time+0.01;
}

// [4/7/2012] Renamed to Discus_ProjectileTouch ~hogsy
void Discus_ProjectileTouch(ServerEntity_t *ent,ServerEntity_t *other)
{
	char	snd[64];
	PLVector3f	vel;

	// [4/8/2012] Updated to use owner ~hogsy
	if(other == ent->local.eOwner)
	{
		// Don't touch the owner while being thrown
		if(ent->local.hit == 0)
			return;

		Discus_Catch(other,ent);
		return;
	}

	if(ent->local.hit == 2)
		ent->local.hit = 1;
	else if(ent->local.hit == 1)
		ent->local.hit = 2;
	else
		ent->local.hit = 1;

	if(other->v.bTakeDamage)
	{
		Entity_Damage(other, ent, 35, 0);

		Sound(ent,CHAN_BODY,"weapons/discus/discushit.wav",255,ATTN_NORM);
		return;
	}

	// [23/5/2012] Cleaned up ~hogsy
	sprintf(snd,"weapons/discus/discusclang%i.wav",rand()%5+1);
	Sound(ent,CHAN_BODY,snd,255,ATTN_NORM);

	vel[0] = vel[1] = 0;
	vel[2] = 5.0f;
	Engine.Particle(ent->v.origin,vel,4.0f,"smoke",15);

	// [4/7/2012] Simplified ~hogsy
	ent->v.angles[2] -= 180.0f;

	if(ent->local.hit == 1)
	{
		ent->v.think		= Discus_Follow;
		ent->v.dNextThink	= Server.time+0.01;
	}
	else
		ent->v.think		= NULL;
}

// [4/7/2012] Renamed to Discus_SpawnProjectile ~hogsy
void Discus_SpawnProjectile(ServerEntity_t *ent,PLVector3f org)
{
	ServerEntity_t *eDiscus;
	MathVector3f_t mvDirection;

	eDiscus = Entity_Spawn();

	eDiscus->v.cClassname = "discus";
	eDiscus->v.movetype = MOVETYPE_FLYBOUNCE;
	eDiscus->Physics.solid = SOLID_BBOX;
	eDiscus->v.effects = EF_MOTION_ROTATE;
	eDiscus->v.enemy = ent;

	// [4/8/2012] Updated to use owner instead ~hogsy
	eDiscus->local.eOwner = ent;

	eDiscus->local.hit = 0;

	Math_MVToVector(Weapon_Aim(ent), mvDirection);
	Math_VectorScale(mvDirection, 700.0f, eDiscus->v.velocity);

	Math_MVToVector(plVectorToAngles(eDiscus->v.velocity), eDiscus->v.angles);

	eDiscus->v.TouchFunction = Discus_ProjectileTouch;

	Entity_SetModel(eDiscus, "models/w_daedalus.md2");
	Entity_SetSizeVector(eDiscus, pl_origin3f, pl_origin3f);
	Entity_SetOrigin(eDiscus, org);

	Sound(ent,CHAN_WEAPON,"weapons/discus/discusthrow.wav",255,ATTN_NORM);
}

// [4/7/2012] Renamed to Discus_PrimaryAttack ~hogsy
void Discus_PrimaryAttack(ServerEntity_t *ent)
{
	//Weapon_Animate(ent,FALSE,8,24,0.035f,0,0,0,FALSE);

	Discus_SpawnProjectile(ent,ent->v.origin);

	// [4/7/2012] Simplified ~hogsy
	ent->v.iPrimaryAmmo = ent->local.discus_ammo2--;

	if(ent->local.attackb_finished > Server.time)	// No attack boost...
		ent->local.dAttackFinished = Server.time+0.35;
	else
		ent->local.dAttackFinished = Server.time+0.75;
}
