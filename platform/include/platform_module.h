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

#ifndef PLATFORM_MODULE_H
#define PLATFORM_MODULE_H

#include "platform.h"

// Copied from Darkplaces.
typedef struct
{
	const char *ccFunctionName;

	void **Function;
} pModuleFunction_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define	pMODULE_EXTENSION	".dll"
#define	pMODULE_EXPORT		__declspec(dllexport)
#define pMODULE_IMPORT		__declspec(dllimport)
#else   // Linux
#define	pMODULE_EXTENSION	".so"
#define pMODULE_EXPORT		__attribute__((visibility("default")))
#define pMODULE_IMPORT		__attribute__((visibility("hidden")))
#endif

	extern pFARPROC plFindModuleFunction(pINSTANCE instance, const char *function);

	pINSTANCE plLoadModule(const char *path);

	extern void plUnloadModule(pINSTANCE instance);
	extern void *plLoadModuleInterface(pINSTANCE instance, const char *path, const char *entry, void *handle);

#ifdef __cplusplus
}
#endif

#endif // !PLATFORM_MODULE_H
