/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
	Copyright (C) 2011-2016 OldTimes Software

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "engine_base.h"

#include "EngineScript.h"
#include "EngineGame.h"

/*
	Script Library
*/

char	cToken[MAXTOKEN],
		*cScriptParse;

int		iScriptLine;

void Script_StartTokenParsing(char *cData)
{
	iScriptLine		= 1;
	cScriptParse	= cData;
}

/*	Copied from hmap2, since the original was pretty fucked up.
*/
bool Script_GetToken(bool bCrossline)
{
	char *cTokenParse,*cTemp;

	// Return something, even if the caller ignores a return false.
	cToken[0] = 0;

	for (;;)
	{
		if (*cScriptParse == 0)
			return false;
		else if (*cScriptParse == '\n')
		{
			if (!bCrossline)
				return false;

			cScriptParse++;
			iScriptLine++;
		}
		else if (*cScriptParse <= ' ')
			cScriptParse++;
		else if (cScriptParse[0] == '/' && cScriptParse[1] == '/')
		{
			while (*cScriptParse && (*cScriptParse != '\n'))
				cScriptParse++;
		}
		else if (cScriptParse[0] == '/' && cScriptParse[1] == '*')
		{
			cTemp = cScriptParse;

			for (;;)
			{
				if (*cScriptParse == 0)
					break;
				else if (cScriptParse[0] == '*' && cScriptParse[1] == '/')
				{
					cScriptParse += 2;
					break;
				}
				else if (*cScriptParse == '\n')
				{
					if (!bCrossline)
					{
						cScriptParse = cTemp;
						return false;
					}

					cScriptParse++;
					iScriptLine++;
				}
				else
					cScriptParse++;
			}
		}
		else
			break;
	}

	cTokenParse = cToken;

	if(*cScriptParse == '"')
	{
		cScriptParse++;
		while(*cScriptParse != '"')
		{
			if(!*cScriptParse)
				Error("End of field inside quoted token! (%i)",iScriptLine);

			*cTokenParse++ = *cScriptParse++;

			if(cTokenParse == &cToken[MAXTOKEN])
				Error("Token too large! (%i)\n",iScriptLine);
		}

		cScriptParse++;
	}
	else if(*cScriptParse == '(')
	{
		cScriptParse++;

		while (*cScriptParse != ')')
		{
			if (!*cScriptParse)
				Error("End of field inside quoted token! (%i)", iScriptLine);

			*cTokenParse++ = *cScriptParse++;

			if (cTokenParse == &cToken[MAXTOKEN])
				Error("Token too large! (%i)\n", iScriptLine);
		}

		cScriptParse++;
	}
	else while(*cScriptParse > 32)
	{
		*cTokenParse++ = *cScriptParse++;
		if(cTokenParse == &cToken[MAXTOKEN])
			Error("Token too large! (%i)",iScriptLine);
	}

	*cTokenParse = 0;

	return true;
}

bool Script_TokenAvaliable(void)
{
	char	*cSearchParse;

	cSearchParse = cScriptParse;
	while(*cSearchParse <= 32)
	{
		if(*cSearchParse == '\n' || *cSearchParse == 0)
			return false;

		cSearchParse++;
	}

	if(*cSearchParse == ';')
		return false;

	return true;
}

/*	Code originally by itsme86 http://www.linuxquestions.org
	Fixes by paulhope123.
*/
char *Script_UpdateString(char *cString,const char *cFind,const char *cReplace)
{
	static char scBuffer[4096];
	char		*p;

	p = strstr(cString,cFind);
	if(!p)
		return cString;

	if(strlen(cFind) >= strlen(cReplace))
	{
		strncpy(scBuffer, cString, p - cString);
		scBuffer[p-cString]	= '\0';
	}
	else
	{
		strncpy(scBuffer, cString, p - cString + strlen(cFind));
		scBuffer[p-cString+strlen(cFind)] = '\0';
	}

	sprintf(scBuffer+(p-cString),"%s%s",cReplace,p+strlen(cFind));

	return scBuffer;
}

