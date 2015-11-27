/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
	Copyright (C) 2011-2015 OldTimes Software

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

Window_t g_mainwindow;

void Window_InitializeVideo(void)
{
	int	iFlags =
		SDL_WINDOW_SHOWN |
		SDL_WINDOW_OPENGL |
		SDL_WINDOW_FULLSCREEN;
	SDL_Surface	*sIcon;

	// [28/7/2013] Moved check here and corrected, seems more secure ~hogsy
	if (SDL_VideoInit(NULL) < 0)
		Sys_Error("Failed to initialize video!\n%s\n", SDL_GetError());

	SDL_DisableScreenSaver();

	// Get display information.
	if (SDL_GetCurrentDisplayMode(0, &sDisplayMode) != 0)
		Sys_Error("Failed to get current display information!\n%s\n", SDL_GetError());

	if (!Video.bFullscreen)
		iFlags &= ~SDL_WINDOW_FULLSCREEN;

#if 0
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 8);
#if 0
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
#endif
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

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
	sIcon = SDL_LoadBMP(va("%s/icon.bmp", com_gamedir));
	if (sIcon)
	{
		// [25/3/2014] Set the transparency key... ~hogsy
		SDL_SetColorKey(sIcon, true, SDL_MapRGB(sIcon->format, 0, 0, 0));
		SDL_SetWindowIcon(sMainWindow, sIcon);
		SDL_FreeSurface(sIcon);
	}
	else
		// Give us a warning, but continue.
		Con_Warning("Failed to load window icon! (%s)\n", SDL_GetError());

	sMainContext = SDL_GL_CreateContext(sMainWindow);
	if (!sMainContext)
		Sys_Error("Failed to create context!\n%s\n", SDL_GetError());

	SDL_GL_SetSwapInterval(0);

#ifdef _WIN32
	if (SDL_GetWindowWMInfo(sMainWindow, &sSystemInfo))
		g_mainwindow.instance = sSystemInfo.info.win.window;
	else
		Con_Warning("Failed to get WM information! (%s)\n", SDL_GetError());
#endif
}

void Window_UpdateVideo(void)
{
	if (Video.iMSAASamples != cvMultisampleSamples.iValue)
	{
		// TODO: Destroy window etc.

		Video.iMSAASamples = cvMultisampleSamples.iValue;
	}

	SDL_SetWindowSize(sMainWindow, Video.iWidth, Video.iHeight);

	if (Video.bVerticalSync != cvVerticalSync.bValue)
	{
		SDL_GL_SetSwapInterval(cvVerticalSync.iValue);

		Video.bVerticalSync = cvVerticalSync.bValue;
	}

	if (Video.bFullscreen != cvFullscreen.bValue)
	{
		if (SDL_SetWindowFullscreen(sMainWindow, (SDL_bool)cvFullscreen.bValue) == -1)
		{
			Con_Warning("Failed to set window mode!\n%s", SDL_GetError());

			// Reset the variable to the current value.
			Cvar_SetValue(cvFullscreen.name, Video.bFullscreen);
		}
		else
			Video.bFullscreen = cvFullscreen.bValue;
	}

	if (!cvFullscreen.value)
		// Center the window, to ensure it's not off screen.
		SDL_SetWindowPosition(sMainWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void Window_Swap(void)
{
	SDL_GL_SwapWindow(sMainWindow);
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
	if (sMainContext)
		// Delete the current context.
		SDL_GL_DeleteContext(sMainContext);

	if (sMainWindow)
		// Destory our window.
		SDL_DestroyWindow(sMainWindow);

	// Quit the SDL subsystem.
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
