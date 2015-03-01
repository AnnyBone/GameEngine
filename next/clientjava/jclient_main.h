/*	Copyright (C) 2015 OldTimes Software
*/
#ifndef __JCLIENTMAIN__
#define __JCLIENTMAIN__

#include "platform.h"
#include "platform_module.h"
#include "platform_window.h"
#include "platform_math.h"
#include "platform_filesystem.h"

#include <jni.h>

#include "..\..\external\glee\GLee.h"

#include <GLFW/glfw3.h>

#define	CLIENTJVM_TITLE	"JClient"

typedef struct
{
	char	cCurrentDirectory[PLATFORM_MAX_PATH];	// Current working directory, used as the base path for scripts and content.
} EOSGlobal_t;

EOSGlobal_t	Global;

extern GLFWwindow *gMainWindow;	// Main GL window

void JClient_Shutdown(void);

#endif