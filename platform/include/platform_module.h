/*	Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef __PLATFORMMODULE__
#define __PLATFORMMODULE__

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define	pMODULE_EXTENSION	".dll"
#define	pMODULE_EXPORT		__declspec(dllexport)
#define pMODULE_IMPORT		__declspec(dllimport)
#else   // Linux
#define	pMODULE_EXTENSION	".so"
#define pMODULE_EXPORT		__attribute__((visibility("default")))
#define pMODULE_IMPORT		__attribute__((visibility("hidden")))
#endif

extern	pFARPROC	pModule_FindFunction(pINSTANCE hModule,const char *cEntryFunction);

pINSTANCE	pModule_Load(const char *ccPath);

extern	void	pModule_Unload(pINSTANCE hModule);
extern	void	*pModule_LoadInterface(pINSTANCE hModule,const char *cPath,const char *cEntryFunction,void *vPoint);

#ifdef __cplusplus
}
#endif

#endif
