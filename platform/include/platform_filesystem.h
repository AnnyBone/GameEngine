/*	Copyright (C) 2011-2015 OldTimes Software

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

#ifndef PLATFORM_FILESYSTEM_H
#define	PLATFORM_FILESYSTEM_H

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

	extern void	pFileSystem_GetUserName(char *out);
	extern void pFileSystem_ScanDirectory(const char *path, const char *extension, void(*Function)(char *filepath));
	extern void	pFileSystem_GetWorkingDirectory(char *out);
	extern void	pFileSystem_UpdatePath(char *path);

	extern bool	pFileSystem_CreateDirectory(const char *path);
	extern bool pFileSystem_IsModified(time_t oldtime, const char *path);

	extern time_t pFileSystem_GetModifiedTime(const char *path);

#ifdef __cplusplus
}
#endif

#endif	// PLATFORM_FILESYSTEM_H
