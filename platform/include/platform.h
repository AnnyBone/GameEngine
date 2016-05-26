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

/*
	Platform Library
	
	This library includes standard platform headers,
	gives you some standard functions to interact with
	the system and includes defines for basic data-types that
	you can use in your applications for easier multi-platform
	support.
*/

// PL_IGNORE_SHARED_HEADERS
// PL_IGNORE_PLATFORM_HEADERS
// PL_IGNORE_STD_HEADERS

#define PL_INCLUDE_STD_BOOL

// Shared headers
#ifndef PL_IGNORE_SHARED_HEADERS
#	include <stdio.h>
#	include <stdlib.h>
#	include <stdarg.h>
#	include <stdlib.h>
#	ifdef PL_INCLUDE_STD_BOOL
#		include <stdbool.h>
#	endif
#	ifdef _MSC_VER	// MSVC doesn't support stdint...
#		ifndef __cplusplus
#			include "platform_inttypes.h"
#		endif
#	else
#		include <stdint.h>
#	endif
#	include <string.h>
#	include <ctype.h>
#	include <math.h>
#	include <setjmp.h>
#	include <errno.h>
#	include <time.h>

#	include <sys/stat.h>
#	include <sys/types.h>

	// C++
#	ifndef PL_IGNORE_STD_HEADERS
#		ifdef __cplusplus
#			include <stdint.h>
#			include <memory>
#			include <string>
#			include <vector>
#			include <set>
#			include <unordered_set>
#			include <map>
#			include <unordered_map>
#			include <algorithm>

			// istream
#			include <fstream>
#			include <iostream>
#		endif
#	endif
#endif

// Windows
#ifdef _WIN32
	// Headers
#	ifndef PL_IGNORE_PLATFORM_HEADERS
#		include <Windows.h>
#		include <WindowsX.h>
#		include <CommCtrl.h>
#		include <direct.h>
#		include <lmcons.h>

#		ifdef PlaySound
#			undef PlaySound
#		endif

#		undef min
#		undef max
#	endif

	// Information
#	define	PL_NAME	"WINDOWS"	// Platform name.

	// Limits
#	define	PLATFORM_MAX_PATH	MAX_PATH-1	// Maximum path length.
#	define	PL_MAX_USERNAME		UNLEN

	// Other
#	ifdef _MSC_VER
#		pragma warning(disable : 4152)
#		pragma warning(disable : 4800)	// 'type' : forcing value to bool 'true' or 'false' (performance warning)

#		ifndef itoa
#			define	itoa		_itoa
#		endif
#		ifndef getcwd
#			define	getcwd		_getcwd
#		endif
#		ifndef snprintf
#			define	snprintf	_snprintf
#		endif
#		ifndef unlink
#			define	unlink		_unlink
#		endif
#		ifndef strcasecmp
#			define	strcasecmp	_stricmp
#		endif
#		ifndef strncasecmp
#			define	strncasecmp	_str
#		endif
#		ifdef __cplusplus
#			ifndef nothrow
//#				define nothrow __nothrow
#			endif
#		endif
#	endif
#elif __APPLE__	// Mac OS X
	// Information
#	define	PL_NAME	"APPLE"

	// Limits
#	define	PLATFORM_MAX_PATH	256	// Supposedly "unlimited", but we'll limit this anyway.

	// Other
#	ifndef st_mtime
#		define	st_mtime st_mtimespec.tv_sec
#	endif
#else	// Linux
	// Headers
#	ifndef PL_IGNORE_PLATFORM_HEADERS
#		include <dirent.h>
#		include <unistd.h>
#		include <dlfcn.h>
#		include <strings.h>
#	endif

	// Information
#	define	PL_NAME	"LINUX"

	// Limits
#	define	PLATFORM_MAX_PATH	256	// Maximum path length.
#	define	PL_MAX_USERNAME		32
#endif

