
#pragma once

#include "cmdlib.h"

#define	MAXTOKEN	1024

PL_EXTERN_C

extern	char	cToken[MAXTOKEN];
extern	int		iScriptLine;

void Script_StartTokenParsing(char *cData);

bool	Script_GetToken(bool bCrossline);
bool	Script_TokenAvaliable(void);

PL_EXTERN_C_END
