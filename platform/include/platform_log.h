/*	Copyright (C) 2011-2015 OldTimes Software
*/
#ifndef __PLATFORMLOG__
#define	__PLATFORMLOG__

#define	pLOG_GLOBAL "global"

#ifdef __cplusplus
extern "C" {
#endif

	void pLog_Write(const char *ccPath, const char *ccMessage, ...);
	void pLog_Clear(const char *ccPath);

#ifdef __cplusplus
}
#endif

#endif
