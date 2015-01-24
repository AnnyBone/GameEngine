// JavaClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <jni.h>

JNIEnv	*jEnvironment;
JavaVM	*jVirtualMachine;

void ClientJVM_Initialize(void)
{
	printf("Initializing Java Virtual Machine...\n");

	JavaVMInitArgs	jArgs;
	JavaVMOption	jOptions[1];

	jOptions[0].optionString = "-Djava.class.path=c:\\projects";

	jArgs.ignoreUnrecognized = false;
	jArgs.nOptions = 1;
	jArgs.options = jOptions;
	jArgs.version = JNI_VERSION_1_8;

	JNI_CreateJavaVM(&jVirtualMachine, (void**)&jEnvironment, &jArgs);
}

void ClientJVM_Shutdown(void)
{
	jVirtualMachine->DestroyJavaVM();
}

int _tmain(int argc, _TCHAR* argv[])
{
	ClientJVM_Initialize();

	ClientJVM_Shutdown();

	return 0;
}

