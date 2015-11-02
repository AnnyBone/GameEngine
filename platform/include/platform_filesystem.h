/*	Copyright (C) 2013-2015 OldTimes Software
*/
#ifndef __PLATFORMFILESYSTEM__
#define	__PLATFORMFILESYSTEM__

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

	extern void	pFileSystem_GetUserName(char *cOut);
	extern void pFileSystem_ScanDirectory(const char *path, const char *extension, void(*Function)(char *filepath));
	extern void	pFileSystem_GetWorkingDirectory(char *cOut);
	extern void	pFileSystem_UpdatePath(char *cPath);

	extern bool	pFileSystem_CreateDirectory(const char *ccPath);
	extern bool pFileSystem_IsModified(time_t tOldTime, const char *ccPath);

	extern time_t pFileSystem_GetModifiedTime(const char *ccPath);

#ifdef __cplusplus
}
#endif

#endif
