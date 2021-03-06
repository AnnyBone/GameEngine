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

#include "platform_math.h"

#include "shared_base.h"
#include "shared_menu.h"
#include "shared_game.h"

#define	MENU_BASE_PATH	"menu/"
#define MENU_HUD_PATH	"menu/hud/"

namespace menu
{
	void Initialize();
	void Shutdown();

	void Draw(CoreViewport *viewport);
	void DrawMouse();

	extern PLuint width, height;
	extern PLint cursor_position[2];
}

PL_EXTERN_C

PL_EXTERN ModuleImport_t Engine;

PL_EXTERN int	iMenuState;				// Global menu state.

void	Menu_AddState(int iState);
void	Menu_SetState(int iState);
void	Menu_RemoveState(int iState);

char *va(char *format,...);	// Temporary convenience function, please don't get comfortable with this :(

PL_EXTERN_C_END