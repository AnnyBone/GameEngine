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

typedef enum EntitySolidType_s
{
	SOLID_NOT,		// Entity isn't solid.
	SOLID_TRIGGER,	// Entity will cause a trigger function.
	SOLID_BBOX,		// Entity is solid.
	SOLID_SLIDEBOX,	// Entity is solid and moves.
	SOLID_BSP
} EntitySolidType_t;

/*	Different type's of damage.
*/
typedef enum EntityDamageType_s
{
	DAMAGE_TYPE_NORMAL,
	DAMAGE_TYPE_EXPLODE,
	DAMAGE_TYPE_BURN,
	DAMAGE_TYPE_FREEZE,
	DAMAGE_TYPE_GRAVITY,
	DAMAGE_TYPE_CRUSH,
	DAMAGE_TYPE_FALL,

	DAMAGE_TYPE_NONE
} EntityDamageType_t;