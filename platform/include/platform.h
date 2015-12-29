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

#ifndef PLATFORM_H
#define PLATFORM_H

/*
	Platform Library
	
	This library includes standard platform headers,
	gives you some standard functions to interact with
	the system and includes defines for basic data-types that
	you can use in your applications for easier multi-platform
	support.
*/

// pIGNORE_SHARED_HEADERS
// pIGNORE_PLATFORM_HEADERS

// Shared headers
#ifndef pIGNORE_SHARED_HEADERS
#	include <stdio.h>
#	include <stdlib.h>
#	include <stdarg.h>
#	include <stdlib.h>
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

#	include <sys/stat.h>
#	include <sys/types.h>

	// C++
#	ifdef __cplusplus
#		include <stdint.h>
#		include <memory>
#		include <string>
#		include <vector>
#	endif
#endif

// Windows
#ifdef _WIN32
	// Headers
#	ifndef pIGNORE_PLATFORM_HEADERS
#		include <Windows.h>
#		include <WindowsX.h>
#		include <CommCtrl.h>
#		include <direct.h>
#		include <lmcons.h>
#	endif

	// Information
#	define	PLATFORM_NAME	"WINDOWS"	// Platform name.

	// Limits
#	define	PLATFORM_MAX_PATH	MAX_PATH-1	// Maximum path length.
#	define	PLATFORM_MAX_USER	UNLEN

	// Other
#	ifdef _MSC_VER
#		pragma warning(disable : 4152)

#		define pIGNORE_INTERNAL_STANDARD

#		ifndef itoa
#			define	itoa		_itoa
#		endif
#		ifndef getcwd
#			define	getcwd		_getcwd
#		endif
#		ifndef snprintf
#			define	snprintf	_snprintf
#		endif
#	endif
#elif __APPLE__	// Mac OS X
	// Information
#	define	PLATFORM_NAME	"APPLE"

	// Limits
#	define	PLATFORM_MAX_PATH	256	// Supposedly "unlimited", but we'll limit this anyway.

	// Other
#	ifndef st_mtime
#		define	st_mtime st_mtimespec.tv_sec
#	endif
#else	// Linux
	// Headers
#	ifndef pIGNORE_PLATFORM_HEADERS
#		include <dirent.h>
#		include <unistd.h>
#		include <dlfcn.h>
#	endif

	// Information
#	define	PLATFORM_NAME	"LINUX"

	// Limits
#	define	PLATFORM_MAX_PATH	256	// Maximum path length.
#	define	PLATFORM_MAX_USER	32
#endif

// Other
#ifndef pINSTANCE
#	define	pINSTANCE	void *	// Instance definition.
#endif
#ifndef pFARPROC
#	define	pFARPROC	void *	// Function pointer.
#endif

// Set the defaults if nothing's been set for any of these...

#ifndef PLATFORM_NAME
#	define	PLATFORM_NAME	"Unknown"
#endif
#ifndef PLATFORM_MAX_PATH
#	define	PLATFORM_MAX_PATH	256
#endif
#ifndef PLATFORM_MAX_USER
#	define	PLATFORM_MAX_USER	256			// Maximum length allowed for a username.
#endif

#define	pEXTERN_C extern "C"

// Helper to allow us to determine the type of CPU; this is used for the module interfaces.
#if defined(__amd64) || defined(__amd64__)
#	define PLATFORM_CPU   "x64"
#else
#	define PLATFORM_CPU   "x86"
#endif

/*
	Boolean
	The boolean data type doesn't really exist in C, so this is
	a custom implementation of it.
*/

// These are probably what you're going to want to use.
#ifndef __cplusplus
#	if 0	// Internal solution
#		ifdef bool
#			undef bool
#		endif
#		ifdef true
#			undef true
#		endif
#		ifdef false
#			undef false
#		endif
		typedef enum
		{
			false,	// "false", nothing.
			true	// "true", otherwise just a positive number.
		} PlatformBoolean_t;
		typedef PlatformBoolean_t bool;
#	else	// Using _Bool data type
		typedef _Bool bool;

#		define	true 1
#		define false 0
#	endif
#endif

// These are usually expected to be defined already, but in-case they're not then we define them here.
#ifndef BOOL
#	define BOOL bool
#endif
#ifndef TRUE
#	define TRUE true
#endif
#ifndef FALSE
#	define FALSE false
#endif

// Use these if you want to show reliance on this library.
typedef bool pBOOL;
#if 0
typedef true pTRUE;
typedef false pFALSE;
#else
#define pTRUE true
#define pFALSE false
#endif

/**/

#ifdef _MSC_VER	// MSVC doesn't support __func__ either...
#	define	pFUNCTION	__FUNCTION__    // Returns the active function.
#else
#	define	pFUNCTION	__func__	    // Returns the active function.
#endif

#define	pARRAYELEMENTS(a)	(sizeof(a)/sizeof(*(a)))	// Returns the number of elements within an array.

typedef unsigned int	pUINT;
typedef	unsigned char	pUCHAR;

#include "platform_log.h"
#include "platform_window.h"

/*	Platform Standard Headers
*/
#include "platform_string.h"

/*
	Error Management Functionality
*/

//static jmp_buf jbException;

#define	pFUNCTION_UPDATE	pError_SetFunction(pFUNCTION)
#ifndef __cplusplus
#define	pFUNCTION_START		pError_SetFunction(pFUNCTION); {
#else
#define	pFUNCTION_START	\
pError_SetFunction(pFUNCTION);
// TRY whatever
#endif
#define pFUNCTION_END 		}

typedef enum
{
	pERROR_INVALID_HANDLE
} PlatformErrorType_t;

#ifdef __cplusplus
extern "C" {
#endif

	extern void	pError_Reset(void);								// Resets the error message to "null", so you can ensure you have the correct message from the library.
	extern void	pError_Set(const char *ccMessage, ...);			// Sets the error message, so we can grab it outside the library.
	extern void	pError_SetFunction(const char *ccFunction, ...);	// Sets the currently active function, for error reporting.

	extern char *pError_SystemGet(void);	// Returns the error message currently given by the operating system.
	extern void pError_SystemReset(void);

	extern char	*pError_Get(void);	// Returns the last recorded error.

	/*
		Standard Implementation
	*/

	extern char *pString_Copy(char *cDest, const char *ccSource);

#ifdef __cplusplus
}
#endif

/**/

#endif	// PLATFORM_H
