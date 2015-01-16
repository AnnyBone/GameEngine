/*	Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef __MENUMAIN__
#define __MENUMAIN__

#include "platform.h"
#include "platform_math.h"

#include "shared_flags.h"
#include "shared_menu.h"
#include "shared_module.h"
#include "shared_engine.h"
#include "shared_game.h"

#define	MENU_BASE_PATH	"textures/interface/"
#define MENU_HUD_PATH	"textures/interface/hud/"

extern ModuleImport_t	Engine;

extern	cvar_t	cvShowMenu,
				cvShowHealth,
				cvShowAmmo,
				cvShowCrosshair,
				cvCrosshairScale;

extern int	iMenuState;				// Global menu state.

void	Menu_AddState(int iState);
void	Menu_SetState(int iState);
void	Menu_RemoveState(int iState);
void	Menu_GetScreenSize(void);

char *va(char *format,...);	// Temporary convenience function, please don't get comfortable with this :(

class CMenu
{
public:
	CMenu();
	~CMenu();

	virtual void	Initialize();

	virtual	unsigned int	GetWidth()	{ return uiWidth; }
	virtual unsigned int	GetHeight() { return uiHeight; }

private:

	unsigned	int	uiWidth, uiHeight;	// Global width/height of the menu.
};

extern CMenu	*mGlobal;

#endif
