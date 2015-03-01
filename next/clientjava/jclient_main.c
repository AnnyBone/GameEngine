/*	Copyright (C) 2015 OldTimes Software
*/

#include "jclient_main.h"

#include "jclient_virtualmachine.h"
#include "jclient_video.h"
#include "jclient_input.h"

/*
	EOS Template for Java Virtual Machine implementation.
*/

void JClient_Initialize(void)
{
	pFileSystem_GetWorkingDirectory(Global.cCurrentDirectory);

	printf("Current directory: %s\n", Global.cCurrentDirectory);

	JVirtualMachine_Initialize();

	if (JVideo_Initialize())
	{
		glfwSetKeyCallback(gMainWindow, JInput_Frame);

		while (!glfwWindowShouldClose(gMainWindow))
		{
			glfwSwapBuffers(gMainWindow);

			glfwPollEvents();

			JVideo_Frame();
		}
	}
}

void JClient_Shutdown(void)
{
	JVirtualMachine_Shutdown();
	JVideo_Shutdown();

	exit(0);
}

int main(int argc, char *argv[])
{
	JClient_Initialize();
	JClient_Shutdown();

	return 0;
}

