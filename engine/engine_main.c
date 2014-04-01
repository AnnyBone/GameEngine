#include "engine_main.h"
#include "engine_video.h"

#include "../platform/include/platform_module.h"

EngineExport_t	eExport;
EngineImport_t	Launcher;

/*	On failure returns false.
*/
bool Engine_Initialize(int argc,char *argv[])
{
    double dNewTime,
            dCurrentTime,
            dOldTime = 0;   // [22/7/2013] Set to 0 since we use it on the first frame ~hogsy

	for(;;)
    {
        dNewTime        = Sys_FloatTime();
        dCurrentTime    = dNewTime-dOldTime;

        Host_Frame(dCurrentTime);

        dOldTime = dNewTime;
    }

    return true;
}

// [8/3/2014] Oops, this didn't match! Fixed ~hogsy
bool System_Main(int iArgumentCount,char *cArguments[]);

GIPL_EXPORT EngineExport_t *Engine_Main(EngineImport_t *mImport)
{
	Launcher.iVersion	= mImport->iVersion;

	eExport.Initialize	= System_Main;
	eExport.iVersion	= ENGINE_INTERFACE;

	return &eExport;
}