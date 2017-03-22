/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

#include "menu_main.h"

#include "menu_panel.h"
#include "menu_hud.h"

ConsoleVariable_t cv_menuhudshow = {"menu_hud_show", "1", false, false, "Toggles the display of the HUD."};
ConsoleVariable_t cv_menuhudcrosshair = {"menu_hud_crosshair", "1", true, false,
                                         "Sets crosshair to use (set to -1 to hide)."};

Material_t *hud_mnumbers, *hud_micons, *hud_mcrosshairs;

// MUST match with entries in hud\icons.material!
enum {
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

void HUD_Initialize() {
    hud_mnumbers = Engine.LoadMaterial("menu/hud/num");
    hud_micons = Engine.LoadMaterial("menu/hud/icons");
    hud_mcrosshairs = Engine.LoadMaterial("menu/hud/crosshairs");

    Engine.Cvar_RegisterVariable(&cv_menuhudshow, NULL);
    Engine.Cvar_RegisterVariable(&cv_menuhudcrosshair, NULL);
}

void HUD_DrawNumber(int x, int y, PLuint w, PLuint h, PLuint value) {
    if (value < 0) value = 0;
    if (value > 9) value = 9;
    Engine.DrawMaterialSurface(hud_mnumbers, value, x, y, w, h, 1);
}

void HUD_DrawNumbers(int x, int y, PLuint value, bool offset) {
    if (value >= 100) {
        HUD_DrawNumber(x, y, 32, 32, (value / 100));
        x += 34;
    } else if (offset) x += 32;

    if (value >= 10) {
        HUD_DrawNumber(x, y, 32, 32, (value % 100));
        x += 34;
    } else if (offset) x += 32;

    HUD_DrawNumber(x, y, 32, 32, (value % 10));
}

void HUD_Draw() {
    if (!cv_menuhudshow.bValue)
        return;

    if (cv_menuhudcrosshair.iValue >= 0) {
        // Ensure it's a valid skin.
        unsigned int skin = (PLuint) cv_menuhudcrosshair.iValue;
        if (skin > (hud_mcrosshairs->num_skins - 1)) {
            skin = (hud_mcrosshairs->num_skins - 1);
        }

        // Draw the crosshair.
        Engine.Client_SetMenuCanvas(CANVAS_CROSSHAIR);

        MaterialSkin_t *cur_skin = &hud_mcrosshairs->skin[skin];
        Engine.DrawMaterialSurface(
                hud_mcrosshairs,
                skin,
                0, 0,
                cur_skin->texture[0].uiWidth, cur_skin->texture[0].uiHeight,
                1
        );
    }

    // Draw the rest...
    Engine.Client_SetMenuCanvas(CANVAS_DEFAULT);

    int health = Engine.Client_GetStat(STAT_HEALTH);
    if (health < 0) health = 0;

    int armor = Engine.Client_GetStat(STAT_ARMOR);
    if (armor < 0) armor = 0;

    // Health
    Engine.DrawMaterialSurface(hud_micons, HUD_ICON_HEARTWAVE0, 70, menu::height - 70, 32, 32,
                               1.0f);    // TODO: Change depending on health...
    Engine.DrawMaterialSurface(hud_micons, HUD_ICON_HEALTH, 70, menu::height - 70, 32, 32, 1.0f);
    HUD_DrawNumbers(106, menu::height - 70, (PLuint) health, false);

    // Armor
    Engine.DrawMaterialSurface(hud_micons, HUD_ICON_ARMORWAVE0, 70, menu::height - 110, 32, 32,
                               1.0f);    // TODO: Change depending on armor...
    Engine.DrawMaterialSurface(hud_micons, HUD_ICON_ARMOR, 70, menu::height - 110, 32, 32, 1.0f);
    HUD_DrawNumbers(106, menu::height - 110, (PLuint) armor, false);
}