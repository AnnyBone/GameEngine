/*	Copyright (C) 2011-2015 OldTimes Software
*/

#include "quakedef.h"

/*
	Font Management
*/

#include "engine_video.h"

typedef struct
{
	char		*cName;
} Font_t;

Font_t	fFonts[32];

void Font_Load(const char *ccName,int iSize);

void Font_Initialize(void)
{
	// Load a basic set of fonts...
	Font_Load("Xolonium-Regular",24);
	Font_Load("Xolonium-Bold",24);
}

/*	Loads the given font for later usage.
	Should be used during initialization OR during game-load.
*/
void Font_Load(const char *ccName,int iSize)
{
#ifdef _WIN32
	HFONT	hFont;

	hFont = CreateFontA(
		-iSize,
		0,
		0,
		0,
		FW_NORMAL,
		FALSE,
		FALSE,
		FALSE,
		ANSI_CHARSET,
		OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH,
		ccName);
	if (!hFont)
	{
		Con_Warning("Failed to load font! (%s)\n");
		return;
	}

	wglUseFontBitmaps(NULL, 32, 96, glGenLists(96));
#endif
}

/*	Basic routine for drawing fonts onto the screen.
*/
void Font_Draw(Font_t *fFont,const char *ccMessage,vec3_t vPos,vec3_t vColour)
{
	VideoObject_t	voFont[4] = { 0 };

	/*	todo:
			need to reserve bind slot.
			calc/save font size.
	*/

	Video_ObjectTexture(&voFont[0], VIDEO_TEXTURE_DIFFUSE, 0, 0);
	Video_ObjectTexture(&voFont[1], VIDEO_TEXTURE_DIFFUSE, 1.0f, 0);
	Video_ObjectTexture(&voFont[2], VIDEO_TEXTURE_DIFFUSE, 1.0f, 1.0f);
	Video_ObjectTexture(&voFont[3], VIDEO_TEXTURE_DIFFUSE, 0, 1.0f);

	Math_VectorCopy(vPos,voFont[0].vVertex);
	Math_VectorCopy(vPos,voFont[1].vVertex);
	Math_VectorCopy(vPos,voFont[2].vVertex);
	Math_VectorCopy(vPos,voFont[3].vVertex);

#if 0
	voFont[1].vVertex[0]	+= sFontSurface->w;
	voFont[2].vVertex[0]	+= sFontSurface->w;
	voFont[2].vVertex[1]	+= sFontSurface->h;
	voFont[3].vVertex[1]	+= sFontSurface->h;
#endif

	Video_DrawFill(voFont,NULL);
}

void Font_Shutdown(void)
{
	int i;

	/*	todo:
			unload all loaded fonts.
	*/
}
