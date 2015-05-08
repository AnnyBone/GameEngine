/*	Copyright (C) 2011-2015 OldTimes Software
*/
#include "server_monster.h"

/*
	The Prisoner
	One of the victims of Mishima's evil plans.
*/

#include "server_waypoint.h"
#include "server_weapon.h"

/*	Notes
		We don't need to check if we're dead or not in
		think functions, this is done in the main Monster_Think.
		It may be worth checking the players health but that's
		about it.
		Prisoners do NOT take anger into account since they have
		nothing to attack with.

	[20/9/2012]
		Revised Prisoner_Stand. ~hogsy
		Firstly prisoners are usually in cells,
		so checking for an enemy here is useless
		and instead we're just going to check if
		we're fleeing instead. We'll start fleeing
		if we're actually under attack anyway, we
		could probably mark any attacking ent as
		a "threat" and then check that here instead! ~hogsy
	[21/10/2012]
		Revision #2 of Prisoner_Stand. ~hogsy
*/

enum
{
	COMMAND_CHECK_CELL,		// Should we still check if we're in our cell or not?
	COMMAND_CHECK_DELAY		// Delay before checking again.
};

#define PRISONER_MAX_HEALTH	30
#define PRISONER_MAX_SIGHT	900.0f
#define	PRISONER_MIN_HEALTH	-20

/*	Checks if the prisoner is currently within
	his cell or not... This isn't fool proof
	so don't rely on it.
*/
bool Prisoner_CheckCell(ServerEntity_t *ePrisoner)
{
	trace_t		tDoorTrace;
	Waypoint_t	*wCellDoorWaypoint;

	// [21/9/2012] Find a specific waypoint ~hogsy
	wCellDoorWaypoint = Waypoint_GetByName(ePrisoner,"celldoor",MONSTER_RANGE_NEAR);
	if(wCellDoorWaypoint)
	{
		// [27/9/2012] End point set to the waypoints position, meaning we expect it to be inside / in-front of the door ~hogsy
		tDoorTrace = Engine.Server_Move(ePrisoner->v.view_ofs,mv3Origin,mv3Origin,wCellDoorWaypoint->position,MOVE_NOMONSTERS,ePrisoner);
		if(tDoorTrace.ent)
		{
			// [20/9/2012] I hate doing shit like this here but if we're not already there, we should be! ~hogsy
			if(	MONSTER_GetRange(ePrisoner,wCellDoorWaypoint->position) > MONSTER_RANGE_NEAR	&&
				// [21/9/2012] Also check that this waypoint is safe from enemies ~hogsy
				Waypoint_IsSafe(ePrisoner,wCellDoorWaypoint))
			{
				// [20/9/2012] Set that as our target so we'll walk over to it later ~hogsy
				Math_VectorCopy(wCellDoorWaypoint->position,ePrisoner->Monster.vTarget);

				// [21/9/2012] Set the state to wandering so we walk over to the selected waypoint ~hogsy
				Monster_SetThink(ePrisoner,THINK_WANDERING);
			}
			return true;
		}
	}
	return false;
}

void Prisoner_Think(ServerEntity_t *ePrisoner)
{
	if(ePrisoner->Monster.iState != STATE_AWAKE)
		return;

	switch(ePrisoner->Monster.iThink)
	{
	case THINK_IDLE:
#if 0
		if (ePrisoner->Monster.iCommandList[COMMAND_CHECK_CELL] && !Prisoner_CheckCell(ePrisoner))
		{
			if (rand() % 200 == 1)
				Sound(ePrisoner, CHAN_VOICE, PRISONER_SOUND_HELP, 255, ATTN_NORM);
		}
#endif
		break;
	case THINK_WANDERING:
		break;
	}
}

void Prisoner_Walk(ServerEntity_t *ePrisoner)
{
	// [21/9/2012] Check our health before we attempt to move! ~hogsy
	if(ePrisoner->v.iHealth <= 0)
		return;

	Monster_MoveToGoal(ePrisoner,ePrisoner->Monster.vTarget,10.0f);
}

void Prisoner_Run(ServerEntity_t *ePrisoner)
{
	// [21/9/2012] Check our health before we attempt to move! ~hogsy
	if(ePrisoner->v.iHealth <= 0)
		return;

	Monster_MoveToGoal(ePrisoner,ePrisoner->Monster.vTarget,20.0f);
}

void Prisoner_Pain(ServerEntity_t *ePrisoner, ServerEntity_t *eOther)
{
}