/*
	New stuff
*/

typedef struct
{
	char	*cKey;

	void	(*Function)(char *cArg);
} ScriptKey_t;

void _FileSystem_SetBasePath(char *cArg);		// common.c
void _FileSystem_SetTexturePath(char *cArg);
void _FileSystem_SetMaterialPath(char *cArg);
void _FileSystem_SetSoundPath(char *cArg);		// common.c
void _FileSystem_SetLevelPath(char *cArg);		// common.c
void _FileSystem_SetScreenshotPath(char *cArg);	// common.c
void _FileSystem_SetFontPath(char *cArg);		// common.c
void _FileSystem_AddGameDirectory(char *cArg);	// common.c
void _FileSystem_SetModulePath(char *cArg);
void _FileSystem_SetShaderPath(char *cArg);

ScriptKey_t	skScriptKeys[]=
{
	// TODO: Make these obsolete
	{ "SetBasePath", _FileSystem_SetBasePath },
	{ "SetTexturePath", _FileSystem_SetTexturePath },
	{ "SetMaterialPath", _FileSystem_SetMaterialPath },
	{ "SetSoundPath", _FileSystem_SetSoundPath },
	{ "SetLevelPath", _FileSystem_SetLevelPath },
	{ "SetScreenshotPath", _FileSystem_SetScreenshotPath },
	{ "AddGameDirectory", _FileSystem_AddGameDirectory },
	{ "SetFontPath", _FileSystem_SetFontPath },
	{ "SetModulePath", _FileSystem_SetModulePath },
	{ "SetShaderPath", _FileSystem_SetShaderPath },

	{ "path_base", _FileSystem_SetBasePath },
	{ "path_textures", _FileSystem_SetTexturePath },
	{ "path_materials", _FileSystem_SetMaterialPath },
	{ "path_sounds", _FileSystem_SetSoundPath },
	{ "path_levels", _FileSystem_SetLevelPath },
	{ "path_screenshots", _FileSystem_SetScreenshotPath },
	{ "path_modules", _FileSystem_SetModulePath },
	{ "path_shaders", _FileSystem_SetShaderPath },
	{ "path_fonts", _FileSystem_SetFontPath },
	{ "path_add", _FileSystem_AddGameDirectory },

	{	0	}
};

/*	Loads a script.
*/
bool Script_Load(const char *ccPath)
{
	char *data = COM_LoadHeapFile(ccPath);
	if (!data)
	{
		Con_Warning("Failed to load script! (%s)\n",ccPath);
		return false;
	}

	Script_StartTokenParsing(data);

	if(!Script_GetToken(true))
	{
		Con_Warning("Failed to get initial token! (%s) (%i)\n",ccPath,iScriptLine);

		free(data);
		return false;
	}
	else if(strcmp(cToken, "{"))
	{
		Con_Warning("Missing '{'! (%s) (%i)\n",ccPath,iScriptLine);

		free(data);
		return false;
	}

	do
	{
		if(!Script_GetToken(true))
		{
			Con_Warning("End of field without closing brace! (%s) (%i)\n",ccPath,iScriptLine);
			break;
		}
		else if(!strcmp(cToken, "}"))
			break;
		else if(strcmp(cToken, "{"))
		{
			ScriptKey_t	*sKey;

			// '$' declares that the following is a function.
			if(cToken[0] == SCRIPT_SYMBOL_FUNCTION)
			{
				for(sKey = skScriptKeys; sKey->cKey; sKey++)
					// Remain case sensitive.
					if (!strcasecmp(sKey->cKey, cToken + 1))
					{
						Script_GetToken(false);

						sKey->Function(cToken);
						break;
					}
			}
			else
			{
				Con_Warning("Invalid field! (%s) (%i)\n", ccPath, iScriptLine);
				break;
			}
		}
	} while(true);

	free(data);
	return true;
}