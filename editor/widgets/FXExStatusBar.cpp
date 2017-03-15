#include "Editor.h"

#include "FXExStatusBar.h"

FXIMPLEMENT(FXExStatusBar, FXFrame, NULL, NULL);

FXExStatusBar::FXExStatusBar(FXComposite *p, FXint slots) {
    statusbar = new FXHorizontalFrame(p, LAYOUT_BOTTOM | JUSTIFY_LEFT | LAYOUT_FILL_X | FRAME_RAISED, 0, 0, 0, 0, 2, 2,
                                      2, 2);
    FXHorizontalFrame *hframea = new FXHorizontalFrame(statusbar,
                                                       LAYOUT_LEFT | JUSTIFY_LEFT | LAYOUT_FILL_X | FRAME_SUNKEN,
                                                       0, 0, 0, 0, 2, 2, 2, 2);
    label0 = new FXLabel(hframea, "", 0, LAYOUT_LEFT | JUSTIFY_LEFT, 0, 0, 0, 0, 1, 1, 1, 1);

    if (slots > 1) {
        FXHorizontalFrame *hframeb = new FXHorizontalFrame(statusbar,
                                                           LAYOUT_LEFT | JUSTIFY_LEFT | LAYOUT_FILL_X | FRAME_SUNKEN,
                                                           0, 0, 0, 0, 2, 2, 2, 2);
        label1 = new FXLabel(hframeb, "", 0, LAYOUT_LEFT | JUSTIFY_LEFT, 0, 0, 0, 0, 1, 1, 1, 1);
    }

    if (slots > 2) {
        FXHorizontalFrame *hframec = new FXHorizontalFrame(statusbar,
                                                           LAYOUT_LEFT | JUSTIFY_LEFT | LAYOUT_FILL_X | FRAME_SUNKEN,
                                                           0, 0, 0, 0, 2, 2, 2, 2);
        label2 = new FXLabel(hframec, "", 0, LAYOUT_LEFT | JUSTIFY_LEFT, 0, 0, 0, 0, 1, 1, 1, 1);
    }

    corner = new FXDragCorner(statusbar);
}

FXExStatusBar::~FXExStatusBar() {

}

void FXExStatusBar::create() {
    FXWindow::create();
}

void FXExStatusBar::setStatusBarLabel(const char *msg, FXint slot) {
    switch(slot) {
        default:
        case 0: label0->setText(msg); break;
        case 1: label1->setText(msg); break;
        case 2: label2->setText(msg); break;
    }
}

FXint FXExStatusBar::getDefaultWidth() {
    return FXFrame::getDefaultWidth();
}

FXint FXExStatusBar::getDefaultHeight() {
    return 24;
}
