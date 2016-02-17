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

#include "engine_base.h"

#include "video.h"
#include "EngineGame.h"

#include <SDL.h>
#include <SDL_syswm.h>

/*
	Window Manager
*/

SDL_Window		*sMainWindow;
SDL_GLContext	sMainContext;
SDL_DisplayMode	sDisplayMode;
SDL_SysWMinfo	sSystemInfo;

GrContext_t	window_glidecontext;

plWindow_t g_mainwindow;

void Window_InitializeVideo(void)
{
	int	iFlags =
		SDL_WINDOW_SHOWN |
		SDL_WINDOW_FULLSCREEN;
	SDL_Surface	*icon;

	// [28/7/2013] Moved check here and corrected, seems more secure ~hogsy
	if (SDL_VideoInit(NULL) < 0)
		Sys_Error("Failed to initialize video!\n%s\n", SDL_GetError());

	SDL_DisableScreenSaver();

	// Get display information.
	if (SDL_GetCurrentDisplayMode(0, &sDisplayMode) != 0)
		Sys_Error("Failed to get current display information!\n%s\n", SDL_GetError());

	if (!Video.fullscreen)
		iFlags &= ~SDL_WINDOW_FULLSCREEN;

	sMainWindow = SDL_CreateWindow(
		Game->Name,				// [9/7/2013] Window name is based on the name given by Game ~hogsy
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		Video.iWidth,
		Video.iHeight,
		iFlags);
	if (!sMainWindow)
		Sys_Error("Failed to create window!\n%s\n", SDL_GetError());

	// Attempt to grab the window icon from the game directory.
	icon = SDL_LoadBMP(va("%s/icon.bmp", com_gamedir));
	if (icon)
	{
		// [25/3/2014] Set the transparency key... ~hogsy
		SDL_SetColorKey(icon, true, SDL_MapRGB(icon->format, 0, 0, 0));
		SDL_SetWindowIcon(sMainWindow, icon);
		SDL_FreeSurface(icon);
	}
	else
		// Give us a warning, but continue.
		Con_Warning("Failed to load window icon! (%s)\n", SDL_GetError());

#ifdef _WIN32
	if (SDL_GetWindowWMInfo(sMainWindow, &sSystemInfo))
		g_mainwindow.instance = sSystemInfo.info.win.window;
	else
		Con_Warning("Failed to get WM information! (%s)\n", SDL_GetError());
#endif
	
	grGet(GR_NUM_BOARDS, sizeof(Video.hardware.num_cards), (FxI32*)&Video.hardware.num_cards);
	if (Video.hardware.num_cards == 0)
		Sys_Error("No Glide capable hardware detected!\n");

	// Initialize Glide.
	grGlideInit();

	// Glide only supports a limited set of resolutions.
	int res;
	if (g_mainwindow.instance)
		res = GR_RESOLUTION_NONE;
	else
	{
		if ((Video.iWidth > 1024) && (Video.iHeight > 768))			res = GR_RESOLUTION_MAX;
		else if ((Video.iWidth == 1024) && (Video.iHeight == 768))	res = GR_RESOLUTION_1024x768;
		else if ((Video.iWidth == 800) && (Video.iHeight == 600))	res = GR_RESOLUTION_800x600;
		else														res = GR_RESOLUTION_640x480;
	}
	
	// Select the graphics card.
	grSstSelect(0);
	window_glidecontext = grSstWinOpen(
		(FxU32)g_mainwindow.instance,
		res,
		GR_REFRESH_60Hz,
		GR_COLORFORMAT_RGBA,
		GR_ORIGIN_UPPER_LEFT,
		2,
		1);
	if (!window_glidecontext)
		Sys_Error("Failed to create window!\n");

	grClipWindow(0, 0, Video.iWidth, Video.iHeight);
}

void Window_UpdateVideo(void)
{
	if (Video.msaa_samples != cv_video_msaasamples.iValue)
	{
		// TODO: Destroy window etc.

		Video.msaa_samples = cv_video_msaasamples.iValue;
	}

	SDL_SetWindowSize(sMainWindow, Video.iWidth, Video.iHeight);

	if (Video.vertical_sync != cv_video_verticlesync.bValue)
	{
		SDL_GL_SetSwapInterval(cv_video_verticlesync.iValue);

		Video.vertical_sync = cv_video_verticlesync.bValue;
	}

	if (Video.fullscreen != cv_video_fullscreen.bValue)
	{
		if (SDL_SetWindowFullscreen(sMainWindow, (SDL_bool)cv_video_fullscreen.bValue) == -1)
		{
			Con_Warning("Failed to set window mode!\n%s", SDL_GetError());

			// Reset the variable to the current value.
			Cvar_SetValue(cv_video_fullscreen.name, Video.fullscreen);
		}
		else
			Video.fullscreen = cv_video_fullscreen.bValue;
	}

	if (!cv_video_fullscreen.value)
		// Center the window, to ensure it's not off screen.
		SDL_SetWindowPosition(sMainWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void Window_Swap(void)
{
	/*	"To gracefully handle the loss of resources 
		(e.g. to another 3D application being scheduled 
		by the Windows 95 operating system), an application 
		is required to periodically (typically once per frame)
		query with grSelectContext() to determine if Glide’s resources 
		have be reallocated by the system. context is a context 
		handle returned from a successful call to grWinOpen()."
	*/
	if(!grSelectContext(window_glidecontext))
	{ }

	grBufferSwap(0);
}

/*	Set the gamma level.
Based on Darkplaces implementation.
*/
void Window_SetGamma(unsigned short *usRamp, int iRampSize)
{
	if (!SDL_SetWindowGammaRamp(sMainWindow, usRamp, usRamp + iRampSize, usRamp + iRampSize * 2))
		Con_Warning("Failed to set gamma level!\n%s", SDL_GetError());
}

/*	Get gamma level.
Based on the Darkplaces implementation.
*/
void Window_GetGamma(unsigned short *usRamp, int iRampSize)
{
	if (!SDL_GetWindowGammaRamp(sMainWindow, usRamp, usRamp + iRampSize, usRamp + iRampSize * 2))
		Con_Warning("Failed to get gamma level!\n%s", SDL_GetError());
}

int Window_GetWidth(void)
{
	return sDisplayMode.w;
}

int Window_GetHeight(void)
{
	return sDisplayMode.h;
}

void Window_GetCursorPosition(int *x, int *y)
{
	SDL_GetMouseState(x, y);
}

void Window_Shutdown(void)
{
	if (window_glidecontext)
		// Delete the current context.
		grSstWinClose(window_glidecontext);

	if (sMainWindow)
		// Destory our window.
		SDL_DestroyWindow(sMainWindow);

	// Quit the SDL subsystem.
	SDL_QuitSubSystem(SDL_INIT_VIDEO);

	grGlideShutdown();
}
