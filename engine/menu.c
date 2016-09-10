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

#include "engine_base.h"

#define MENU_MAIN								1	// Main Menu
#	define MENU_NEW								2	// New Game
#	define MENU_LOAD							3	// Load Game
#	define MENU_OPTIONS							5	// Options
#	define MENU_QUIT							6	// Quit

void M_Menu_Main_f (void);
	void M_Menu_Quit_f (void);

int		m_return_state;
bool	m_return_onerror;
char	m_return_reason [32];

void M_ToggleMenu_f (void)
{
}

//=============================================================================
/* MAIN MENU */

void M_Menu_Main_f (void)
{
}

//=============================================================================
/* NET MENU */

void M_Menu_Quit_f (void)
{
}
