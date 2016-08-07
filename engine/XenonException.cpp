/*
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

XException::XException(const char *message, ...)
{
	va_list		ArgPtr;
	char		Out[1024];
	static int	ErrorPass0 = 0,
				ErrorPass1 = 0,
				ErrorPass2 = 0;

#ifdef _DEBUG
	assert(message);
#endif

	if (!ErrorPass2)
		ErrorPass2 = 1;

	va_start(ArgPtr, message);
	vsprintf(Out, message, ArgPtr);
	va_end(ArgPtr);

	plWriteLog(ENGINE_LOG, "Error: %s", Out);

	// switch to windowed so the message box is visible, unless we already
	// tried that and failed
	if (!ErrorPass0)
	{
		ErrorPass0 = 1;

		plMessageBox("Fatal Error", Out);
	}
	else
		plMessageBox("Double Fatal Error", Out);

	if (!ErrorPass1)
	{
		ErrorPass1 = 1;

		Host_Shutdown();
	}
}
