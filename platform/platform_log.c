/*	Copyright (C) 2011-2016 OldTimes Software

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

#include "platform.h"

/*
	Log System
*/

#define	LOG_FILE_EXTENSION	".log"

void pLog_Write(const char *ccPath, const char *ccMessage, ...)
{
	pFUNCTION_START

	FILE *fLog;
	va_list vlArguments;
	static char scData[1024];
	char cPath[PLATFORM_MAX_PATH];
	unsigned int uiData;

	sprintf(cPath, "%s"LOG_FILE_EXTENSION, ccPath);

	va_start(vlArguments, ccMessage);
	vsprintf(scData, ccMessage, vlArguments);
	va_end(vlArguments);

	uiData = strlen(scData);

	fLog = fopen(cPath, "a");
	if (fwrite(scData, sizeof(char), uiData, fLog) != uiData)
		pSetError("Failed to write to log! (%s)", cPath);
	fclose(fLog);

	pFUNCTION_END
}

void pLog_Clear(const char *ccPath)
{
	pFUNCTION_START

	char cPath[PLATFORM_MAX_PATH];

	sprintf(cPath, "%s"LOG_FILE_EXTENSION, ccPath);

#ifdef _WIN32
	_unlink(cPath);
#else
	unlink(cPath);
#endif

	pFUNCTION_END
}
