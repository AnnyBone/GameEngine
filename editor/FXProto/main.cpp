//
// Created by hogsy on 04/09/16.
//

#include <fox-1.6/fx.h>

class ProtoWindow : public FXMainWindow
{
    FXDECLARE(ProtoWindow)

public:
    ProtoWindow(FXApp *a) : FXMainWindow(a, "Xenon Editor", NULL, NULL, DECOR_ALL, 0, 0, 1024, 1024)
    {
    }

    virtual ~ProtoWindow()
    {

    }

    virtual void create()
    {
        FXMainWindow::create();

        show(PLACEMENT_SCREEN);
    }

protected:
private:
};

FXDEFMAP(ProtoWindow) ProtoWindowMap[]=
        {

        };

FXIMPLEMENT(ProtoWindow, FXMainWindow, ProtoWindowMap, ARRAYNUMBER(ProtoWindowMap))

int main(int argc, char *argv[])
{
    FXApp app("ProtoWindow", "Testy");
    app.init(argc, argv);

    new ProtoWindow(&app);

    app.create();

    return app.run();
}

