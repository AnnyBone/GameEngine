/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#pragma once

#include "platform.h"

#include "shared_flags.h"
#include "shared_video.h"
#include "shared_menu.h"
#include "shared_engine.h"
#include "shared_material.h"
#include "shared_game.h"

#define	MENU_BASE_PATH	"menu/"
#define MENU_HUD_PATH	"menu/hud/"

plEXTERN_C_START

extern ModuleImport_t Engine;

extern int	iMenuState;				// Global menu state.

extern int g_menuwidth, g_menuheight;

void	Menu_UpdateScreenSize(void);
void	Menu_AddState(int iState);
void	Menu_SetState(int iState);
void	Menu_RemoveState(int iState);

char *va(char *format,...);	// Temporary convenience function, please don't get comfortable with this :(

plEXTERN_C_END