void Prisoner_Die(ServerEntity_t *ePrisoner, ServerEntity_t *eOther)
{
	if(ePrisoner->v.iHealth < PRISONER_MIN_HEALTH)
	{
		bool bSliced = false;

		Sound(ePrisoner,CHAN_VOICE,"misc/gib1.wav",255,ATTN_NORM);

		if(Entity_IsPlayer(eOther))
			if(Weapon_GetCurrentWeapon(eOther)->iItem == WEAPON_DAIKATANA)
				bSliced = true;

		if(bSliced)
		{
			ThrowGib(ePrisoner->v.origin,ePrisoner->v.velocity,"models/prisoner_torso.md2",(float)ePrisoner->v.iHealth*-1,true);
			ThrowGib(ePrisoner->v.origin,ePrisoner->v.velocity,"models/prisoner_torsoless.md2",(float)ePrisoner->v.iHealth*-1,true);
		}
		else
		{
			// [13/9/2012] Updated paths ~hogsy
			ThrowGib(ePrisoner->v.origin,ePrisoner->v.velocity,PHYSICS_MODEL_GIB0,(float)ePrisoner->v.iHealth*-1,true);
			ThrowGib(ePrisoner->v.origin,ePrisoner->v.velocity,PHYSICS_MODEL_GIB1,(float)ePrisoner->v.iHealth*-1,true);
			ThrowGib(ePrisoner->v.origin,ePrisoner->v.velocity,PHYSICS_MODEL_GIB2,(float)ePrisoner->v.iHealth*-1,true);
		}

		Engine.Particle(ePrisoner->v.origin,ePrisoner->v.velocity,10.0f,"blood",20);

		Entity_Remove(ePrisoner);
	}
}

void Prisoner_Spawn(ServerEntity_t *ePrisoner)
{
	char cPrisonerName[16];

	Server_PrecacheModel(PRISONER_MODEL_BODY);
	Server_PrecacheModel(PRISONER_MODEL_LEGS);
	Server_PrecacheModel(PRISONER_MODEL_TORSO);
	Server_PrecacheSound(PRISONER_SOUND_HELP);

#if 0
	{
		static	int	siPrisonerNumber[512],
					siPrisonerCount = 0;
		int			i;

		// [20/9/2012] Make sure no two serials are the same ~hogsy
		siPrisonerCount++;
PRISONER_GENERATEKEY:
		siPrisonerNumber[siPrisonerCount] = rand()%500+1;
		for(i = 0; i < siPrisonerCount; i++)
			if(siPrisonerNumber[i] == siPrisonerNumber[siPrisonerCount])
				goto PRISONER_GENERATEKEY;
	}
#endif

	// [20/9/2012] Little touch to randomize prisoner names ~hogsy
	sprintf(cPrisonerName,"Prisoner %i",rand()%300+1);

	ePrisoner->v.movetype		= MOVETYPE_STEP;
	ePrisoner->v.iHealth		= PRISONER_MAX_HEALTH;
	ePrisoner->local.iMaxHealth = PRISONER_MAX_HEALTH;
	ePrisoner->v.bTakeDamage	= true;
	ePrisoner->v.netname		= cPrisonerName;
	ePrisoner->v.frame			= 0;

	// Physics Properties
	ePrisoner->Physics.iSolid		= SOLID_SLIDEBOX;
	ePrisoner->Physics.fMass		= 1.2f;						// [6/8/2013] Just a little more than the player ~hogsy
	ePrisoner->Physics.fGravity		= cvServerGravity.value;
	ePrisoner->Physics.fFriction	= 1.5f;

	// Initial Command States
	ePrisoner->Monster.iCommandList[COMMAND_CHECK_CELL]	= true;

	// Monster Properties
	ePrisoner->Monster.iType = MONSTER_PRISONER;

	Entity_SetKilledFunction(ePrisoner, Prisoner_Die);
	Entity_SetDamagedFunction(ePrisoner, Prisoner_Pain);

	//	ePrisoner->Monster.think_walk		= Prisoner_Walk;
	//	ePrisoner->Monster.think_run		= Prisoner_Run;
	ePrisoner->Monster.Think = Prisoner_Think;

	// [6/8/2012] State must be set before think! ~hogsy
	Monster_SetState(ePrisoner,STATE_AWAKE);
	Monster_SetThink(ePrisoner,THINK_IDLE);

	Entity_SetModel(ePrisoner,PRISONER_MODEL_BODY);
	Entity_SetSize(ePrisoner,-16.0f,-16.0f,-24.0f,16.0f,16.0f,32.0f);

	Entity_DropToFloor(ePrisoner);
}
