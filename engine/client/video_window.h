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

/*	Generic Window Header	*/

#include "platform_window.h"

#define WINDOW_MINIMUM_WIDTH	640
#define WINDOW_MINIMUM_HEIGHT	480

plEXTERN_C_START

void Window_Initialize(void);
void Window_Update(void);
void Window_GetGamma(unsigned short *usRamp, int iRampSize);
void Window_SetGamma(unsigned short *usRamp, int iRampSize);

void Window_GetCursorPosition(int *x, int *y);
void Window_Swap(void);
void Window_Shutdown(void);

void Window_SetFullscreen(bool fullscreen);
void Window_SetSize(unsigned int width, unsigned int height);
void Window_SetVerticalSync(int interval);

int Window_GetWidth(void);	// todo, this should be unsigned
int Window_GetHeight(void);	// todo, this should be unsigned

extern plWindow_t g_mainwindow;

plEXTERN_C_END