/*
Copyright (C) 2011-2016 OldTimes Software

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

#pragma once

#include "shared_entity.h"

typedef unsigned int ServerEntityHandle_t;

/*	Server Entity Structs	*/

typedef struct ServerEntity_s ServerEntity_t;

typedef struct ServerEntityModel_s
{
	float scale;

	unsigned int skin;
} ServerEntityModel_t;

typedef struct ServerEntityPhysics_s
{
	/*	todo, move the following here...
	movetype
	watertype
	waterlevel
	velocity
	avelocity
	solid
	*/

	float mass, gravity, friction;

	EntitySolidType_t solid;

	MaterialProperty_t surface;

	ServerEntity_t *ignore;	// todo, this should be dynamically set up
} ServerEntityPhysics_t;

typedef struct ServerEntityVehicle_s
{
	unsigned int passengers, maxpassengers;
	
	ServerEntityHandle_t slots[24];

	float fuel, maxfuel;
	float speed, maxspeed;

	bool active; // Is the vehicle turned on or not?

	// Callbacks
	void(*Enter)(ServerEntity_t *vehicle, ServerEntity_t *other);		// Function to call when a player enters the vehicle.
	void(*Exit)(ServerEntity_t *vehicle, ServerEntity_t *other);		// Function to call when a player leaves the vehicle.
	void(*Kill)(ServerEntity_t *vehicle, ServerEntity_t *other);		// Function to call when the vehicle is destroyed.
} ServerEntityVehicle_t;

typedef struct ServerEntityFrame_s
{
	// Callbacks
	void(*Event)(ServerEntity_t *entity);

	unsigned int frame;

	float speed;

	bool isend;
} ServerEntityFrame_t;