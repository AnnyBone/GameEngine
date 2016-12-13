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

#include "pr_comp.h"			// defs shared with qcc

#include "shared_game.h"

typedef union eval_s
{
	int		string;
	float			_float;
	float			vector[3];
	int			function;
	int				_int;
	int				edict;
} eval_t;

#define	EDICT_FROM_AREA(l) STRUCT_FROM_LINK(l,ServerEntity_t,area)

//============================================================================

extern	GlobalState_t	pr_global_struct;

//============================================================================

void PR_Init (void);

ServerEntity_t *ED_Alloc (void);
void ED_Free(ServerEntity_t *ed);

void Edict_Print(ServerEntity_t *eEntity);
void ED_Write(FILE *f, ServerEntity_t *ed);

char *ED_ParseEdict(char *data, ServerEntity_t *ent);

void ED_WriteGlobals (FILE *f);
void ED_ParseGlobals (char *data);
void ED_LoadFromFile (char *data);

ServerEntity_t *EDICT_NUM(unsigned int n);
unsigned int NUM_FOR_EDICT(ServerEntity_t *e);

//============================================================================

typedef void (*builtin_t) (void);

void ED_PrintEdicts (void);
void ED_PrintNum (int ent);

eval_t *GetEdictFieldValue(ServerEntity_t *ed, char *field);
