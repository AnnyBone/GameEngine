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

#include "server_menu.h"

/*	General function for globally updating the HUD for clients.
*/
void ServerMenu_UpdateClient(ServerEntity_t *entity, int menustate, bool show)
{
	// This is who we're telling to hide/show their HUD.
	Engine.SetMessageEntity(entity);

	Engine.WriteByte(MSG_ONE, SVC_UPDATEMENU);
	Engine.WriteByte(MSG_ONE, menustate);
	Engine.WriteByte(MSG_ONE, show);
}