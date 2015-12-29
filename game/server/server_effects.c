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

#include "server_main.h"

#include "server_effects.h"

void ServerEffect_BloodSpray(MathVector3f_t position)
{
	Engine.WriteByte(MSG_BROADCAST, SVC_TEMPENTITY);
	Engine.WriteByte(MSG_BROADCAST, CTE_BLOODSPRAY);
	Engine.WriteCoord(MSG_BROADCAST, position[0]);
	Engine.WriteCoord(MSG_BROADCAST, position[1]);
	Engine.WriteCoord(MSG_BROADCAST, position[2]);
}

void ServerEffect_Explosion(MathVector3f_t position)
{
	Engine.WriteByte(MSG_BROADCAST, SVC_TEMPENTITY);
	Engine.WriteByte(MSG_BROADCAST, CTE_EXPLOSION);
	Engine.WriteCoord(MSG_BROADCAST, position[0]);
	Engine.WriteCoord(MSG_BROADCAST, position[1]);
	Engine.WriteCoord(MSG_BROADCAST, position[2]);
}