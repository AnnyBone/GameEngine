/*	Copyright (C) 2011-2014 OldTimes Software
*/
#ifndef __SERVERPLAYER__
#define __SERVERPLAYER__

#include "server_main.h"

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

#endif