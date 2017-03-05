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

#pragma once

PL_EXTERN_C

void ServerEffect_BloodPuff(PLVector3D position);
void ServerEffect_BloodCloud(PLVector3D position, BloodType_t type);
void ServerEffect_Explosion(PLVector3D position);
void ServerEffect_MuzzleFlash(PLVector3D position, PLVector3D angles);

PL_EXTERN_C_END