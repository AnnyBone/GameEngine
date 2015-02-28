#include "jclient_main.h"

/*
	Java Virtual Machine
*/

#include "jclient_virtualmachine.h"

bool	bVMInitialized = false;

JNIEnv	*jEnvironment;
JavaVM	*jVirtualMachine;

jint(*_JVirtualMachine_Create)(JavaVM **pvm, void **penv, void *args);

void JVirtualMachine_Initialize(void)
{
	int iStatus;

	if (bVMInitialized)
		return;

	printf("Initializing virtual machine... ");

	pINSTANCE	iLibrary;

	// Direct loading of JVM library.
	iLibrary = pModule_Load("./jre/bin/server/jvm");
	if (!iLibrary)
	{
		gWindow_MessageBox(CLIENTJVM_TITLE, "Failed to load JVM library!");
		return;
	}

	_JVirtualMachine_Create = (void*)pModule_FindFunction(iLibrary, "JNI_CreateJavaVM");
	if (!_JVirtualMachine_Create)
	{
		gWindow_MessageBox(CLIENTJVM_TITLE, "Failed to assign function!");
		return;
	}

	JavaVMInitArgs	jArgs;
	JavaVMOption	jOptions[1];

	jOptions[0].optionString = "-Djava.class.path=c:\\projects";

	jArgs.ignoreUnrecognized = false;
	jArgs.nOptions = 1;
	jArgs.options = jOptions;
	jArgs.version = JNI_VERSION_1_8;

	iStatus = _JVirtualMachine_Create(&jVirtualMachine, (void**)&jEnvironment, &jArgs);
	if (iStatus != JNI_OK)
	{
		gWindow_MessageBox(CLIENTJVM_TITLE, "Failed to create Java Virtual Machine!");
		return;
	}

	bVMInitialized = true;

	printf("DONE!\n");

	// Attempt to call up main function.
	JVirtualMachine_CallStaticFunction("main", JVirtualMachine_GetClass("data/scripts/GameMain"));
}

void JVirtualMachine_CallStaticFunction(const char *ccFunction, jclass jClass)
{
	if (!bVMInitialized || !jClass)
		return;

	jmethodID jMethod = (*jEnvironment)->GetStaticMethodID(jEnvironment, jClass, "main", "([Ljava/lang/String;)V");
	if (!jMethod)
	{
		gWindow_MessageBox(CLIENTJVM_TITLE, "Failed to get static method! (%s)\n", ccFunction);
		return;
	}

	(*jEnvironment)->CallStaticVoidMethod(jEnvironment, jClass, jMethod);
}

jclass JVirtualMachine_GetClass(const char *ccPath)
{
	if (!bVMInitialized)
		return NULL;
	else if (ccPath[0] == ' ')
	{
		printf("Invalid string for class!\n");
		return NULL;
	}

	jclass jClass = (*jEnvironment)->FindClass(jEnvironment, ccPath);
	if (!jClass)
	{
		gWindow_MessageBox(CLIENTJVM_TITLE, "Failed to find class! (%s)\n", ccPath);
		return NULL;
	}

	return jClass;
}

void JVirtualMachine_Shutdown(void)
{
	if (!bVMInitialized)
		return;

	printf("Shutting down virtual machine... ");

	if (jVirtualMachine)
		(*jVirtualMachine)->DestroyJavaVM(jVirtualMachine);

	printf("DONE!\n");
}