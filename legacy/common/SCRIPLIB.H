
#pragma once

#include "cmdlib.h"

#define	MAXTOKEN	1024

plEXTERN_C_START

extern	char	cToken[MAXTOKEN];
extern	int		iScriptLine;

void Script_StartTokenParsing(char *cData);

bool	Script_GetToken(bool bCrossline);
bool	Script_TokenAvaliable(void);

plEXTERN_C_END
