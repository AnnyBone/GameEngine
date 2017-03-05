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

/*	Simple puff of blood.
*/
void ServerEffect_BloodPuff(PLVector3D position)
{
	g_engine->WriteByte(MSG_BROADCAST, SVC_TEMPENTITY);
	g_engine->WriteByte(MSG_BROADCAST, CTE_BLOODPUFF);

	g_engine->WriteCoord(MSG_BROADCAST, position.x);
	g_engine->WriteCoord(MSG_BROADCAST, position.y);
	g_engine->WriteCoord(MSG_BROADCAST, position.z);
}

/*	Cloud of blood.
*/
void ServerEffect_BloodCloud(PLVector3D position, BloodType_t type) {
	g_engine->WriteByte(MSG_BROADCAST, SVC_TEMPENTITY);
	g_engine->WriteByte(MSG_BROADCAST, CTE_BLOODCLOUD);

    g_engine->WriteCoord(MSG_BROADCAST, position.x);
    g_engine->WriteCoord(MSG_BROADCAST, position.y);
    g_engine->WriteCoord(MSG_BROADCAST, position.z);

	g_engine->WriteByte(MSG_BROADCAST, type);
}

void ServerEffect_Explosion(PLVector3D position)
{
	g_engine->WriteByte(MSG_BROADCAST, SVC_TEMPENTITY);
	g_engine->WriteByte(MSG_BROADCAST, CTE_EXPLOSION);

    g_engine->WriteCoord(MSG_BROADCAST, position.x);
    g_engine->WriteCoord(MSG_BROADCAST, position.y);
    g_engine->WriteCoord(MSG_BROADCAST, position.z);
}

void ServerEffect_MuzzleFlash(PLVector3D position, PLVector3D angles)
{
	g_engine->WriteByte(MSG_BROADCAST, SVC_TEMPENTITY);
	g_engine->WriteByte(MSG_BROADCAST, CTE_MUZZLEFLASH);

    g_engine->WriteCoord(MSG_BROADCAST, position.x);
    g_engine->WriteCoord(MSG_BROADCAST, position.y);
    g_engine->WriteCoord(MSG_BROADCAST, position.z);

	g_engine->WriteAngle(MSG_BROADCAST, angles.x);
    g_engine->WriteAngle(MSG_BROADCAST, angles.y);
    g_engine->WriteAngle(MSG_BROADCAST, angles.z);
}