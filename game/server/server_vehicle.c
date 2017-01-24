/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#include "server_vehicle.h"

/*	Base vehicle system.	*/

#define	VEHICLE_MAX_DAMAGE	-20

/*	Few notes here since this is confusing me a little in-regards to how all this is being set out...
	Vehicle_Enter implies that the vehicle is perhaps entering something. Monster_Use for example
	would traditionally imply that the monster is using something. Vehicles do not enter things -_-
*/

/*	Called when a vehicle is destroyed.
*/
void Vehicle_Kill(ServerEntity_t *eVehicle,ServerEntity_t *eAttacker)
{
	if(eVehicle->v.health <= VEHICLE_MAX_DAMAGE)
	{
		// [12/4/2013] TODO: Explode! ~hogsy
		eVehicle->v.movetype		= MOVETYPE_NONE;

		if(eVehicle->Vehicle.Kill)
			eVehicle->Vehicle.Kill(eVehicle,eAttacker);
	}
}

/*	Should be called for when a vehicle is damaged.
*/
void Vehicle_Damage(ServerEntity_t *eVehicle,ServerEntity_t *eAttacker,int iDamage)
{
	if(eVehicle->Monster.state == AI_STATE_DEAD)
		return;

	eVehicle->v.health -= iDamage;
	if(eVehicle->v.health <= 0)
		Vehicle_Kill(eVehicle,eAttacker);
}

/*	Should be called for entities that
	need to be set up as a vehicle. This
	sets up the basic parameters and
	states of the vehicle.
	Requires that iMaxPassengers is set!
*/
void Vehicle_Spawn(ServerEntity_t *eVehicle)
{
	// Monster variables
	eVehicle->Monster.type	= MONSTER_VEHICLE;
	
	// Physics
	eVehicle->Physics.solid		= SOLID_SLIDEBOX;
	eVehicle->v.movetype		= MOVETYPE_BOUNCE;
	eVehicle->Physics.gravity	= cv_server_gravity.value;

	//memset(&vehicle->Vehicle.slots, 0, plArrayElements(vehicle->Vehicle.slots));
}

/*	Checks the current number of passengers, sets up
	for the new passenger and sets whether the vehicle
	is active or not.
*/
void Vehicle_Enter(ServerEntity_t *eVehicle,ServerEntity_t *eOther)
{
	if(eVehicle->Vehicle.passengers >= eVehicle->Vehicle.maxpassengers)
	{
		Engine.CenterPrint(eOther,"Vehicle is full.\n");
		// [27/1/2013] TODO: Change to client-specific sound function ~hogsy
		Engine.Sound(eVehicle,CHAN_AUTO,"misc/deny.wav",255,ATTN_NORM);
		return;
	}
	else if(!eVehicle->Vehicle.fuel)
	{
		Engine.CenterPrint(eOther,"Vehicle is out of fuel.\n");
		// [27/1/2013] TODO: Change to client-specific sound function ~hogsy
		Engine.Sound(eVehicle,CHAN_AUTO,"misc/deny.wav",255,ATTN_NORM);
		return;
	}
	else if(!eVehicle->v.health)
	{
		Engine.CenterPrint(eOther,"Vehicle is too damaged.\n");
		// [27/1/2013] TODO: Change to client-specific sound function ~hogsy
		Engine.Sound(eVehicle,CHAN_AUTO,"misc/deny.wav",255,ATTN_NORM);
		return;
	}
	else if(!eOther->v.health || eOther->local.vehicle)
		return;

#if 0 // todo, rewrite this
	vehicle->Vehicle.passengers++;
	if(vehicle->Vehicle.passengers == SLOT_DRIVER)
	{
		vehicle->Vehicle.active = true;
		vehicle->Vehicle.iSlot[vehicle->Vehicle.iPassengers]	= SLOT_DRIVER;
	}
	else
		vehicle->Vehicle.iSlot[vehicle->Vehicle.iPassengers]	= SLOT_PASSENGER;
#endif

	eOther->local.vehicle		= eVehicle;
	eOther->local.iVehicleSlot	= eVehicle->Vehicle.passengers;

	if(eVehicle->Vehicle.Enter)
		eVehicle->Vehicle.Enter(eVehicle,eOther);
}

/*	Should be called when an entity exits a vehicle.
*/
void Vehicle_Exit(ServerEntity_t *eVehicle,ServerEntity_t *eOther)
{
#if 0 // todo, rewrite this
	if(vehicle->Vehicle.iSlot[eOther->local.iVehicleSlot] == SLOT_DRIVER)
		vehicle->Vehicle.bActive = false;

	vehicle->Vehicle.iPassengers--;
	vehicle->Vehicle.iSlot[eOther->local.iVehicleSlot] = SLOT_OPEN;
#endif

	eOther->local.iVehicleSlot	= 0;
	eOther->local.vehicle		= NULL;

	if(eVehicle->Vehicle.Exit)
		eVehicle->Vehicle.Exit(eVehicle,eOther);
}