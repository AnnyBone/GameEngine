/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#pragma once

#ifdef _WIN32
#else	// Linux
#	include <X11/Xlib.h>
#endif

typedef struct
{
	char	*title,	*classname;

	unsigned int width, height;

	int		x, y;

	bool is_active, is_fullscreen;
	bool is_unlocked;

	int vsync_interval;

#ifdef _WIN32
	HWND	instance;
	HDC		dc;
#else	// Linux
	Window  instance;
#endif
} plWindow_t;

plEXTERN_C_START

extern void plMessageBox(const char *title, const char *msg, ...);
extern void plShowCursor(bool show);
extern void plGetCursorPosition(int *x, int *y);

// Window Management
extern void plCreateWindow(plWindow_t *window);

extern int plGetScreenWidth(void);	// Returns width of current screen.
extern int plGetScreenHeight(void);	// Returns height of current screen.
extern int plGetScreenCount(void);	// Returns number of avaliable screens.

// Rendering
extern void plSwapBuffers(plWindow_t *window);

plEXTERN_C_END
