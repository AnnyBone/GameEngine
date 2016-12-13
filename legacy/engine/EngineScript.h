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

#pragma once

#include "common/SCRIPLIB.H"

#define	SCRIPT_SYMBOL_FUNCTION	'$'
#define	SCRIPT_SYMBOL_VARIABLE	'%'

#define SCRIPT_START			'{'
#define	SCRIPT_END				'}'

plEXTERN_C_START
bool Script_Load(const char *ccPath);
plEXTERN_C_END
