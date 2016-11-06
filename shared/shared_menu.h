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

#pragma once

typedef enum
{
	MENU_IMAGE,
	MENU_MODEL,
	MENU_WINDOW,
	MENU_BUTTON,
		MENU_BUTTON_OK,
		MENU_BUTTON_CANCEL,
		MENU_BUTTON_YESNO,

	// Special types
	MENU_TYPE_HUD,
	MENU_TYPE_MENU
} MenuType_t;

// Different menu states.
#define	MENU_STATE_LOADING		1	// Display loading menu.
#define	MENU_STATE_HUD			2	// Display HUD.
#define	MENU_STATE_SCOREBOARD	4	// Display player score.
#define	MENU_STATE_MENU			8	// Display the menu.
#define	MENU_STATE_PAUSED		16	// Display pause menu.

// Exported functions
typedef struct
{
	int	version;	// Inteface version.

	void	(*Initialize)(void);				// Called when we initialize the module.
	void	(*Draw)(CoreViewport *viewport);	// Called whenever we draw the menu.
	void	(*Input)(int iKey);					// Called whenever we press a key while in menu mode.
	void	(*AddState)(int iState);			// Add onto the current menu state.
	void	(*RemoveState)(int iState);			// Remove a state from the menus current state set.
	void	(*SetState)(int iState);			// Forcefully set a new state.
	void	(*Shutdown)(void);					// Called upon shutdown.

	int		(*GetState)(void);	// Get the menus current state so we can check it.
} MenuExport_t;

#define MENU_MODULE		"menu"
#define	MENU_VERSION	(sizeof(MenuExport_t)+sizeof(ModuleImport_t))
