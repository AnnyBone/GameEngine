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

#include "platform.h"

typedef struct
{
	const char *ccFunctionName;

	void **Function;
} pModuleFunction_t;

plEXTERN_C_START

#ifdef _WIN32
#	define PL_MODULE_EXTENSION	".dll"
#	define PL_MODULE_EXPORT		__declspec(dllexport)
#	define PL_MODULE_IMPORT		__declspec(dllimport)
#else   // Linux
#	define PL_MODULE_EXTENSION	".so"
#	define PL_MODULE_EXPORT		__attribute__((visibility("default")))
#	define PL_MODULE_IMPORT		__attribute__((visibility("hidden")))
#endif

PL_EXTERN PL_FARPROC plFindLibraryFunction(PL_INSTANCE instance, const PLchar *function);

PL_EXTERN PLvoid *plLoadLibraryInterface(PL_INSTANCE instance, const PLchar *path, const PLchar *entry, PLvoid *handle);
PL_INSTANCE plLoadLibrary(const PLchar *path);	// Loads new library instance.
PL_EXTERN PLvoid plUnloadLibrary(PL_INSTANCE instance);	// Unloads library instance.

plEXTERN_C_END
