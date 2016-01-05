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

void HUD_DrawNumbers(int x, int y, int value)
{
	if (value >= 100)
	Engine.DrawMaterialSurface(g_mhudnumbers, (value / 100), x - 64, y, 32, 32, 0.8f);
	if (value >= 10)
		Engine.DrawMaterialSurface(g_mhudnumbers, ((value % 100) / 10), x - 32, y, 32, 32, 0.8f);
	Engine.DrawMaterialSurface(g_mhudnumbers, (value % 10), x, y, 32, 32, 0.8f);
}

void HUD_Draw(void)
{
	int	armor, health;

	// Draw the crosshair...
	{
		Engine.Client_SetMenuCanvas(CANVAS_CROSSHAIR);
		//Engine.DrawPic(va(MENU_HUD_PATH"crosshair%i",1),1.0f,-16,-16,32,32);
	}

	// Draw the rest...
	Engine.Client_SetMenuCanvas(CANVAS_DEFAULT);

#ifdef GAME_OPENKATANA		
	health = Engine.Client_GetStat(STAT_HEALTH);
	if (health < 0)
		health = 0;

	armor = Engine.Client_GetStat(STAT_ARMOR);
	if (armor < 0)
		armor = 0;

	// Health
	Engine.DrawMaterialSurface(g_mhudicons, 0, 70, iMenuHeight - 70, 32, 32, 1.0f);
	HUD_DrawNumbers(166, iMenuHeight - 70, health);

	// Armor
	Engine.DrawMaterialSurface(g_mhudicons, 1, 70, iMenuHeight - 110, 32, 32, 1.0f);
	HUD_DrawNumbers(166, iMenuHeight - 110, armor);
#if 0
	if (armor >= 100)
		Engine.DrawMaterialSurface(g_mhudnumbers, (armor / 100), 102, iMenuHeight-70, 32, 32, 1.0f);
	if(armor >= 10)
		Engine.DrawMaterialSurface(g_mhudnumbers, ((armor % 100) / 10), 134, iMenuHeight - 70, 32, 32, 1.0f);
#endif
#endif
}