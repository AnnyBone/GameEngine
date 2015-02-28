/*	Copyright (C) 2015 OldTimes Software
*/
#ifndef __JCLIENTMAIN__
#define __JCLIENTMAIN__

#include "platform.h"
#include "platform_module.h"
#include "platform_window.h"
#include "platform_math.h"

#include <jni.h>

#include "..\..\external\glee\GLee.h"

#include <GLFW/glfw3.h>

#define	CLIENTJVM_TITLE	"JClient"

extern GLFWwindow *gMainWindow;	// Main GL window

void JClient_Shutdown(void);

#endif