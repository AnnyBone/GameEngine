#include "jclient_main.h"

#include "jclient_input.h"

void JInput_Initialize(void)
{}

void JInput_Frame(GLFWwindow *gWindow, int iKey, int iScanCode, int iAction, int iMods)
{
	switch (iKey)
	{
	case GLFW_KEY_ESCAPE:
		if (iAction == GLFW_PRESS)
			glfwSetWindowShouldClose(gWindow, true);
		break;
	case GLFW_KEY_UP:
	case GLFW_KEY_DOWN:
	case GLFW_KEY_LEFT:
	case GLFW_KEY_RIGHT:
		break;
	default:
		printf("Unhandled key input! (%i)\n", iKey);
	}
}