/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2015 Marco 'eukara' Hladik <eukos@oldtimes-software.com>
Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#include "menu_main.h"

#include "menu_panel.h"
#include "menu_hud.h"

ConsoleVariable_t	cv_menuhudshow = { "menu_hud_show", "1", false, false, "Toggles the display of the HUD." };
ConsoleVariable_t	cv_menuhudcrosshair = { "menu_hud_crosshair", "1", true, false, "Sets crosshair to use (set to -1 to hide)." };

Material *hud_mnumbers, *hud_micons, *hud_mcrosshairs;

// MUST match with entries in hud\icons.material!
enum
{
	HUD_ICON_HEALTH,
	HUD_ICON_ARMOR,

	HUD_ICON_HEARTWAVE0,

	HUD_ICON_ARMORWAVE0,
};

#if 0
namespace Menu
{
	class HUDPanel : public Panel
	{
	public:
		HUDPanel(Panel *parent);

	private:
	};

	HUDPanel::HUDPanel(Panel *parent) : Panel(parent)
	{
	}
}
#endif

void HUD_Initialize()
{
	hud_mnumbers = Engine.LoadMaterial("menu/hud/num");
	hud_micons = Engine.LoadMaterial("menu/hud/icons");
	hud_mcrosshairs = Engine.LoadMaterial("menu/hud/crosshairs");

	Engine.Cvar_RegisterVariable(&cv_menuhudshow, NULL);
	Engine.Cvar_RegisterVariable(&cv_menuhudcrosshair, NULL);
}

void HUD_DrawNumber(int x, int y, PLuint w, PLuint h, PLuint value)
{
	if (value < 0)	value = 0;
	if (value > 9)	value = 9;
	Engine.DrawMaterialSurface(hud_mnumbers, value, x, y, w, h, 1);
}

void HUD_DrawNumbers(int x, int y, PLuint value, bool offset)
{
	if (value >= 100)
	{
		HUD_DrawNumber(x, y, 32, 32, (value / 100));
		x += 34;
	}
	else if (offset) x += 32;

	if (value >= 10)
	{
		HUD_DrawNumber(x, y, 32, 32, (value % 100));
		x += 34;
	}
	else if (offset) x += 32;

	HUD_DrawNumber(x, y, 32, 32, (value % 10));
}

void HUD_Draw()
{
	if (!cv_menuhudshow.boolean_value)
		return;

	if (cv_menuhudcrosshair.iValue >= 0)
	{
		// Ensure it's a valid skin.
		unsigned int skin = (PLuint)cv_menuhudcrosshair.iValue;
		if (skin > (hud_mcrosshairs->num_skins - 1))
			skin = (hud_mcrosshairs->num_skins - 1);

		// Draw the crosshair.
		Engine.Client_SetMenuCanvas(CANVAS_CROSSHAIR);
		Engine.DrawMaterialSurface(hud_mcrosshairs, skin, -16, -16, 32, 32, 1);
	}

	// Draw the rest...
	Engine.Client_SetMenuCanvas(CANVAS_DEFAULT);
	
	int health = Engine.Client_GetStat(STAT_HEALTH);
	if (health < 0) health = 0;

	int armor = Engine.Client_GetStat(STAT_ARMOR);
	if (armor < 0) armor = 0;

	// Health
	Engine.DrawMaterialSurface(hud_micons, HUD_ICON_HEARTWAVE0, 70, menu::height - 70, 32, 32, 1.0f);	// TODO: Change depending on health...
	Engine.DrawMaterialSurface(hud_micons, HUD_ICON_HEALTH, 70, menu::height - 70, 32, 32, 1.0f);
	HUD_DrawNumbers(106, menu::height - 70, (PLuint)health, false);

	// Armor
	Engine.DrawMaterialSurface(hud_micons, HUD_ICON_ARMORWAVE0, 70, menu::height - 110, 32, 32, 1.0f);	// TODO: Change depending on armor...
	Engine.DrawMaterialSurface(hud_micons, HUD_ICON_ARMOR, 70, menu::height - 110, 32, 32, 1.0f);
	HUD_DrawNumbers(106, menu::height - 110, (PLuint)armor, false);
}