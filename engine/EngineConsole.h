/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
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

#pragma once

plEXTERN_C_START

extern unsigned int con_totallines;
extern unsigned int con_backscroll;

extern	bool	con_forcedup;	// because no entities to refresh
extern	bool	bConsoleInitialized;

extern uint8_t *con_chars;

extern char con_lastcenterstring[]; //johnfitz

void Con_DrawCharacter(int cx, int line, int num);
void Con_CheckResize(void);
void Console_Initialize(void);
void Con_DrawConsole(int lines, bool drawinput);
void Con_Print(char *txt);
void Con_Printf(const char *fmt, ...);
void Con_SPrintf(char *dest, int size, char *fmt, ...);
void Con_Warning(const char *fmt, ...); //johnfitz
void Con_Error(char *fmt, ...);
void Con_DPrintf(const char *fmt, ...);
void Con_SafePrintf(const char *fmt, ...);
void Con_Clear_f(void);
void Con_DrawNotify(void);
void Con_ClearNotify(void);
void Con_ToggleConsole_f(void);
void Console_ErrorMessage(bool bCrash, const char *ccFile, const char *reason);

// Other crap...
char *Con_Quakebar(unsigned int len);
void Con_LogCenterPrint(char *str);

plEXTERN_C_END

