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

#include "server_vehicle.h"

/*
	Base vehicle system.
*/

#define	VEHICLE_MAX_DAMAGE	-20

/*	Few notes here since this is confusing me a little in-regards to how all this is being set out...
	Vehicle_Enter implies that the vehicle is perhaps entering something. Monster_Use for example
	would traditionally imply that the monster is using something. Vehicles do not enter things -_-
*/

/*	Called when a vehicle is destroyed.
*/
void Vehicle_Kill(ServerEntity_t *eVehicle,ServerEntity_t *eAttacker)
{
	if(eVehicle->v.iHealth <= VEHICLE_MAX_DAMAGE)
	{
		// [12/4/2013] TODO: Explode! ~hogsy
		eVehicle->Monster.state = MONSTER_STATE_DEAD;
		eVehicle->v.movetype		= MOVETYPE_NONE;

		if(eVehicle->Vehicle.Kill)
			eVehicle->Vehicle.Kill(eVehicle,eAttacker);
	}
}

/*	Should be called for when a vehicle is damaged.
*/
void Vehicle_Damage(ServerEntity_t *eVehicle,ServerEntity_t *eAttacker,int iDamage)
{
	if(eVehicle->Monster.state == MONSTER_STATE_DEAD)
		return;

	eVehicle->v.iHealth -= iDamage;
	if(eVehicle->v.iHealth <= 0)
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
	int	i;

	// Monster variables
	eVehicle->Monster.iType	= MONSTER_VEHICLE;
	eVehicle->Vehicle.iType	= eVehicle->local.style;
	
	// Physics
	eVehicle->Physics.iSolid	= SOLID_SLIDEBOX;
	eVehicle->v.movetype		= MOVETYPE_BOUNCE;
	eVehicle->Physics.fGravity	= cvServerGravity.value;

	// [15/4/2013] Set all slots to open (in-case of respawn) ~hogsy
	for(i = 0; i < eVehicle->Vehicle.iMaxPassengers; i++)
		eVehicle->Vehicle.iSlot[i] = SLOT_OPEN;
}

/*	Checks the current number of passengers, sets up
	for the new passenger and sets whether the vehicle
	is active or not.
*/
void Vehicle_Enter(ServerEntity_t *eVehicle,ServerEntity_t *eOther)
{
	if(eVehicle->Vehicle.iPassengers >= eVehicle->Vehicle.iMaxPassengers)
	{
		Engine.CenterPrint(eOther,"Vehicle is full.\n");
		// [27/1/2013] TODO: Change to client-specific sound function ~hogsy
		Engine.Sound(eVehicle,CHAN_AUTO,"misc/deny.wav",255,ATTN_NORM);
		return;
	}
	else if(!eVehicle->Vehicle.iFuel)
	{
		Engine.CenterPrint(eOther,"Vehicle is out of fuel.\n");
		// [27/1/2013] TODO: Change to client-specific sound function ~hogsy
		Engine.Sound(eVehicle,CHAN_AUTO,"misc/deny.wav",255,ATTN_NORM);
		return;
	}
	else if(!eVehicle->v.iHealth)
	{
		Engine.CenterPrint(eOther,"Vehicle is too damaged.\n");
		// [27/1/2013] TODO: Change to client-specific sound function ~hogsy
		Engine.Sound(eVehicle,CHAN_AUTO,"misc/deny.wav",255,ATTN_NORM);
		return;
	}
	else if(!eOther->v.iHealth || eOther->local.eVehicle)
		return;

	eVehicle->Vehicle.iPassengers++;
	if(eVehicle->Vehicle.iPassengers == SLOT_DRIVER)
	{
		eVehicle->Vehicle.bActive								= true;
		eVehicle->Vehicle.iSlot[eVehicle->Vehicle.iPassengers]	= SLOT_DRIVER;
	}
	else
		eVehicle->Vehicle.iSlot[eVehicle->Vehicle.iPassengers]	= SLOT_PASSENGER;

	eOther->local.eVehicle		= eVehicle;
	eOther->local.iVehicleSlot	= eVehicle->Vehicle.iPassengers;

	if(eVehicle->Vehicle.Enter)
		eVehicle->Vehicle.Enter(eVehicle,eOther);
}

/*	Should be called when an entity exits a vehicle.
*/
void Vehicle_Exit(ServerEntity_t *eVehicle,ServerEntity_t *eOther)
{
	if(eVehicle->Vehicle.iSlot[eOther->local.iVehicleSlot] == SLOT_DRIVER)
		eVehicle->Vehicle.bActive = false;

	eVehicle->Vehicle.iPassengers--;
	eVehicle->Vehicle.iSlot[eOther->local.iVehicleSlot] = SLOT_OPEN;

	eOther->local.iVehicleSlot	= 0;
	eOther->local.eVehicle		= NULL;

	if(eVehicle->Vehicle.Exit)
		eVehicle->Vehicle.Exit(eVehicle,eOther);
}