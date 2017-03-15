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

#include "Editor.h"

using namespace xenon;

class Editor : public FXMainWindow {
FXDECLARE(Editor)

public:
    Editor(FXApp *app);

    virtual ~Editor();

    virtual void create();

    void createMenuBar();


protected:
    Editor() {}

private:
    FXMenuBar *menubar;
    FXExStatusBar *statusbar;
};

FXIMPLEMENT(Editor, FXMainWindow, NULL, NULL)

Editor::Editor(FXApp *app) :
        FXMainWindow(app, EDITOR_TITLE, NULL, NULL, DECOR_ALL, 0, 0, 1024, 768) {

    createMenuBar();

    FXDockSite *topdock = new FXDockSite(this, DOCKSITE_NO_WRAP | LAYOUT_SIDE_TOP | LAYOUT_FILL_X);
    FXToolBarShell *shell = new FXToolBarShell(this, FRAME_RAISED);
    FXMenuBar *toolbar = new FXMenuBar(topdock, shell,
                                       LAYOUT_DOCK_SAME | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | FRAME_RAISED);
    new FXToolBarGrip(toolbar, toolbar, FXMenuBar::ID_TOOLBARGRIP, TOOLBARGRIP_DOUBLE);

    FXVerticalFrame *frame = new FXVerticalFrame(this, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0,
                                                 0, 0, 0, 4, 4);

    FXMDIClient *mainframe = new FXMDIClient(frame, LAYOUT_FILL_X | LAYOUT_FILL_Y);
    FXMDIMenu* mdimenu=new FXMDIMenu(this,mainframe);
    new FXMDIWindowButton(menubar, mdimenu, mainframe, FXMDIClient::ID_MDI_MENUWINDOW, LAYOUT_LEFT | LAYOUT_CENTER_Y);
    new FXMDIDeleteButton(menubar, mainframe, FXMDIClient::ID_MDI_MENUCLOSE,
                          FRAME_RAISED | LAYOUT_RIGHT | LAYOUT_CENTER_Y);
    new FXMDIRestoreButton(menubar, mainframe, FXMDIClient::ID_MDI_MENURESTORE,
                           FRAME_RAISED | LAYOUT_RIGHT | LAYOUT_CENTER_Y);
    new FXMDIMinimizeButton(menubar, mainframe, FXMDIClient::ID_MDI_MENUMINIMIZE,
                            FRAME_RAISED | LAYOUT_RIGHT | LAYOUT_CENTER_Y);

    FXMDIChild *childframe = new FXMDIChild(mainframe, "Penis");
    childframe->setFocus();

    mainframe->setActiveChild(childframe);

    statusbar = new FXExStatusBar(frame);
    statusbar->setStatusBarLabel("Initialized...", 0);
}

Editor::~Editor() {

}

void Editor::create() {
    FXMainWindow::create();

    show(PLACEMENT_SCREEN);
}

void Editor::createMenuBar() {
    menubar = new FXMenuBar(this, FRAME_RAISED | LAYOUT_SIDE_TOP | LAYOUT_FILL_X);

    FXMenuPane *menufile = new FXMenuPane(this);
    new FXMenuTitle(menubar, "&File", NULL, menufile);

    FXMenuPane *menuhelp = new FXMenuPane(this);
    new FXMenuTitle(menubar, "&Help\tF1", NULL, menuhelp);
}

////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
    plInitialize(PL_SUBSYSTEM_GRAPHICS | PL_SUBSYSTEM_IMAGE | PL_SUBSYSTEM_LOG);

    plClearLog(EDITOR_LOG);
    //plWriteLog(LAUNCHER_LOG, "Launcher (Interface Version %i)\n", ENGINE_VERSION_INTERFACE);

    // Initialize.
    if (!engine::Initialize(argc, argv)) {
        plWriteLog(EDITOR_LOG, "Engine failed to initialize, check engine log!\n");
        plMessageBox(EDITOR_TITLE, "Failed to initialize engine!");
        return -1;
    }

    FXApp app(EDITOR_TITLE, "Xenon");
    app.init(argc, argv);

    new Editor(&app);

    app.create();

    return app.run();
}
