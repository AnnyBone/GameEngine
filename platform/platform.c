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

#include "platform_system.h"


/*
	Generic functions for platform, such as	error handling.

	TODO:
		Introduce generic return types (e.g. PLATFORM_ERROR_FILENOTFOUND)
*/

#define	MAX_FUNCTION_LENGTH	64
#define	MAX_ERROR_LENGTH	2048

char 
	sys_error[MAX_ERROR_LENGTH],
	loc_error[MAX_ERROR_LENGTH],
	loc_function[MAX_FUNCTION_LENGTH];

/*	Sets the name of the currently entered function.
*/
void plSetErrorFunction(const char *function, ...)
{
	char	out[MAX_FUNCTION_LENGTH];
	va_list args;

	va_start(args, function);
	vsprintf(out, function, args);
	va_end(args);

	p_strncpy(loc_function, out, sizeof(function));
}

void plResetError(void)
{
	// Set everything to "null".
	sprintf(loc_error, "null");
	sprintf(sys_error, "null");
	sprintf(loc_function, "null");
}

/*	Sets the local error message.
*/
void plSetError(const char *msg, ...)
{
	char	out[MAX_ERROR_LENGTH];
	va_list args;

	va_start(args, msg);
	vsprintf(out, msg, args);
	va_end(args);

	p_strncpy(loc_error, out, sizeof(loc_error));
}

/*	Returns the locally generated error message.
*/
char *plGetError(void)
{
	return loc_error;
}

/*	Returns a system error message.
*/
char *plGetSystemError(void)
{
#ifdef _WIN32
	char	*buffer = NULL;
	int		error;

	error = GetLastError();
	if (error == 0)
		return "Unknown system error!";

	if (!FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&buffer,
		0, NULL))
		return "Failed to get system error details!";

	p_strcpy(sys_error, _strdup(buffer));

	LocalFree(buffer);

	return sys_error;
#else
	p_strcpy(sys_error,dlerror());
	return sys_error;
#endif
}
