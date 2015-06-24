#include "server_weapon.h"

void Zeus_Deploy(ServerEntity_t *ent)
{
	//Weapon_Animate(ent,FALSE,1,19,0.08f,0,0,0,FALSE);
}

void Zeus_Hit(ServerEntity_t *ent)
{
	vec3_t	vSource, vTarg, vel, mvDirection;
	trace_t	trace;

	Math_MVToVector(Weapon_Aim(ent), mvDirection);

	vSource[0] = ent->v.origin[0];
	vSource[1] = ent->v.origin[1];
	vSource[2] = ent->v.origin[2]+22.0f;

	vTarg[0] = vSource[0] + (mvDirection[0] * 250);
	vTarg[1] = vSource[1] + (mvDirection[1] * 250);
	vTarg[2] = vSource[2] + (mvDirection[2] * 250);

	Math_VectorClear(vel);

	trace = Traceline(ent,vSource,vTarg,0);
	if(trace.fraction != 1.0f)
		{
			if(trace.ent && trace.ent->v.bTakeDamage)
			{
				if(trace.ent->local.bBleed)
					Engine.Particle(trace.endpos,vel,10,"blood",30);

				Entity_Damage(trace.ent, ent, 5, 0);
			}
			else
				Engine.Particle(trace.endpos,vel,10,"zspark",30);
		}
}

void Zeus_PrimaryAttack(ServerEntity_t *ent)
{
	//Weapon_Animate(ent,FALSE,20,35,0.08f,0,0,0,FALSE);

	if(ent->local.attackb_finished > Server.dTime)
		ent->local.dAttackFinished = Server.dTime+0.6;
	else
		ent->local.dAttackFinished = Server.dTime+1.2;
}
