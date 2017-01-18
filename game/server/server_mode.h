/*
Copyright (C) 2011-2017 Mark E Sowden <markelswo@gmail.com>

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

#include "server_main.h"

enum
{
	MODE_SINGLEPLAYER,			// Just one player playing
	MODE_COOPERATIVE,			// Two players working together through the main game
	MODE_DEATHMATCH,			// Competitive online fighting
	MODE_CAPTURETHEFLAG,		// Competitive online capture the flag
	MODE_VEKTAR,				// Vektar mode!

	MODE_NONE					// No gametype, used for checks
};

// Deathmatch
void Deathmatch_Spawn(ServerEntity_t *ePlayer);
void Deathmatch_Frame(void);

extern bool	g_ismultiplayer, g_iscooperative, g_isdeathmatch;