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

plEXTERN_C_START

void ClientEffect_Initialize(void);
void ClientEffect_ParticleField(MathVector3f_t position, MathVector3f_t mins, MathVector3f_t maxs, float density);
void ClientEffect_BloodPuff(MathVector3f_t position);
void ClientEffect_BloodCloud(MathVector3f_t position, BloodType_t type);
void ClientEffect_Smoke(MathVector3f_t position);
void ClientEffect_Explosion(MathVector3f_t position);
void ClientEffect_MuzzleFlash(MathVector3f_t position, MathVector3f_t angles);
#ifdef GAME_OPENKATANA
void ClientEffect_IonBallTrail(MathVector3f_t position);
#endif

plEXTERN_C_END