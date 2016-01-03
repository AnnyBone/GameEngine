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

#include "platform_filesystem.h"

/*
	File System
*/

bool pFileSystem_IsModified(time_t tOldTime, const char *ccPath)
{
	if (!tOldTime)
	{
		pSetError("Invalid time, skipping check!\n");
		return false;
	}

	struct stat sAttributes;
	if(stat(ccPath, &sAttributes) == -1)
	{
		pSetError("Failed to get file stats!\n");
		return false;
	}

	if (sAttributes.st_mtime > tOldTime)
		return true;

	return false;
}

time_t pFileSystem_GetModifiedTime(const char *ccPath)
{
	struct stat sAttributes;
	if (stat(ccPath, &sAttributes) == -1)
	{
		pSetError("Failed to get modification time!\n");
		return 0;
	}
	return sAttributes.st_mtime;
}

void pFileSystem_UpdatePath(char *cPath)
{
	pFUNCTION_START
	int i;
	for (i = 0; cPath[i]; i++)
		cPath[i] = (char)tolower(cPath[i]);
	pFUNCTION_END
}

/*  Creates a folder at the given path.
*/
bool pFileSystem_CreateDirectory(const char *ccPath)
{
	pFUNCTION_START
		
#ifdef _WIN32
	if(CreateDirectory(ccPath,NULL) || (GetLastError() == ERROR_ALREADY_EXISTS))
		return true;
	else if(GetLastError() == ERROR_PATH_NOT_FOUND)
		pSetError("Failed to find an intermediate directory! (%s)\n",ccPath);
	else    // Assume it already exists.
		pSetError("Unknown error! (%s)\n",ccPath);
#else	// TODO: Won't the below code work fine on Windows too??
	{
		struct stat ssBuffer;

		if(stat(ccPath,&ssBuffer) == -1)
		{
			if(mkdir(ccPath,0777) == 0)
				return true;
			else
			{
				switch(errno)
				{
				case EACCES:
					pSetError("Failed to get permission! (%s)\n",ccPath);
				case EROFS:
					pSetError("File system is read only! (%s)\n",ccPath);
				case ENAMETOOLONG:
					pSetError("Path is too long! (%s)\n",ccPath);
				default:
					pSetError("Failed to create directory! (%s)\n",ccPath);
				}
			}
		}
		else
			// Path already exists, so this is fine.
			return true;
	}
#endif

	return false;

	pFUNCTION_END
}

/*	Returns the name of the systems	current user.
	TODO:
		This is crazy unsafe... Urghsjhfdj
*/
void pFileSystem_GetUserName(char *out)
{
	pFUNCTION_START

#ifdef _WIN32
	char	userstring[PLATFORM_MAX_USER];
	DWORD	dName;

	sprintf(userstring, "user");

	// Set these AFTER we update active function.
	dName = sizeof(userstring);
	if(GetUserName(userstring,&dName))
#else   // Linux
	char	*userstring = "user";
	userstring = getenv("LOGNAME");
	if (strcasecmp(userstring, "user"))
#endif
	{
		int		i = 0,
				userlength = (int)strlen(userstring);
		while (i < userlength)
		{
			if (userstring[i] == ' ')
				out[i] = '_';
			else
				out[i] = (char)tolower(userstring[i]);
			i++;
		}
	}

	//p_strncpy(out, cUser, sizeof(out));

	pFUNCTION_END
}

/*	Scans the given directory.
	On each found file it calls the given function to handle the file.
	TODO:
		Better error management.
		Finish Linux implementation.
*/
void pFileSystem_ScanDirectory(const char *path, const char *extension, void(*Function)(char *filepath))
{
	pFUNCTION_START
	char	filestring[PLATFORM_MAX_PATH];

	if (path[0] == ' ')
	{
		pSetError("Invalid path!\n");
		return;
	}

#ifdef _WIN32
	{
		WIN32_FIND_DATA	finddata;
		HANDLE			find;

		sprintf(filestring, "%s/*%s", path, extension);

		find = FindFirstFile(filestring, &finddata);
		if (find == INVALID_HANDLE_VALUE)
		{
			pSetError("Failed to find an initial file!\n");
			return;
		}

		do
		{
			// Pass the entire dir + filename.
			sprintf(filestring, "%s/%s", path, finddata.cFileName);
			Function(filestring);
		} while(FindNextFile(find, &finddata));
	}
#else
	{
		DIR             *dDirectory;
		struct  dirent  *dEntry;

		dDirectory = opendir(path);
		if (dDirectory)
		{
			while ((dEntry = readdir(dDirectory)))
			{
				if (strstr(dEntry->d_name, extension))
				{
					sprintf(filestring, "%s/%s", path, dEntry->d_name);
					Function(filestring);
				}
			}

			closedir(dDirectory);
		}
	}
#endif
	pFUNCTION_END
}

void pFileSystem_GetWorkingDirectory(char *cOut)
{
	pFUNCTION_START
	if(!getcwd(cOut,PLATFORM_MAX_PATH))
	{
		switch(errno)
		{
		case EACCES:
			pSetError("Permission to read or search a component of the filename was denied!\n");
			break;
		case EFAULT:
			pSetError("buf points to a bad address!\n");
			break;
		case EINVAL:
			pSetError("The size argument is zero and buf is not a null pointer!\n");
			break;
		case ENOMEM:
			pSetError("Out of memory!\n");
			break;
		case ENOENT:
			pSetError("The current working directory has been unlinked!\n");
			break;
		case ERANGE:
			pSetError("The size argument is less than the length of the absolute pathname of the working directory, including the terminating null byte. \
						You need to allocate a bigger array and try again!\n");
			break;
		}
		return;
	}
	strcat(cOut,"\\");
	pFUNCTION_END
}
