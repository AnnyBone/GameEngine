/*	Copyright (C) 2015 OldTimes Software
*/
#ifndef __JCLIENTVIRTUALMACHINE__
#define __JCLIENTVIRTUALMACHINE__

void JVirtualMachine_Initialize(void);
void JVirtualMachine_CallStaticFunction(const char *ccFunction, jclass jClass);
void JVirtualMachine_Shutdown(void);

jclass JVirtualMachine_GetClass(const char *ccPath);

#endif