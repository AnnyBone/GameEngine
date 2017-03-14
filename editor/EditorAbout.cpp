
#include "Editor.h"
#include "EditorAbout.h"

FXIMPLEMENT(EditorAbout, FXDialogBox, NULL, NULL)

EditorAbout::EditorAbout(FXMainWindow *parent)
        : FXDialogBox(parent, "About", DECOR_TITLE|DECOR_BORDER|DECOR_RESIZE) {

}

EditorAbout::~EditorAbout() {

}

void EditorAbout::create() {
    FXDialogBox::create();
}
