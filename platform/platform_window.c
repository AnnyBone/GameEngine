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

#include "platform_window.h"

/*
	Window/Display management.
	TODO: BLITZ!!!!!
*/

/*
	Display Information
*/

int	plGetScreenWidth(void)
{
#ifdef _WIN32
	return GetSystemMetrics(SM_CXSCREEN);
#else
	Display *display;
	Screen	*screen;

	display = XOpenDisplay(NULL);
	if (!display)
	{
		plSetError("Failed to open display!\n");
		return 4000;
	}

	screen = DefaultScreenOfDisplay(display);
	if (!screen)
	{
		plSetError("Failed to get screen of display!\n");
		return 4000;
	}

	return screen->width;
#endif
}

int plGetScreenHeight(void)
{
	pFUNCTION_START
#ifdef _WIN32
	return GetSystemMetrics(SM_CYSCREEN);
#else
	Display *display;
	Screen	*screen;

	display = XOpenDisplay(NULL);
	if (!display)
	{
		plSetError("Failed to open display!\n");
		return 4000;
	}

	screen = DefaultScreenOfDisplay(display);
	if (!screen)
	{
		plSetError("Failed to get screen of display!\n");
		return 4000;
	}

	return screen->height;
#endif
	pFUNCTION_END
}

int plGetScreenCount(void)
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

#ifdef __linux__
Display *dMainDisplay;
Window  wRootWindow;
#endif

pINSTANCE iGlobalInstance;

bool is_cursorvisible = true;		// Are we showing the cursor?

int	iActive = 0,	// Number of current active windows.
iScreen;		// Default screen.

typedef struct
{
	char	*cTitle,
		*cClass;

	int		iWidth, iHeight,
		x, y;

	bool	bActive,
		bFullscreen;

#ifdef _WIN32
	HWND	hWindow;
	HDC		hDeviceContext;
#else	// Linux
	Window  wInstance;
#endif
} plWindow_t;

/*	Create a new window.
	TODO:
		Rewrite this...
*/
void plCreateWindow(plWindow_t *window)
{
	pFUNCTION_UPDATE();

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
#if 0
		INITCOMMONCONTROLSEX iCommonControls;

		iCommonControls.dwICC	= 0;
		iCommonControls.dwSize	= sizeof(INITCOMMONCONTROLSEX);

		if(!InitCommonControlsEx(&iCommonControls))
		{
			Platform_SetError("Failed to initialize common controls!\n");

			return;
		}
#endif

		wWindowClass.cbClsExtra		= 0;
		wWindowClass.cbSize			= sizeof(WNDCLASSEX);
		wWindowClass.cbWndExtra		= 0;
		wWindowClass.hbrBackground	= NULL;
		wWindowClass.hCursor		= LoadCursor(iGlobalInstance,IDC_ARROW);
		wWindowClass.hIcon			= LoadIcon(iGlobalInstance,IDI_APPLICATION);
		wWindowClass.hIconSm		= 0;
		wWindowClass.hInstance		= iGlobalInstance;
		wWindowClass.lpfnWndProc	= NULL;	// (WNDPROC)Platform_WindowProcedure;
		wWindowClass.lpszClassName	= window->cClass;
		wWindowClass.lpszMenuName	= 0;
		wWindowClass.style			= CS_OWNDC;

		if(!RegisterClassEx(&wWindowClass))
		{
			plSetError("Failed to register window class!\n");
			return;
		}

		window->hWindow = CreateWindowEx(
			0,
			window->cClass,
			window->cTitle,
			WS_OVERLAPPED|WS_BORDER|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
			window->x,
			window->y,
			window->iWidth,
			window->iHeight,
			NULL,NULL,
			iGlobalInstance,
			NULL);
		if (!window->hWindow)
		{
			plSetError("Failed to create window!\n");
			return;
		}

		ShowWindow(window->hWindow, SW_SHOWDEFAULT);

		UpdateWindow(window->hWindow);

		SetForegroundWindow(window->hWindow);

		window->hDeviceContext = GetDC(window->hWindow);
	}
#else	// Linux
	{
		// Create our window.
		window->wInstance = XCreateSimpleWindow(
			dMainDisplay,
			RootWindow(dMainDisplay,iScreen),
			window->x,
			window->y,
			window->iWidth,
			window->iHeight,
			1,
			BlackPixel(dMainDisplay,iScreen),
			WhitePixel(dMainDisplay,iScreen));
		if(!window->wInstance)
		{
			plSetError("Failed to create window!\n");
			return;
		}

		// Set the window title.
		XStoreName(dMainDisplay,window->wInstance,window->cTitle);
	}
#endif

	window->bActive = true;

	iActive++;
}

void plDestroyWindow(plWindow_t *window)
{
#ifdef _WIN32
	if (!window->hWindow)
		return;

	// Destroy our window.
	DestroyWindow(window->hWindow);
#else	// Linux
	// Close our display instance.
	if(dMainDisplay)
		XCloseDisplay(dMainDisplay);
#endif

	free(window);

	iActive--;
}

/*  Shows or hides the cursor for
	the active window.
*/
void plShowCursor(bool show)
{
	if (show == is_cursorvisible)
		return;
#ifdef _WIN32
	ShowCursor(show);
#else	// Linux
#endif
	is_cursorvisible = show;
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

	pFUNCTION_UPDATE();

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
	SwapBuffers(window->hDeviceContext);
#else	// Linux
	//glXSwapBuffers() // todo
#endif
}
