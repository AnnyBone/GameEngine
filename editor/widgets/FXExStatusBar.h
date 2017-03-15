
#pragma once

class FXExStatusBar : public FXFrame {
    FXDECLARE(FXExStatusBar);

public:
    FXExStatusBar(FXComposite *p, FXint slots = 3);
    virtual ~FXExStatusBar();

    virtual FXint getDefaultWidth();
    virtual FXint getDefaultHeight();

    void setStatusBarLabel(const char *msg, FXint slot);

    void create();

protected:
    FXHorizontalFrame *statusbar;
    FXLabel *label0, *label1, *label2;
    FXDragCorner *corner;

private:
    FXExStatusBar() {}
    FXExStatusBar(const FXExStatusBar&) {}
};