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

#include "platform_log.h"

/*
	Log System
*/

#define	LOG_FILE_EXTENSION	".log"

void plWriteLog(const char *path, const char *msg, ...)
{
	pFUNCTION_START

	FILE			*file;
	va_list			args;
	static char		buffer[1024];
	char			newpath[PLATFORM_MAX_PATH];
	unsigned int	size;

	sprintf(newpath, "%s"LOG_FILE_EXTENSION, path);

	va_start(args, msg);
	vsnprintf(buffer, sizeof(buffer), msg, args);
	va_end(args);

	size = strlen(buffer);

	file = fopen(newpath, "a");
	if (fwrite(buffer, sizeof(char), size, file) != size)
		plSetError("Failed to write to log! (%s)", newpath);
	fclose(file);

	pFUNCTION_END
}

void plClearLog(const char *path)
{
	pFUNCTION_START

	char newpath[PLATFORM_MAX_PATH];
	sprintf(newpath, "%s"LOG_FILE_EXTENSION, path);
	unlink(newpath);

	pFUNCTION_END
}
