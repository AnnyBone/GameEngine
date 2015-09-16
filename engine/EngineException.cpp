#include "EngineBase.h"

CEngineException::CEngineException(const char *ccMessage, ...)
{
	va_list	ArgPtr;
	char Out[1024];
	static int	
		ErrorPass0 = 0,
		ErrorPass1 = 0,
		ErrorPass2 = 0,
		ErrorPass3 = 0;

#ifdef _DEBUG
	assert(ccMessage);
#endif

	if (!ErrorPass3)
		ErrorPass3 = 1;

	va_start(ArgPtr, ccMessage);
	vsprintf(Out, ccMessage, ArgPtr);
	va_end(ArgPtr);

	pLog_Write(ENGINE_LOG, "Error: %s", Out);

	// switch to windowed so the message box is visible, unless we already
	// tried that and failed
	if (!ErrorPass0)
	{
		ErrorPass0 = 1;

		gWindow_MessageBox("Fatal Error", Out);
	}
	else
		gWindow_MessageBox("Double Fatal Error", Out);

	if (!ErrorPass1)
	{
		ErrorPass1 = 1;

		Host_Shutdown();
	}
}
