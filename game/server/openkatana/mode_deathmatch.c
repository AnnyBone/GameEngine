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

#include "server_item.h"
#include "server_weapon.h"
#include "server_menu.h"

/*
	Standard Deathmatch Mode
	All vs All
*/

void Deathmatch_Spawn(ServerEntity_t *ePlayer)
{
	if(!Server.bRoundStarted)
	{
		ePlayer->v.model		= "";
		ePlayer->v.movetype		= MOVETYPE_NOCLIP;
		ePlayer->Physics.iSolid	= SOLID_BBOX;

		// Spectator mode! Hide the hud.
		ServerMenu_UpdateClient(ePlayer, MENU_STATE_HUD, false);

		// Make sure we have no model set.
		Entity_SetModel(ePlayer,ePlayer->v.model);

		return;
	}

	// Clear our inventory.
	memset(ePlayer->v.iInventory,0,sizeof(ePlayer->v.iInventory));

	// Add the default inventory.
	Item_AddInventory(Item_GetItem(WEAPON_DAIKATANA),ePlayer);
	Item_AddInventory(Item_GetItem(WEAPON_IONBLASTER),ePlayer);

	// Add ammo before setting it as active!
	ePlayer->local.ionblaster_ammo = 45;

	// Set the IonBlaster as our active weapon.
	Weapon_SetActive(Weapon_GetWeapon(WEAPON_IONBLASTER), ePlayer, true);

	ServerMenu_UpdateClient(ePlayer, MENU_STATE_HUD, true);
}

void Deathmatch_Frame(void)
{
	if(!Server.bActive || (!bIsMultiplayer && !bIsDeathmatch))
		return;
	else if((Server.iClients >= cvServerGameClients.value) && !Server.bRoundStarted)
	{
		Server.bRoundStarted = true;

		Engine.Server_BroadcastPrint("Fight!\n");
		return;
	}

	// [5/9/2013] TODO: Check scores etc etc etc ~hogsy
	/*
	for( i = 0; i < Server.iClients; i++)
	{
		ServerEntity_t *eClient = getentnum(i);
		if(eClient->iScore >= cvServerMaxScore.value)
			you won!
	}
	*/
}
