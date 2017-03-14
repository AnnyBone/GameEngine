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

#include <platform_image.h>
#include "Editor.h"

using namespace xenon;

class Editor : public FXMainWindow {
    FXDECLARE(Editor)

public:
    Editor(FXApp *app);
    virtual ~Editor();

    virtual void create();

    void createMenuBar();
    void createStatusBar();

    void setStatusBarLabel(const char *text, int slot);

protected:
    Editor() {}

private:
    FXMenuBar *menubar_;

    FXHorizontalFrame *statusbar_;
    FXLabel *statusbar_label0_, *statusbar_label1_;
};

FXIMPLEMENT(Editor, FXMainWindow, NULL, NULL)

Editor::Editor(FXApp *app) :
        FXMainWindow(app, EDITOR_TITLE, NULL, NULL, DECOR_ALL, 0, 0, 1024, 768) {

    createMenuBar();
    createStatusBar();

    new FXToolTip(app, 0);
}

Editor::~Editor() {

}

void Editor::create() {
    FXMainWindow::create();

    show(PLACEMENT_SCREEN);
}

void Editor::createMenuBar() {
    menubar_ = new FXMenuBar(this, FRAME_RAISED|LAYOUT_SIDE_TOP|LAYOUT_FILL_X);
}

void Editor::createStatusBar() {
    PLImage image;
    plLoadImage("", &image);

    statusbar_ = new FXHorizontalFrame(this, LAYOUT_BOTTOM|JUSTIFY_LEFT|LAYOUT_FILL_X|FRAME_RAISED, 0, 0, 0, 0, 2, 2, 2, 2);
    FXHorizontalFrame *hframea = new FXHorizontalFrame(statusbar_, LAYOUT_LEFT|JUSTIFY_LEFT|LAYOUT_FILL_X|FRAME_SUNKEN,
                                                      0, 0, 0, 0, 2, 2, 2, 2);
    FXHorizontalFrame *hframeb = new FXHorizontalFrame(statusbar_, LAYOUT_LEFT|JUSTIFY_LEFT|LAYOUT_FILL_X|FRAME_SUNKEN,
                                                      0, 0, 0, 0, 2, 2, 2, 2);
    FXHorizontalFrame *hframec = new FXHorizontalFrame(statusbar_, LAYOUT_LEFT|JUSTIFY_LEFT|LAYOUT_FILL_X|FRAME_SUNKEN,
                                                      0, 0, 0, 0, 2, 2, 2, 2);
    statusbar_label0_ = new FXLabel(hframea, "This is slot a", 0, LAYOUT_LEFT|JUSTIFY_LEFT, 0, 0, 0, 0, 2, 2, 2, 2);
    statusbar_label1_ = new FXLabel(hframeb, "This is slot b", 0, LAYOUT_LEFT|JUSTIFY_LEFT, 0, 0, 0, 0, 2, 2, 2, 2);
    new FXBMPIcon(getApp(), image.data[0]);
}

void Editor::setStatusBarLabel(const char *text, int slot) {
    switch(slot) {
        case 0: {
            statusbar_label0_->setText(text);
            break;
        }
        case 1: {
            statusbar_label1_->setText(text);
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////

int main(int argc,char *argv[]) {
    plInitialize(PL_SUBSYSTEM_GRAPHICS|PL_SUBSYSTEM_IMAGE|PL_SUBSYSTEM_LOG);

    plClearLog(EDITOR_LOG);
    //plWriteLog(LAUNCHER_LOG, "Launcher (Interface Version %i)\n", ENGINE_VERSION_INTERFACE);

    // Initialize.
    if (!engine::Initialize(argc, argv))	{
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
