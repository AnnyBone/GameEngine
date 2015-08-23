#include "EngineBase.h"

#include "EngineException.h"

CEngineException::CEngineException(const char *ccMessage)
{

	Sys_Error(ccMessage);
}
