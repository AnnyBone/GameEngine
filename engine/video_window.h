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

#ifndef VIDEO_WINDOW_H
#define	VIDEO_WINDOW_H

/*
	Generic Window Header
*/

#define WINDOW_MINIMUM_WIDTH	640
#define WINDOW_MINIMUM_HEIGHT	480

plEXTERN_C_START

void Window_InitializeVideo(void);
void Window_UpdateVideo(void);
void Window_GetGamma(unsigned short *usRamp, int iRampSize);
void Window_SetGamma(unsigned short *usRamp, int iRampSize);
void Window_GetCursorPosition(int *x, int *y);
void Window_Swap(void);
void Window_Shutdown(void);

int Window_GetWidth(void);
int Window_GetHeight(void);

typedef struct
{
#ifdef _WIN32
	HWND instance;
#endif
} Window_t;

extern Window_t g_mainwindow;

plEXTERN_C_END

#endif // !VIDEO_WINDOW_H
