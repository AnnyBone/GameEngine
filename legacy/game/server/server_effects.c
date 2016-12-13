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
void ServerEffect_BloodPuff(MathVector3f_t position)
{
	g_engine->WriteByte(MSG_BROADCAST, SVC_TEMPENTITY);
	g_engine->WriteByte(MSG_BROADCAST, CTE_BLOODPUFF);

	for (int i = 0; i < 3; i++)
		g_engine->WriteCoord(MSG_BROADCAST, position[i]);
}

/*	Cloud of blood.
*/
void ServerEffect_BloodCloud(MathVector3f_t position, BloodType_t type)
{
	g_engine->WriteByte(MSG_BROADCAST, SVC_TEMPENTITY);
	g_engine->WriteByte(MSG_BROADCAST, CTE_BLOODCLOUD);

	for (int i = 0; i < 3; i++)
		g_engine->WriteCoord(MSG_BROADCAST, position[i]);

	g_engine->WriteByte(MSG_BROADCAST, type);
}

void ServerEffect_Explosion(MathVector3f_t position)
{
	g_engine->WriteByte(MSG_BROADCAST, SVC_TEMPENTITY);
	g_engine->WriteByte(MSG_BROADCAST, CTE_EXPLOSION);

	for (int i = 0; i < 3; i++)
		g_engine->WriteCoord(MSG_BROADCAST, position[i]);
}

void ServerEffect_MuzzleFlash(MathVector3f_t position, MathVector3f_t angles)
{
	g_engine->WriteByte(MSG_BROADCAST, SVC_TEMPENTITY);
	g_engine->WriteByte(MSG_BROADCAST, CTE_MUZZLEFLASH);

	for (int i = 0; i < 3; i++)
		g_engine->WriteCoord(MSG_BROADCAST, position[i]);

	for (int i = 0; i < 3; i++)
		g_engine->WriteAngle(MSG_BROADCAST, angles[i]);
}