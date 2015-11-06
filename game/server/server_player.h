/*	Copyright (C) 2011-2015 OldTimes Software

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

#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

int	iPlayerModelIndex;

extern	EntityFrame_t	PlayerAnimation_Death1[];
extern	EntityFrame_t	PlayerAnimation_Death2[];
extern	EntityFrame_t	PlayerAnimation_Fire[];
extern	EntityFrame_t	PlayerAnimation_Idle[];
extern	EntityFrame_t	PlayerAnimation_Jump[];
extern	EntityFrame_t	PlayerAnimation_RunJump[];
extern	EntityFrame_t	PlayerAnimation_Walk[];
extern	EntityFrame_t	PlayerAnimation_KatanaIdle[];
extern	EntityFrame_t	PlayerAnimation_KatanaAttack1[];
extern	EntityFrame_t	PlayerAnimation_KatanaAttack2[];

void Player_PostThink(ServerEntity_t *ePlayer);
void Player_PreThink(ServerEntity_t *ePlayer);
void Player_Spawn(ServerEntity_t *self);
void Player_Jump(ServerEntity_t *self);
#ifdef OPENKATANA
void Player_CheckPowerups(ServerEntity_t *self);
#endif
void Player_DeathThink(ServerEntity_t *ent);
void Player_Use(ServerEntity_t *ePlayer);

#endif	// SERVER_PLAYER_H