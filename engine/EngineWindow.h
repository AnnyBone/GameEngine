#ifndef __ENGINEWINDOW__
#define	__ENGINEWINDOW__

/*
	Generic Window Header
*/

#define WINDOW_MINIMUM_WIDTH	640
#define WINDOW_MINIMUM_HEIGHT	480

void Window_InitializeVideo(void);
void Window_UpdateVideo(void);
void Window_Shutdown(void);

int Window_GetWidth(void);
int Window_GetHeight(void);

#endif