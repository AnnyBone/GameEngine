/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#pragma once

#include "platform_log.h"

#include <fox-1.6/fx.h>

#define MODELVIEWER_WINDOW_TITLE    "Xenon Model Viewer"
#define MODELVIEWER_WINDOW_WIDTH    640
#define MODELVIEWER_WINDOW_HEIGHT   640

class ModelViewerWindow : public FXMainWindow {

    FXDECLARE(ModelViewerWindow)

public:
    ModelViewerWindow(FXApp *a);
    ModelViewerWindow() {}
    virtual ~ModelViewerWindow();

    virtual void create();

    enum
    {
        ID_CANVAS   = FXMainWindow::ID_LAST,
        ID_LAST
    };

protected:
private:

    FXCanvas *_canvas;

};
