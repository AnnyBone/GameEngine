/*	Copyright (C) 2013-2015 OldTimes Software
*/

#include "platform.h"

typedef struct
{
	char	id[4];

	int	dirLength, dirOffset;
} PackHeader_t;

typedef struct
{
	char	name[PLATFORM_MAX_PATH];

	int	position, length;
} PackFile_t;

typedef struct
{
	char	name[PLATFORM_MAX_PATH];

	int	handle, files;

	PackFile_t *fileArray;
} Pack_t;

char *FileSystem_SkipPath(char *cPath)
{
	char    *last;

	last = cPath;

	while (*cPath)
	{
		if (*cPath == '/')
			last = cPath + 1;
		cPath++;
	}

	return last;
}