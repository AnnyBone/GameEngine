/*	Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef __PLATFORM__
#define __PLATFORM__

/*
	Platform Library
	Version 0.10

	This library includes standard platform headers,
	gives you some standard functions to interact with
	the system and includes defines for basic data-types that
	you can use in your applications for easier multi-platform
	support.
*/

// Shared headers
#ifndef pIGNORE_SHARED_HEADERS
//#	ifndef __cplusplus
#		include <stdio.h>
#		include <stdlib.h>
#		include <stdarg.h>
#		include <stdlib.h>
#		ifdef _MSC_VER	// MSVC doesn't support stdint...
#			include "platform_inttypes.h"
#		else
#			include <stdint.h>
#		endif
#		include <string.h>
#		include <ctype.h>
#		include <math.h>
#		include <setjmp.h>
#		include <errno.h>

#		include <sys/stat.h>
#		include <sys/types.h>
/*#	else
#		include <cstdio>
#		include <cstdlib>
#		include <cstdarg>
#		include <cstdlib>
#		include <cstdint>
#		include <cstring>
#		include <cctype>
#		include <cmath>
#		include <csetjmp>
#		include <cerrno>
#	endif*/
#endif

#ifdef _WIN32	// Windows
	// Windows Headers
#	include <Windows.h>
#	include <WindowsX.h>
#	include <CommCtrl.h>
#	include <direct.h>

	// Platform information
#	define	PLATFORM_NAME		"WINDOWS"	// Platform name.
#	define	PLATFORM_MAX_PATH	MAX_PATH-1	// Maximum path length.

	// Other
#	define	pINSTANCE	HINSTANCE       // Instance definition.
#	define	pFARPROC	FARPROC			// Function pointer.
#else	// Linux
	// Linux Headers
#	include <dirent.h>
#	include <unistd.h>
#	include <dlfcn.h>

	// Platform information
#	define	PLATFORM_NAME		"LINUX" // Platform name.
#	define	PLATFORM_MAX_PATH	256		// Maximum path length.

	// Other
#	define	pINSTANCE	void *		// Instance definition.
#	define	pFARPROC	void *		// Function pointer.
#endif

#define	PLATFORM_MAX_USER	256			// Maximum length allowed for a username.

// Helper to allow us to determine the type of CPU; this is used for the module interfaces.
#if defined(__amd64) || defined(__amd64__)
#	define PLATFORM_CPU   "x64"
#else
#	define PLATFORM_CPU   "x86"
#endif

/*
	Boolean
	The boolean data type doesn't exist in C, so this is
	a custom implementation of it.
*/

// These are probably what you're going to want to use.
#ifndef __cplusplus
#if 0	// Internal solution
#ifdef bool
#undef bool
#endif
#ifdef true
#undef true
#endif
#ifdef false
#undef false
#endif
typedef enum
{
	false,	// "false", nothing.
	true	// "true", otherwise just a positive number.
} PlatformBoolean_t;
typedef PlatformBoolean_t bool;
#else	// Using _Bool data type
typedef _Bool bool;

#define	true 1
#define false 0
#endif
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
#define	pBOOL bool
#define pTRUE true
#define pFALSE false

/**/

#ifdef _MSC_VER	// MSVC doesn't support __func__ either...
#	define	pFUNCTION	__FUNCTION__	// Returns the active function.
#else
#	define	pFUNCTION	__func__		// Returns the active function.
#endif

#define	pARRAYELEMENTS(a)	(sizeof(a)/sizeof(*(a)))	// Returns the number of elements within an array.

typedef unsigned int pUINT;
typedef	unsigned char pUCHAR;

#include "platform_window.h"

/*
	Error Management Functionality
*/

static jmp_buf jbException;

#define	pFUNCTION_UPDATE	pError_SetFunction(pFUNCTION)
#define	pFUNCTION_START		pFUNCTION_UPDATE; if(setjmp(jbException)) gWindow_MessageBox("Error","Encountered exception in "pFUNCTION); { 
#define pFUNCTION_END 		}

#ifdef __cplusplus
extern "C" {
#endif

extern	void	pError_Reset(void);								// Resets the error message to "null", so you can ensure you have the correct message from the library.
extern	void	pError_Set(const char *ccMessage,...);			// Sets the error message, so we can grab it outside the library.
extern	void	pError_SetFunction(const char *ccFunction,...);	// Sets the currently active function, for error reporting.

extern char *pError_SystemGet(void);							// Returns the error message currently given by the operating system.
extern void pError_SystemReset(void);

extern char	*pError_Get(void);									// Returns the last recorded error.

#ifdef __cplusplus
}
#endif

/**/

#endif
