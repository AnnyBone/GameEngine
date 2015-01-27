/*	Copyright (C) 2015 OldTimes Software
*/
#include "platform.h"
#include "platform_module.h"
#include "platform_window.h"

/*
	EOS Template for Java Virtual Machine implementation.
*/

#include <jni.h>

#define	CLIENTJVM_TITLE	"ClientJVM"

JNIEnv	*jEnvironment;
JavaVM	*jVirtualMachine;

bool bClientJVMInitialized = false;

jint (*ClientJVM_Create)(JavaVM **pvm, void **penv, void *args);

void ClientJVM_Initialize(void)
{
	if (bClientJVMInitialized)
		return;

	printf("Initializing Java Virtual Machine...\n");

	JavaVMInitArgs	jArgs;
	JavaVMOption	jOptions[1];

	jOptions[0].optionString = "-Djava.class.path=c:\\projects";

	jArgs.ignoreUnrecognized = false;
	jArgs.nOptions = 1;
	jArgs.options = jOptions;
	jArgs.version = JNI_VERSION_1_8;

	if(JNI_CreateJavaVM(&jVirtualMachine, (void**)&jEnvironment, &jArgs) != JNI_OK)
	{
		gWindow_MessageBox(CLIENTJVM_TITLE, "Failed to create Java Virtual Machine!");
		return;
	}

	bClientJVMInitialized = true;
}

void ClientJVM_Shutdown(void)
{
	if (!bClientJVMInitialized)
		return;

	if (jVirtualMachine)
		jVirtualMachine->DestroyJavaVM();
}

int main(int argc, char *argv[])
{
	pINSTANCE	iLibrary;

	// Direct loading of JVM library.
	iLibrary = pModule_Load("C:\\Program Files\\Java\\jdk1.8.0_31\\jre\\bin\\server\\jvm");
	if (!iLibrary)
	{
		gWindow_MessageBox(CLIENTJVM_TITLE, "Failed to load JVM library!");
		return 0;
	}

	ClientJVM_Create = (void*)pModule_FindFunction(iLibrary, "JNI_CreateJavaVM");
	if (!ClientJVM_Create)
	{
		gWindow_MessageBox(CLIENTJVM_TITLE, "Failed to assign function!");
		return 0;
	}

	ClientJVM_Initialize();

	ClientJVM_Shutdown();

	return 0;
}

