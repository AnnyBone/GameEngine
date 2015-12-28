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

#ifndef MENU_MAIN_H
#define MENU_MAIN_H

#include "platform.h"
#include "platform_math.h"

#include "shared_flags.h"
#include "shared_video.h"
#include "shared_menu.h"
#include "SharedModule.h"
#include "shared_engine.h"
#include "shared_material.h"
#include "shared_game.h"

#define	MENU_BASE_PATH	"menu/"
#define MENU_HUD_PATH	"menu/hud/"

extern ModuleImport_t Engine;

extern Material_t *g_mhudnumbers;
extern Material_t *g_mhudicons;

extern	cvar_t	cvShowMenu,
				cvShowHealth,
				cvShowAmmo,
				cvShowCrosshair,
				cvCrosshairScale;

extern int	iMenuState;				// Global menu state.

extern int
iMenuWidth,
iMenuHeight;

void	Menu_UpdateScreenSize(void);
void	Menu_AddState(int iState);
void	Menu_SetState(int iState);
void	Menu_RemoveState(int iState);

char *va(char *format,...);	// Temporary convenience function, please don't get comfortable with this :(

#endif
