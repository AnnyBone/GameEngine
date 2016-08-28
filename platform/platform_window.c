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

#include "platform_window.h"

/*	Simple Window/Display Handling	*/

PLuint plGetScreenWidth(PLvoid)
{
#ifdef _WIN32
	return GetSystemMetrics(SM_CXSCREEN);
#else
	Display *display = XOpenDisplay(NULL);
	if (!display)
	{
		plSetError("Failed to open display!\n");
		return 4000;
	}

	Screen *screen = DefaultScreenOfDisplay(display);
	if (!screen)
	{
		plSetError("Failed to get screen of display!\n");
		return 4000;
	}

	return screen->width;
#endif
}

PLuint plGetScreenHeight(PLvoid)
{
	pFUNCTION_START
#ifdef _WIN32
	return GetSystemMetrics(SM_CYSCREEN);
#else
	Display *display = XOpenDisplay(NULL);
	if (!display)
	{
		plSetError("Failed to open display!\n");
		return 4000;
	}

	Screen *screen = DefaultScreenOfDisplay(display);
	if (!screen)
	{
		plSetError("Failed to get screen of display!\n");
		return 4000;
	}

	return screen->height;
#endif
	pFUNCTION_END
}

#ifdef __linux__
Display *dMainDisplay;
Window  wRootWindow;
#endif

PLint plGetScreenCount(PLvoid)
{
#ifdef _WIN32
	return GetSystemMetrics(SM_CMONITORS);
#else
	return XScreenCount(dMainDisplay);
#endif
}

/*
	Window Creation
*/

PL_INSTANCE iGlobalInstance;

int	iActive = 0,	// Number of current active windows.
iScreen;			// Default screen.

/*	Create a new window.
*/
void plCreateWindow(plWindow_t *window)
{
	plFunctionStart();

	// Make sure the window has been initialized.
	if (!window)
	{
		plSetError("Window has not been allocated!\n");
		return;
	}
#if 0
	// Make sure that any platform specific window systems are set up.
	else if()
	{
#ifdef __linux__
		dMainDisplay = XOpenDisplay(NULL);
		if(!dMainDisplay)
		{
			plSetError("Failed to open display!\n");
			return;
		}

		iScreen = DefaultScreen(dMainDisplay);
#else
#endif
	}
#endif

#ifdef _WIN32
	{
	    WNDCLASSEX wWindowClass;

		wWindowClass.cbClsExtra		= 0;
		wWindowClass.cbSize			= sizeof(WNDCLASSEX);
		wWindowClass.cbWndExtra		= 0;
		wWindowClass.hbrBackground	= NULL;
		wWindowClass.hCursor		= LoadCursor(iGlobalInstance,IDC_ARROW);
		wWindowClass.hIcon			= LoadIcon(iGlobalInstance,IDI_APPLICATION);
		wWindowClass.hIconSm		= 0;
		wWindowClass.hInstance		= iGlobalInstance;
		wWindowClass.lpfnWndProc	= NULL;	// (WNDPROC)Platform_WindowProcedure;
		wWindowClass.lpszClassName	= window->classname;
		wWindowClass.lpszMenuName	= 0;
		wWindowClass.style			= CS_OWNDC;

		if(!RegisterClassEx(&wWindowClass))
		{
			plSetError("Failed to register window class!\n");
			return;
		}

		window->instance = CreateWindowEx(
			0,
			window->classname,
			window->title,
			WS_OVERLAPPED|WS_BORDER|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
			window->x,
			window->y,
			window->width,
			window->height,
			NULL,NULL,
			iGlobalInstance,
			NULL);
		if (!window->instance)
		{
			plSetError("Failed to create window!\n");
			return;
		}

		UpdateWindow(window->instance);
		SetForegroundWindow(window->instance);

		window->dc = GetDC(window->instance);
	}
#else	// Linux
	{
		// Create our window.
		window->wInstance = XCreateSimpleWindow(
			dMainDisplay,
			RootWindow(dMainDisplay,iScreen),
			window->x,
			window->y,
			window->width,
			window->height,
			1,
			BlackPixel(dMainDisplay,iScreen),
			WhitePixel(dMainDisplay,iScreen));
		if(!window->wInstance)
		{
			plSetError("Failed to create window!\n");
			return;
		}

		// Set the window title.
		XStoreName(dMainDisplay,window->wInstance,window->title);
	}
#endif

	window->is_active = true;
}

/*  
*/
PLvoid plShowCursor(PLbool show)
{
	static bool _cursorvisible = true;
	if (show == _cursorvisible)
		return;
#ifdef _WIN32
	ShowCursor(show);
#else	// Linux
#endif
	_cursorvisible = show;
}

/*	Gets the position of the cursor.
	TODO:
		Move into platform_input.
*/
void plGetCursorPosition(int *x, int *y)
{
#ifdef _WIN32
	POINT	pPoint;
	GetCursorPos(&pPoint);
	*x = pPoint.x; *y = pPoint.y;
#else	// Linux
#endif
}

/*	Displays a simple dialogue window.
*/
void plMessageBox(const char *ccTitle, const char *ccMessage, ...)
{
	char	cOut[2048];
	va_list	vlArguments;

	plFunctionStart();

	va_start(vlArguments, ccMessage);
	vsprintf(cOut, ccMessage, vlArguments);
	va_end(vlArguments);

	// Also print a message out, on the off chance the message box fails.
	printf("Platform: %s", cOut);

#ifndef _WIN32
	{
		int     iDefaultScreen;
		XEvent  xEvent;
		Display *dMessageDisplay;
		Window  wMessageWindow;

		dMessageDisplay = XOpenDisplay(NULL);
		if (!dMessageDisplay)
		{
			plSetError("Failed to open display!\n");
			return;
		}

		iDefaultScreen = DefaultScreen(dMessageDisplay);

		wMessageWindow = XCreateSimpleWindow(
			dMessageDisplay,
			RootWindow(dMessageDisplay, iDefaultScreen),
			50, 50,
			512, 64,
			1,
			BlackPixel(dMessageDisplay, iDefaultScreen),
			WhitePixel(dMessageDisplay, iDefaultScreen));
		XStoreName(dMessageDisplay, wMessageWindow, ccTitle);
		XSelectInput(dMessageDisplay, wMessageWindow, ExposureMask | KeyPressMask);
		XMapWindow(dMessageDisplay, wMessageWindow);

		for (;;)
		{
			XNextEvent(dMessageDisplay, &xEvent);

			if (xEvent.type == Expose)
			{
				XDrawString(dMessageDisplay, wMessageWindow, DefaultGC(dMessageDisplay, iDefaultScreen), 10, 10, cOut, strlen(cOut));
				XDrawString(dMessageDisplay, wMessageWindow, DefaultGC(dMessageDisplay, iDefaultScreen), 10, 54, "Press any key to continue...", 32);
			}
			else if (xEvent.type == KeyPress)
				break;
		}

		XCloseDisplay(dMessageDisplay);
	}
#else   // Windows
	MessageBoxEx(NULL, cOut, ccTitle, MB_SETFOREGROUND | MB_ICONERROR, 0);
#endif
}

void plSwapBuffers(plWindow_t *window)
{
#ifdef _WIN32
	SwapBuffers(window->dc);
#else	// Linux
	//glXSwapBuffers() // todo
#endif
}
