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
// PLATFORM_STRICT_BOOL

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
#		ifndef unlink
#			define	unlink		_unlink
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

#ifdef __cplusplus
#	define	plEXTERN_C_START	extern "C" {
#	define	plEXTERN_C_END		}
#else
#	define	plEXTERN_C_START
#	define	plEXTERN_C_END
#endif

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

// Use these if you want to show reliance on this library
// or want stricter conformance.
#ifdef PLATFORM_STRICT_BOOL
	typedef enum {
		pl_false, pl_true
	} plBoolean_t;
	typedef plBoolean_t pl_bool;
#else
	enum {
		pl_false, pl_true
	};
	typedef unsigned char pl_bool;
#endif

// These are probably what you're going to want to use.
#ifndef __cplusplus
#	ifdef bool
#		undef bool
#	endif
#	ifdef true
#		undef true
#	endif
#	ifdef false
#		undef false
#	endif
#	ifndef PLATFORM_STRICT_BOOL
		// Using _Bool data type
		enum {
			true, false
		};
		typedef _Bool bool;
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

/**/

#ifdef _MSC_VER	// MSVC doesn't support __func__ either...
#	define	pFUNCTION	__FUNCTION__    // Returns the active function.
#else
#	define	pFUNCTION	__func__	    // Returns the active function.
#endif

#define	pARRAYELEMENTS(a)	(sizeof(a)/sizeof(*(a)))	// Returns the number of elements within an array.

typedef unsigned int	pl_uint;
typedef	unsigned char	pl_uchar;

#include "platform_log.h"
#include "platform_window.h"

/*
	Error Management Functionality
*/

//static jmp_buf jbException;

#define	pFUNCTION_UPDATE()			\
	plResetError();					\
	plSetErrorFunction(pFUNCTION)
#ifndef __cplusplus
#define	pFUNCTION_START		plSetErrorFunction(pFUNCTION); {
#else
#define	pFUNCTION_START	\
plSetErrorFunction(pFUNCTION);
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

/**/

#endif	// PLATFORM_H
