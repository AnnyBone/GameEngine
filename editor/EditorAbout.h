
#pragma once

class EditorAbout : public FXDialogBox {
    FXDECLARE(EditorAbout)

public:
    EditorAbout(FXMainWindow *parent);
    virtual ~EditorAbout();

    virtual void create();

private:
    EditorAbout() {}
};
