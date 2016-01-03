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

#include "platform.h"

#include "platform_module.h"

/*
	Module management
*/

pFARPROC plFindModuleFunction(pINSTANCE instance, const char *function)
{
	pFUNCTION_UPDATE();

	if(instance)
	{
		pFARPROC address;

#ifdef _WIN32
		address = GetProcAddress(instance, function);
#else   // Linux
		address = dlsym(instance, function);
#endif
		if (address)
			return address;
	}

	return (NULL);
}

void plUnloadModule(pINSTANCE instance)
{
	pFUNCTION_UPDATE();

	if(instance)
	{
#ifdef _WIN32
		FreeLibrary(instance);
#else   // Linux
		dlclose(instance);
#endif

		// Set the instance to null.
		instance = NULL;
	}
}

/*	Function to allow direct loading of an external module.
*/
pINSTANCE plLoadModule(const char *path)
{
	pINSTANCE	instance;
	char		newpath[PLATFORM_MAX_PATH];

	pFUNCTION_UPDATE();

	sprintf(newpath, "%s"pMODULE_EXTENSION, path);

	instance =
#ifdef _WIN32
		LoadLibrary(newpath);
#else
		dlopen(newpath,RTLD_NOW);
#endif
	if (!instance)
	{
		plSetError("Failed to load module! (%s)\n%s\n", newpath, plGetSystemError());
#if 0	// Second attempt; load it from a platform-specific subdirectory.

		// Print it...
		printf("%s", plGetError());

		// Attempt to load under a different directory.
		sprintf(newpath, PLATFORM_CPU"/%s"pMODULE_EXTENSION, path);
		instance =
#ifdef _WIN32
			LoadLibrary(newpath);
#else
			dlopen(newpath, RTLD_NOW);
#endif
		if (!iModule)
		{
			plSetError("%s\nFailed to load module! (%s)\n%s\n", newpath, plGetSystemError());
			return NULL;
		}
#else
		return NULL;
#endif
	}

	return instance;
}

/*	Generic interface to allow loading of an external module.
*/
void *plLoadModuleInterface(pINSTANCE instance, const char *path, const char *entry, void *handle)
{
//	pFUNCTION_START

	char	newpath[PLATFORM_MAX_PATH];
	void	*(*EntryFunction)(void*);

	sprintf(newpath, "%s."PLATFORM_CPU, path);
	instance = plLoadModule(newpath);
	if(!instance)
		return NULL;

	EntryFunction = (void*)plFindModuleFunction(instance, entry);
	if (!EntryFunction)
	{
		plSetError("Failed to find entry function! (%s)\n", entry);
		return NULL;
	}

	return (EntryFunction(handle));

//	pFUNCTION_END
}
