/*
Copyright (C) 2011-2017 OldTimes Software

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

// Platform library
#include "platform_library.h"
#include "menu_main.h"

/*
	Main menu entry!
	Functions called by the engine can be found here.
*/

#include "menu_hud.h"

/*	TODO:
		#	Get menu elements to be handled like objects.
		#	Let us handle models (for 3D menu interface).
		#	Get basic window environment done, move windows
			if being clicked and mousepos changing, allow
			windows to be resized, let us show a "virtual"
			window within another window, "Katana Menu Scripts",
			allow us to manipulate windows within a 3D space
		#	Allow for texture maps!
		etc
		etc
*/

MenuExport_t Export;
ModuleImport_t Engine;

int iMenuState = 0;

ConsoleVariable_t cv_menushow = {"menu_show", "1", false, false, "Toggle the display of any menu elements."};
ConsoleVariable_t cv_menudebug = {"menu_debug", "0", false, false, "Toggle the display of any debugging information."};

Material_t *debug_logo = NULL;

namespace menu {
    PLuint width = 640, height = 480;
    PLint cursor_position[2] = {0};

    CoreViewport *viewport = nullptr;
}

// TODO: Why are we doing this!? Should be using the one from the lib
char *va(char *format, ...) {
    va_list argptr;
    static char string[1024];

    va_start(argptr, format);
    vsprintf(string, format, argptr);
    va_end(argptr);

    return string;
}

void menu::Initialize() {
    Engine.Con_Printf("Initializing menu...\n");

    Engine.Cvar_RegisterVariable(&cv_menushow, NULL);

    // temp start
    debug_logo = Engine.LoadMaterial("debug/debug_logo");
    debug_logo->flags = MATERIAL_FLAG_PRESERVE;
    // temp end

    HUD_Initialize();
}

void menu::DrawMouse() {
    Engine.Client_SetMenuCanvas(CANVAS_DEFAULT);

    Engine.GetCursorPosition(&menu::cursor_position[0], &menu::cursor_position[1]);

    //Engine.DrawPic(MENU_BASE_PATH"cursor", 1.0f, iMousePosition[0], iMousePosition[1], 16, 16);
}

void menu::Draw(CoreViewport *viewport) {
    if (!cv_menushow.bValue || !viewport) {
        return;
    }

    menu::width = viewport->GetWidth();
    menu::height = viewport->GetHeight();

    Engine.Client_SetMenuCanvas(CANVAS_DEFAULT);

    if (iMenuState & MENU_STATE_LOADING) {
        // [21/5/2013] TODO: Switch to element ~hogsy
        //Engine.DrawPic(MENU_BASE_PATH"loading",1.0f,(iMenuWidth-256)/2,(g_menuheight-32)/2,256,32);
        return;
    } else if (iMenuState & MENU_STATE_PAUSED) {
        //Engine.DrawPic(MENU_BASE_PATH"paused",1.0f,0,0,32,64);
        return;
    }

    if ((iMenuState & MENU_STATE_HUD) && (!(iMenuState & MENU_STATE_SCOREBOARD) && !(iMenuState & MENU_STATE_MENU)))
        HUD_Draw();
    else if (iMenuState & MENU_STATE_MENU) {
        PLColour col = {0, 0, 0, 0.8f};
        Engine.DrawRectangle(0, 0, viewport->GetWidth(), viewport->GetHeight(), col);

        Engine.DrawString(110, 80, ">");
        Engine.DrawString(190, 80, "<");

#if 0
        switch(iMenuDisplaySelection)
        {
        case MENU_MAIN:
            Engine.DrawString(120,80,"New Game");
            Engine.DrawString(120,90,"Load Game");
            Engine.DrawString(120,100,"Settings");
            Engine.DrawString(120,110,"Quit");
            break;
        case MENU_NEW:
            break;
        case MENU_LOAD:
        case MENU_OPTIONS:
            break;
        case MENU_QUIT:
            // Do we even need to draw anything for this? ~hogsy
            break;
        }
#endif

        menu::DrawMouse();
    }
}

/*	Called by the engine.
	Add state to the current state set.
*/
void Menu_AddState(int iState) {
    // [17/9/2013] Return since the state is already added ~hogsy
    if (iMenuState & iState)
        return;

    iMenuState |= iState;
}

/*	Called by the engine.	*/
void Menu_RemoveState(int iState) {
    // [17/9/2013] Return since the state is already removed ~hogsy
    if (!(iMenuState & iState))
        return;

    iMenuState &= ~iState;
}

/*	Called by the engine.	*/
void Menu_SetState(int iState) {
    iMenuState = iState;
}

/*	Called by the engine.	*/
int Menu_GetState(void) {
    return iMenuState;
}

/*	Called by the engine.	*/
void menu::Shutdown() {
    Engine.Con_Printf("Shutting down menu...\n");
}

void Input_Key(int iKey) {

}

extern "C" PL_MODULE_EXPORT MenuExport_t *Menu_Main(ModuleImport_t *Import) {
    Engine = *Import;

    Export.version = MENU_VERSION;
    Export.Initialize = menu::Initialize;
    Export.Shutdown = menu::Shutdown;
    Export.Draw = menu::Draw;
    Export.Input = Input_Key;
    Export.SetState = Menu_SetState;
    Export.RemoveState = Menu_RemoveState;
    Export.AddState = Menu_AddState;
    Export.GetState = Menu_GetState;

    return &Export;
}