// Other
#ifndef PL_INSTANCE
#	define	PL_INSTANCE	void *	// Instance definition.
#endif
#ifndef PL_FARPROC
#	define	PL_FARPROC	void *	// Function pointer.
#endif

// Set the defaults if nothing's been set for any of these...

#ifndef PL_NAME
#	define PL_NAME "Unknown"	// Platform name.
#endif
#ifndef PLATFORM_MAX_PATH
#	define PLATFORM_MAX_PATH 256	// Max path supported on platform.
#endif
#ifndef PL_MAX_PATH
#	define PL_MAX_PATH 256	// Max path supported on platform.
#endif
#ifndef PL_MAX_USERNAME
#	define PL_MAX_USERNAME 256	// Maximum length allowed for a username.
#endif

#if defined(_MSC_VER)
#	define PL_EXTERN	extern
#	define PL_CALL		__stdcall
#	define PL_INLINE	__inline

	// MSVC doesn't support __func__
#	define PL_FUNCTION	__FUNCTION__    // Returns the active function.
#else
#	define PL_EXTERN	extern
#	define PL_CALL		
#	define PL_INLINE	inline

#	define PL_FUNCTION	__func__	    // Returns the active function.
#endif

#ifdef __cplusplus
#	define	plEXTERN_C_START	extern "C" {
#	define	plEXTERN_C_END		}
#else
#	define	plEXTERN_C_START
#	define	plEXTERN_C_END
#endif

// Helper to allow us to determine the type of CPU; this is used for the module interfaces.
#if defined(__amd64) || defined(__amd64__)
#	define PL_CPU_ID   "x64"
#else
#	define PL_CPU_ID   "x86"
#endif

// These are usually expected to be defined already, but in-case they're not then we define them here.
#ifndef BOOL
#	define BOOL	bool
#endif
#ifndef TRUE
#	define TRUE true
#endif
#ifndef FALSE
#	define FALSE false
#endif

#define PL_BOOL		BOOL
#define PL_TRUE		TRUE
#define PL_FALSE	FALSE

/**/

#define	plArrayElements(a)	(sizeof(a)/sizeof(*(a)))	// Returns the number of elements within an array.

typedef unsigned int	pl_uint;
typedef	unsigned char	pl_uchar;
typedef unsigned char	pl_bool;

#include "platform_log.h"
#include "platform_window.h"
#include "platform_math.h"
#include "platform_video_layer.h"

/*
	Error Management Functionality
*/

//static jmp_buf jbException;

#define	pFUNCTION_UPDATE()			\
	plResetError();					\
	plSetErrorFunction(PL_FUNCTION)
#ifndef __cplusplus
#define	pFUNCTION_START		plSetErrorFunction(PL_FUNCTION); {
#else
#define	pFUNCTION_START	\
plSetErrorFunction(PL_FUNCTION);
// TRY whatever
#endif
#define pFUNCTION_END 		}

plEXTERN_C_START

	extern void	plResetError(void);									// Resets the error message to "null", so you can ensure you have the correct message from the library.
	extern void	plSetError(const char *msg, ...);					// Sets the error message, so we can grab it outside the library.
	extern void	plSetErrorFunction(const char *function, ...);		// Sets the currently active function, for error reporting.

	extern char *plGetSystemError(void);	// Returns the error message currently given by the operating system.
	extern char	*plGetError(void);			// Returns the last recorded error.

plEXTERN_C_END

/*	Converts string to time.
	http://stackoverflow.com/questions/1765014/convert-string-from-date-into-a-time-t
*/
static PL_INLINE time_t plStringToTime(const char *ts)
{
	char s_month[5];
	int day, year;
	sscanf(ts, "%s %d %d", s_month, &day, &year);

	static const char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	int month = (strstr(months, s_month) - months) / 3;
	struct tm time = { 0 };
	time.tm_mon = month;
	time.tm_mday = day;
	time.tm_year = year - 1900;
	time.tm_isdst = -1;

	return mktime(&time);
}

/**/
