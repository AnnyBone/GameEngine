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

#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H

#ifdef _WIN32
#else	// Linux
#	include <X11/Xlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

	extern void plMessageBox(const char *title, const char *msg, ...);
	extern void plShowCursor(bool show);
	extern void plGetCursorPosition(int *x, int *y);

	extern int plGetScreenWidth(void);	// Returns width of current screen.
	extern int plGetScreenHeight(void);	// Returns height of current screen.
	extern int plGetScreenCount(void);	// Returns number of avaliable screens.

#ifdef __cplusplus
}
#endif

#endif // !PLATFORM_WINDOW_H
