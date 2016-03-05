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

/*
	cvar_t variables are used to hold scalar or string variables that can be changed or displayed at the console or prog code as well as accessed directly
	in C code.

	it is sufficient to initialize a cvar_t with just the first two fields, or
	you can add a true flag for variables that you want saved to the configuration
	file when the game is quit:

	cvar_t	r_draworder = {"r_draworder","1"};
	cvar_t	scr_screensize = {"screensize","1",TRUE};

	Cvars must be registered before use, or they will have a 0 value instead of the float interpretation of the string.  Generally, all cvar_t declarations should be registered in the apropriate init function before any console commands are executed:
	Cvar_RegisterVariable (&host_framerate);


	C code usually just references a cvar in place:
	if ( r_draworder.value )

	It could optionally ask for the value to be looked up for a string name:
	if (Cvar_VariableValue ("r_draworder"))

	The user can access cvars from the console in two ways:
	r_draworder			prints the current value
	r_draworder 0		sets the current value to 0
	Cvars are restricted from having the same names as commands to keep this
	interface from being ambiguous.
*/

plEXTERN_C_START

void Cvar_Init(void);

void 	Cvar_RegisterVariable(ConsoleVariable_t *variable, void(*Function)(void)); //johnfitz -- cvar callback
// registers a cvar that allready has the name, string, and optionally the
// archive elements set.

void 	Cvar_Set(const char *var_name, char *value);
// equivelant to "<name> <variable>" typed at the console

void	Cvar_SetValue(const char *var_name, float value);
// expands value to a string and calls Cvar_Set

float	Cvar_VariableValue(const char *var_name);
// returns 0 if not defined or non numeric

bool ConsoleVariable_GetBoolValue(const char *var_name);

char	*Cvar_VariableString(char *var_name);
// returns an empty string if not defined

bool Cvar_Command(void);
// called by Cmd_ExecuteString when Cmd_Argv(0) doesn't match a known
// command.  Returns TRUE if the command was a variable reference that
// was handled. (print or change)

void 	Cvar_WriteVariables(FILE *f);
// Writes lines containing "set variable value" for all variables
// with the archive flag set to TRUE.

void Cvar_Reset(const char *name);

ConsoleVariable_t *Cvar_FindVar(const char *var_name);

extern ConsoleVariable_t *cConsoleVariables;

plEXTERN_C_END
