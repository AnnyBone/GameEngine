/*	Copyright (C) 2011-2016 OldTimes Software

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "engine_base.h"

/*
	Font Management
	TODO:
		Move into menu.
*/

#include "video.h"

typedef struct
{
	char *cName;

	unsigned int uiCharacters;
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
#ifdef VL_MODE_OPENGL
	char cPath[PLATFORM_MAX_PATH];

	sprintf(cPath, "%s/%s", g_state.path_fonts, ccName);

#ifdef _WIN32	// TODO: Move this over to platform.
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

	wglUseFontBitmapsA(NULL, 32, 96, glGenLists(96));

	DeleteObject(hFont);
#endif
#endif
}

/*	Basic routine for drawing fonts onto the screen.
*/
void Font_Draw(Font_t *fFont, const char *ccMessage, plVector3f_t vPos, plVector3f_t vColour)
{
	vlVertex_t voFont[4] = { { { 0 } } };

	/*	todo:
			need to reserve bind slot.
			calc/save font size.
	*/

	Video_ObjectTexture(&voFont[0], VIDEO_TEXTURE_DIFFUSE, 0, 0);
	Video_ObjectTexture(&voFont[1], VIDEO_TEXTURE_DIFFUSE, 1.0f, 0);
	Video_ObjectTexture(&voFont[2], VIDEO_TEXTURE_DIFFUSE, 1.0f, 1.0f);
	Video_ObjectTexture(&voFont[3], VIDEO_TEXTURE_DIFFUSE, 0, 1.0f);

	Math_VectorCopy(vPos,voFont[0].position);
	Math_VectorCopy(vPos,voFont[1].position);
	Math_VectorCopy(vPos,voFont[2].position);
	Math_VectorCopy(vPos,voFont[3].position);

#if 0
	voFont[1].vVertex[0] += sFontSurface->w;
	voFont[2].vVertex[0] += sFontSurface->w;
	voFont[2].vVertex[1] += sFontSurface->h;
	voFont[3].vVertex[1] += sFontSurface->h;
#endif

	Video_DrawFill(voFont, NULL, 0);
}

void Font_Shutdown(void)
{
	/*	todo:
			unload all loaded fonts.
	*/
}
