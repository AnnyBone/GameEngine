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

#include "platform_filesystem.h"

/*
	File System
*/

/*	Checks whether a file has been modified or not.
*/
bool plIsFileModified(time_t oldtime, const char *path)
{
	if (!oldtime)
	{
		plSetError("Invalid time, skipping check!\n");
		return false;
	}

	struct stat sAttributes;
	if (stat(path, &sAttributes) == -1)
	{
		plSetError("Failed to get file stats!\n");
		return false;
	}

	if (sAttributes.st_mtime > oldtime)
		return true;

	return false;
}

time_t plGetFileModifiedTime(const char *path)
{
	struct stat sAttributes;
	if (stat(path, &sAttributes) == -1)
	{
		plSetError("Failed to get modification time!\n");
		return 0;
	}
	return sAttributes.st_mtime;
}

void plLowerCasePath(char *out)
{
	pFUNCTION_START
	int i;
	for (i = 0; out[i]; i++)
		out[i] = (char)tolower(out[i]);
	pFUNCTION_END
}

/*  Creates a folder at the given path.
*/
bool plCreateDirectory(const char *ccPath)
{
	pFUNCTION_START
		
#ifdef _WIN32
	if(CreateDirectory(ccPath,NULL) || (GetLastError() == ERROR_ALREADY_EXISTS))
		return true;
	else if(GetLastError() == ERROR_PATH_NOT_FOUND)
		plSetError("Failed to find an intermediate directory! (%s)\n", ccPath);
	else    // Assume it already exists.
		plSetError("Unknown error! (%s)\n", ccPath);
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
					plSetError("Failed to get permission! (%s)\n",ccPath);
				case EROFS:
					plSetError("File system is read only! (%s)\n",ccPath);
				case ENAMETOOLONG:
					plSetError("Path is too long! (%s)\n",ccPath);
				default:
					plSetError("Failed to create directory! (%s)\n",ccPath);
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

void plGetFileExtension(char *in, char *out)
{
	int i;
	while (*in && *in != '.')
		in++;
	if (!*in)
		strncpy(out, "", sizeof(out));
	in++;
	for (i = 0; i < 7 && *in; i++, in++)
		out[i] = *in;
	out[i] = 0;
}

/*	Returns the name of the systems	current user.
	TODO:
		Move this into platform_system
*/
void plGetUserName(char *out)
{
	pFUNCTION_START

#ifdef _WIN32
	char	userstring[PLATFORM_MAX_USER];
	DWORD	name;

	// Set these AFTER we update active function.
	name = sizeof(userstring);
	if (GetUserName(userstring, &name) == 0)
		// If it fails, just set it to user.
		sprintf(userstring, "user");
#else   // Linux
	char *userstring = getenv("LOGNAME");
	if (userstring == NULL)
		// If it fails, just set it to user.
		userstring = "user";
#endif
	{
		int	i = 0,
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

	//strncpy(out, cUser, sizeof(out));

	pFUNCTION_END
}

/*	Scans the given directory.
	On each found file it calls the given function to handle the file.
	TODO:
		Better error management.
		Finish Linux implementation.
*/
void plScanDirectory(const char *path, const char *extension, void(*Function)(char *filepath))
{
	pFUNCTION_START
	char	filestring[PLATFORM_MAX_PATH];

	if (path[0] == ' ')
	{
		plSetError("Invalid path!\n");
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
			plSetError("Failed to find an initial file!\n");
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

void plGetWorkingDirectory(char *out)
{
	pFUNCTION_START
	if (!getcwd(out, PLATFORM_MAX_PATH))
	{
		switch(errno)
		{
		case EACCES:
			plSetError("Permission to read or search a component of the filename was denied!\n");
			break;
		case EFAULT:
			plSetError("buf points to a bad address!\n");
			break;
		case EINVAL:
			plSetError("The size argument is zero and buf is not a null pointer!\n");
			break;
		case ENOMEM:
			plSetError("Out of memory!\n");
			break;
		case ENOENT:
			plSetError("The current working directory has been unlinked!\n");
			break;
		case ERANGE:
			plSetError("The size argument is less than the length of the absolute pathname of the working directory, including the terminating null byte. \
						You need to allocate a bigger array and try again!\n");
			break;
		}
		return;
	}
	strcat(out, "\\");
	pFUNCTION_END
}

/*
	File I/O
*/

/*	Checks if a file exists or not.
*/
bool plFileExists(const char *path)
{
	struct stat buffer;
	return (stat(path, &buffer) == 0);
}

int	plGetLittleShort(FILE *fin)
{
	int	b1 = fgetc(fin);
	int	b2 = fgetc(fin);
	return (short)(b1 + b2 * 256);
}

int plGetLittleLong(FILE *fin)
{
	int b1 = fgetc(fin);
	int	b2 = fgetc(fin);
	int	b3 = fgetc(fin);
	int	b4 = fgetc(fin);
	return b1 + (b2 << 8) + (b3 << 16) + (b4 << 24);
}
