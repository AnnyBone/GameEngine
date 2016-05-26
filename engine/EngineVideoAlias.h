/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
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

extern bool	bShading;

typedef struct
{
	short pose1,pose2;

	float blend;

	MathVector3f_t origin,angles;
} lerpdata_t;

plEXTERN_C_START

void Alias_SetupFrame(MD2_t *mModel, ClientEntity_t *ceCurrent, lerpdata_t *ldLerp);
void Alias_Draw(ClientEntity_t *eEntity);
void Alias_DrawFrame(MD2_t *mModel, ClientEntity_t *eEntity, lerpdata_t lLerpData);

plEXTERN_C_END
