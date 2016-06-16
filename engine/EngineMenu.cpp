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

#include "engine_base.h"

#include "EngineMenu.h"
#include "video.h"
#include "engine_client.h"

#include "platform_window.h"

PL_INSTANCE hMenuInstance;

int	Menu_GetScreenWidth(void);
int Menu_GetScreenHeight(void);

MenuExport_t *g_menu = nullptr;

void Menu_Initialize(void)
{
	bool			loaded = false;
	ModuleImport_t	mImport;

	if (g_menu)
		plUnloadLibrary(hMenuInstance);

	mImport.Con_Printf = Con_Printf;
	mImport.Con_DPrintf = Con_DPrintf;
	mImport.Con_Warning = Con_Warning;
	mImport.Sys_Error = Sys_Error;
	mImport.Cvar_RegisterVariable = Cvar_RegisterVariable;

	mImport.LoadMaterial		= Material_Load;
	mImport.Material_Precache	= Material_Precache;

	mImport.DrawString				= Draw_String;
	mImport.DrawRectangle			= Draw_Rectangle;
	mImport.DrawMaterialSurface		= Draw_MaterialSurface;

	mImport.GetScreenWidth				= Menu_GetScreenWidth;
	mImport.GetScreenHeight				= Menu_GetScreenHeight;
	mImport.ShowCursor					= plShowCursor;
	mImport.GetCursorPosition			= Window_GetCursorPosition;
	mImport.Cmd_AddCommand				= Cmd_AddCommand;
	mImport.Client_GetStat				= Client_GetStat;
	mImport.Client_PrecacheResource		= Client_PrecacheResource;
	mImport.Client_SetMenuCanvas		= GL_SetCanvas;

	g_menu = (MenuExport_t*)plLoadLibraryInterface(hMenuInstance, va("%s/%s" MENU_MODULE, com_gamedir, g_state.path_modules), "Menu_Main", &mImport);
	if (!g_menu)
		Con_Warning(plGetError(), com_gamedir, MENU_MODULE);
	else if (g_menu->version != MENU_VERSION)
		Con_Warning("Size mismatch (recieved %i, expected %i)!\n", g_menu->version, MENU_VERSION);
	else
		loaded = true;

	if (!loaded)
	{
		// Unload our module.
		plUnloadLibrary(hMenuInstance);

		// Let the user know the module failed to load. ~hogsy
		Sys_Error("Failed to load %s/%s."PL_CPU_ID""PL_MODULE_EXTENSION"!\nCheck log for details.\n", com_gamedir, MENU_MODULE);
	}
}

// todo, obsolete
int	Menu_GetScreenWidth(void)
{
	Core::Camera *camera = g_cameramanager->GetCurrentCamera();
	if (!camera) return 640;
	Core::Viewport *viewport = dynamic_cast<Core::Viewport*>(camera->GetViewport());
	if (!viewport) return 640;

	return viewport->GetWidth();
}

// todo, obsolete
int Menu_GetScreenHeight(void)
{
	Core::Camera *camera = g_cameramanager->GetCurrentCamera();
	if (!camera) return 480;
	Core::Viewport *viewport = dynamic_cast<Core::Viewport*>(camera->GetViewport());
	if (!viewport) return 480;

	return viewport->GetHeight();
}

/*
	Import Functions
*/

/*	Toggles whether the menu is active or not.
*/
void Menu_Toggle(void)
{
	if (g_menu->GetState() & MENU_STATE_MENU)
		g_menu->RemoveState(MENU_STATE_MENU);
	else
		g_menu->AddState(MENU_STATE_MENU);
}

/*  Gets the state of the selected menu type.
*/
int Menu_GetState(MenuType_t mtType)
{
	switch(mtType)
	{
	case MENU_TYPE_HUD:
		break;
	case MENU_TYPE_MENU:
		break;
	default:
		Con_Warning("Unknown menu type! (%i)\n",mtType);
	}

	return 0;
}

/*
	Rendering
*/

#if 0
void Menu_DrawCharacter(int iX,int iY,int iCharacterNum)
{}

void Menu_Draw(void)
{}
#endif
