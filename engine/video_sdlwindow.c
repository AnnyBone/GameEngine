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

/*	Window Manager	*/

SDL_Window		*sdl_mainwindow;
SDL_GLContext	sdl_context;
SDL_DisplayMode	sdl_displaymode;

plWindow_t g_mainwindow;

void Window_Initialize(void)
{
	if (SDL_VideoInit(NULL) < 0)
		Sys_Error("Failed to initialize video!\n%s\n", SDL_GetError());

	SDL_DisableScreenSaver();

	// Get display information.
	if (SDL_GetCurrentDisplayMode(0, &sdl_displaymode) != 0)
		Sys_Error("Failed to get current display information!\n%s\n", SDL_GetError());

	g_mainwindow.is_unlocked	= false;	// Video mode is initially locked.
	g_mainwindow.is_active		= true;		// Window is intially assumed active.

	if (COM_CheckParm("-window"))
	{
		g_mainwindow.is_fullscreen	= false;
		g_mainwindow.is_unlocked	= true;
	}
	else g_mainwindow.is_fullscreen = cv_video_fullscreen.bValue;

	if (COM_CheckParm("-width"))
	{
		g_mainwindow.width			= atoi(com_argv[COM_CheckParm("-width") + 1]);
		g_mainwindow.is_unlocked	= false;
	}
	else g_mainwindow.width = cv_video_width.iValue;

	if (COM_CheckParm("-height"))
	{
		g_mainwindow.height			= atoi(com_argv[COM_CheckParm("-height") + 1]);
		g_mainwindow.is_unlocked	= false;
	}
	else g_mainwindow.height = cv_video_height.iValue;

#if 0
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
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

	int	flags =	SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
	if (!g_mainwindow.is_fullscreen)
		flags &= ~SDL_WINDOW_FULLSCREEN;

	sdl_mainwindow = SDL_CreateWindow(
		Game->Name,	// Use name passed by game module.
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		g_mainwindow.width,
		g_mainwindow.height,
		flags);
	if (!sdl_mainwindow)
		Sys_Error("Failed to create window!\n%s\n", SDL_GetError());
	
	// Attempt to grab the window icon from the game directory.
	SDL_Surface	*icon = SDL_LoadBMP(va("%s/icon.bmp", com_gamedir));
	if (icon)
	{
		SDL_SetColorKey(icon, true, SDL_MapRGB(icon->format, 0, 0, 0));
		SDL_SetWindowIcon(sdl_mainwindow, icon);
		SDL_FreeSurface(icon);
	}
	else
		// Give us a warning, but continue.
		Con_Warning("Failed to load window icon! (%s)\n", SDL_GetError());

	sdl_context = SDL_GL_CreateContext(sdl_mainwindow);
	if (!sdl_context)
		Sys_Error("Failed to create context!\n%s\n", SDL_GetError());

	Window_SetVerticalSync(0);

#ifdef _WIN32
	SDL_SysWMinfo sdl_systeminfo;
	if (SDL_GetWindowWMInfo(sdl_mainwindow, &sdl_systeminfo))
		g_mainwindow.instance = sdl_systeminfo.info.win.window;
	else
		Con_Warning("Failed to get WM information! (%s)\n", SDL_GetError());
#endif
}

void Window_SetSize(unsigned int width, unsigned int height)
{
	if ((g_mainwindow.width == width) && (g_mainwindow.height == height))
		return;

	// Ensure the given width and height are within reasonable bounds.
	if (width < WINDOW_MINIMUM_WIDTH ||
		height < WINDOW_MINIMUM_HEIGHT)
	{
		Con_Warning("Failed to get an appropriate resolution!\n");

		width = WINDOW_MINIMUM_WIDTH;
		height = WINDOW_MINIMUM_HEIGHT;
	}
	// If we're not fullscreen, then constrain our window size to the size of the desktop.
	else if (!g_mainwindow.is_fullscreen && ((width > plGetScreenWidth()) || (height > plGetScreenHeight())))
	{
		Con_Warning("Attempted to set resolution beyond scope of desktop! (%i x %i)\n", width, height);

		width = plGetScreenWidth();
		height = plGetScreenHeight();
	}

	g_mainwindow.width = width; g_mainwindow.height = height;
	SDL_SetWindowSize(sdl_mainwindow, g_mainwindow.width, g_mainwindow.height);

	Cvar_SetValue(cv_video_width.name, g_mainwindow.width);
	Cvar_SetValue(cv_video_height.name, g_mainwindow.height);
}

