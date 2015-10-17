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

#include "platform_filesystem.h"

/*
	File System
*/

bool pFileSystem_IsModified(time_t tOldTime, const char *ccPath)
{
	if (!tOldTime)
	{
		pError_Set("Invalid time, skipping check!\n");
		return false;
	}

	struct stat sAttributes;
	if(stat(ccPath, &sAttributes) == -1)
	{
		pError_Set("Failed to get file stats!\n");
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
		pError_Set("Failed to get modification time!\n");
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
		pError_Set("Failed to find an intermediate directory! (%s)\n",ccPath);
	else    // Assume it already exists.
		pError_Set("Unknown error! (%s)\n",ccPath);
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
					pError_Set("Failed to get permission! (%s)\n",ccPath);
				case EROFS:
					pError_Set("File system is read only! (%s)\n",ccPath);
				case ENAMETOOLONG:
					pError_Set("Path is too long! (%s)\n",ccPath);
				default:
					pError_Set("Failed to create directory! (%s)\n",ccPath);
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

/*	Returns the name of the systems
	current user.
*/
void pFileSystem_GetUserName(char *cOut)
{
	pFUNCTION_START

#ifdef _WIN32
    char	cUser[128] = "user";
	DWORD	dName;

	// Set these AFTER we update active function.
	dName	= sizeof(cUser);

	if(GetUserName(cUser,&dName))
#else   // Linux
    char *cUser = "user";

    cUser = getenv("LOGNAME");
	if(strcasecmp(cUser,"user"))
#endif
	{
		int	i		= 0,
			iLength = (int)strlen(cUser);

		// [11/5/2013] Quick fix for spaces ~hogsy
		while(i < iLength)
		{
			if(cUser[i] == ' ')
				cUser[i] = '_';
			else
				cUser[i] = (char)tolower(cUser[i]);

			i++;
		}
	}

	strcpy(cOut,cUser);

	pFUNCTION_END
}

/*	Scans the given directory.
	On each found file it calls the given function to handle the file.
	TODO:
		Better error management.
		Finish Linux implementation.
*/
void pFileSystem_ScanDirectory(const char *ccPath,const char *ccExtension,void (*vFunction)(char *cFile))
{
	pFUNCTION_START
	if (ccPath[0] == ' ')
	{
		pError_Set("Invalid path!\n");
		return;
	}

#ifdef _WIN32
	{
        char cFileString[PLATFORM_MAX_PATH];
		WIN32_FIND_DATA	wfdData;
		HANDLE			hFile;

		sprintf(cFileString, "%s*%s", ccPath, ccExtension);

		hFile = FindFirstFile(cFileString, &wfdData);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			pError_Set("Recieved invalid handle! (%s)\n", cFileString);
			return;
		}
		else if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			pError_Set("Failed to find first file! (%s)\n", cFileString);
			return;
		}

		do
		{
			// Send the file we've found!
			vFunction(wfdData.cFileName);
		} while (FindNextFile(hFile, &wfdData));

		FindClose(hFile);
	}
#else	// Linux (todo)
/*	{
		DIR             *dDirectory;
		struct  dirent  *dEntry;

		sprintf(cFileString, "%s*%s", ccPath, ccExtension);

		dDirectory = opendir(ccPath);
		if (dDirectory)
		{
			while ((dEntry = readdir(dDirectory)))
			{
				if (strstr(dEntry->d_name, ccExtension))
				{
					vFunction(wfdData.cFileName);
				}
			}

			closedir(dDirectory);
		}
	}*/
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
            pError_Set("Permission to read or search a component of the filename was denied!\n");
			break;
		case EFAULT:
			pError_Set("buf points to a bad address!\n");
			break;
		case EINVAL:
			pError_Set("The size argument is zero and buf is not a null pointer!\n");
			break;
		case ENOMEM:
			pError_Set("Out of memory!\n");
			break;
		case ENOENT:
			pError_Set("The current working directory has been unlinked!\n");
			break;
		case ERANGE:
			pError_Set("The size argument is less than the length of the absolute pathname of the working directory, including the terminating null byte. \
					    You need to allocate a bigger array and try again!\n");
			break;
        }
        return;
    }
    strcat(cOut,"\\");
	pFUNCTION_END
}
