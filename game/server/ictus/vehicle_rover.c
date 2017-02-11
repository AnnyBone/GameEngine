/*	Copyright (C) 2011-2014 OldTimes Software
*/
#include "server_main.h"

/*
	Rover
	Simple test vehicle for Ictus.
*/

#include "server_vehicle.h"

void Rover_Enter(ServerEntity_t *eRover,ServerEntity_t *eOther);

void Rover_Spawn(ServerEntity_t *eRover)
{
	Engine.Server_PrecacheResource(RESOURCE_MODEL,"models/vehicles/van.bsp");

	// [15/4/2013] Set up passengers ~hogsy
	eRover->Vehicle.maxpassengers	= 4;
	eRover->Vehicle.Enter			= Rover_Enter;

	Vehicle_Spawn(eRover);

	eRover->v.netname		= "Rover";
	eRover->v.takedamage	= true;

	Entity_SetModel(eRover,"models/vehicles/van.bsp");

	eRover->Physics.mass = 2.0f;
}

/*	Called for when the player enters into the Rover.
*/
void Rover_Enter(ServerEntity_t *eRover,ServerEntity_t *eOther)
{
}