void Window_SetFullscreen(bool fullscreen)
{
	if (g_mainwindow.is_fullscreen == fullscreen)
		return;

	if (SDL_SetWindowFullscreen(sdl_mainwindow, (SDL_bool)fullscreen) != 0)
	{
		Con_Warning("Failed to set window mode!\n%s", SDL_GetError());
		return;
	}

	// Center the window, to ensure it's not off screen.
	if (!fullscreen) SDL_SetWindowPosition(sdl_mainwindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	g_mainwindow.is_fullscreen = fullscreen;
	Cvar_SetValue(cv_video_fullscreen.name, g_mainwindow.is_fullscreen);
}

void Window_SetVerticalSync(int interval)
{
	if (g_mainwindow.vsync_interval == interval)
		return;

	if (SDL_GL_SetSwapInterval(interval) != 0)
	{
		Con_Warning("Failed to set swap interval!\n%s", SDL_GetError());
		return;
	}

	g_mainwindow.vsync_interval = interval;
	Cvar_SetValue(cv_video_verticlesync.name, interval);
}

void Window_Update(void)
{
	if (g_state.embedded) 
		return;

	if (!g_mainwindow.is_unlocked)
	{
		Cvar_SetValue(cv_video_fullscreen.name, (float)g_mainwindow.is_fullscreen);
		Cvar_SetValue(cv_video_width.name, (float)g_mainwindow.width);
		Cvar_SetValue(cv_video_height.name, (float)g_mainwindow.height);
		Cvar_SetValue(cv_video_verticlesync.name, (float)g_mainwindow.vsync_interval);

		g_mainwindow.is_unlocked = true;
		return;
	}

	Window_SetSize(cv_video_width.iValue, cv_video_height.iValue);
	Window_SetVerticalSync(cv_video_verticlesync.iValue);
	Window_SetFullscreen(cv_video_fullscreen.bValue);
}

void Window_Swap(void)
{
	SDL_GL_SwapWindow(sdl_mainwindow);
}

/*	Set the gamma level.
	Based on Darkplaces implementation.
*/
void Window_SetGamma(unsigned short *usRamp, int iRampSize)
{
	if (!SDL_SetWindowGammaRamp(sdl_mainwindow, usRamp, usRamp + iRampSize, usRamp + iRampSize * 2))
		Con_Warning("Failed to set gamma level!\n%s", SDL_GetError());
}

/*	Get gamma level.
	Based on the Darkplaces implementation.
*/
void Window_GetGamma(unsigned short *usRamp, int iRampSize)
{
	if (!SDL_GetWindowGammaRamp(sdl_mainwindow, usRamp, usRamp + iRampSize, usRamp + iRampSize * 2))
		Con_Warning("Failed to get gamma level!\n%s", SDL_GetError());
}

int Window_GetWidth(void)
{
	return g_mainwindow.width;
}

int Window_GetHeight(void)
{
	return g_mainwindow.height;
}

void Window_GetCursorPosition(int *x, int *y)
{
	SDL_GetMouseState(x, y);
}

void Window_Shutdown(void)
{
	if (sdl_context)	SDL_GL_DeleteContext(sdl_context);
	if (sdl_mainwindow)	SDL_DestroyWindow(sdl_mainwindow);

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
