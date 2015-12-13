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

#include "client.h"

ConsoleVariable_t g_cvclientplayermodel = { "client_playermodel", MODEL_PLAYER, false, false };

/*	Called when a change occurs to our
	current player model. Sends request to
	server for it to apply the model we want.
*/
void ClientPlayer_UpdateModel(void)
{
}

void ClientPlayer_Initialize()
{
	Engine.Cvar_RegisterVariable(&g_cvclientplayermodel, ClientPlayer_UpdateModel);
}