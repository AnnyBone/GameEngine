/*	Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef __MENUMAIN__
#define __MENUMAIN__

#include "platform.h"
#include "platform_math.h"

#include "shared_flags.h"
#include "shared_video.h"
#include "shared_menu.h"
#include "shared_module.h"
#include "shared_engine.h"
#include "shared_game.h"

#define	MENU_BASE_PATH	"menu/"
#define MENU_HUD_PATH	"menu/hud/"

extern ModuleImport_t	Engine;

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
