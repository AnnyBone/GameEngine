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

#include "menu_main.h"

#include "menu_hud.h"

ConsoleVariable_t	cv_menuhudshow = { "menu_hud_show", "1", false, false, "Toggles the display of the HUD." };
ConsoleVariable_t	cv_menuhudcrosshair = { "menu_hud_crosshair", "1", true, false, "Sets crosshair to use (set to -1 to hide)." };

Material_t *hud_mnumbers, *hud_micons, *hud_mcrosshairs;

void HUD_Initialize()
{
	hud_mnumbers = Engine.LoadMaterial("menu/hud/num");
	hud_micons = Engine.LoadMaterial("menu/hud/icons");
	hud_mcrosshairs = Engine.LoadMaterial("menu/hud/crosshairs");

	Engine.Cvar_RegisterVariable(&cv_menuhudshow, NULL);
	Engine.Cvar_RegisterVariable(&cv_menuhudcrosshair, NULL);
}

void HUD_DrawNumber(int x, int y, int w, int h, int value)
{
	Engine.DrawMaterialSurface(hud_mnumbers, value, x, y, w, h, 1);
}

void HUD_DrawNumbers(int x, int y, int value)
{
	if (value >= 100)
	{
		HUD_DrawNumber(x - 64, y, 32, 32, (value / 100));
	}
	if (value >= 10)
		HUD_DrawNumber(x - 32, y, 32, 32, (value % 100));
	HUD_DrawNumber(x, y, 32, 32, (value % 10));
}

void HUD_Draw(void)
{
	int	armor, health;

	if (!cv_menuhudshow.bValue)
		return;

	if (cv_menuhudcrosshair.iValue >= 0)
	{
		// Ensure it's a valid skin.
		int skin = cv_menuhudcrosshair.iValue;
		if (skin > (hud_mcrosshairs->iSkins - 1))
			skin = (hud_mcrosshairs->iSkins - 1);

		// Draw the crosshair.
		Engine.Client_SetMenuCanvas(CANVAS_CROSSHAIR);
		Engine.DrawMaterialSurface(hud_mcrosshairs, skin, -16, -16, 32, 32, 1);
	}

	// Draw the rest...
	Engine.Client_SetMenuCanvas(CANVAS_DEFAULT);
	
	health = Engine.Client_GetStat(STAT_HEALTH);
	if (health < 0)
		health = 0;

	armor = Engine.Client_GetStat(STAT_ARMOR);
	if (armor < 0)
		armor = 0;

	// Health
	Engine.DrawMaterialSurface(hud_micons, 0, 70, g_menuheight - 70, 32, 32, 1.0f);
	HUD_DrawNumbers(166, g_menuheight - 70, health);

	// Armor
	Engine.DrawMaterialSurface(hud_micons, 1, 70, g_menuheight - 110, 32, 32, 1.0f);
	HUD_DrawNumbers(166, g_menuheight - 110, armor);
}