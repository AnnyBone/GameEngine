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

#include <stdlib.h>

#include "engine_base.h"

#include "video.h"
#include "engine_client.h"	// [28/7/2013] Added for precache functions ~hogsy

#define	BLOCK_WIDTH		256
#define	BLOCK_HEIGHT	BLOCK_WIDTH

ConsoleVariable_t cvConsoleAlpha = { "screen_consolealpha", "0.7", true, false, "Sets the alpha value for the console background." }; //johnfitz

qpic_t	*draw_backtile;

typedef struct
{
	gltexture_t *gltexture;
	float		sl, tl, sh, th;
} glpic_t;

VideoCanvasType_t currentcanvas = CANVAS_NONE; //johnfitz -- for GL_SetCanvas

//==============================================================================
//
//  PIC CACHING
//
//==============================================================================

typedef struct cachepic_s
{
	char		name[MAX_QPATH];
	qpic_t		pic;
	uint8_t		padding[32];	// for appended glpic
} cachepic_t;

#define	MAX_CACHED_PICS		128
cachepic_t	menu_cachepics[MAX_CACHED_PICS];
int			menu_numcachepics;

//  scrap allocation
//  Allocate all the little status bar obejcts into a single texture
//  to crutch up stupid hardware / drivers

#define	MAX_SCRAPS		2

int			scrap_allocated[MAX_SCRAPS][BLOCK_WIDTH];
uint8_t		scrap_texels[MAX_SCRAPS][BLOCK_WIDTH*BLOCK_HEIGHT]; //johnfitz -- removed *4 after BLOCK_HEIGHT
bool		scrap_dirty;
gltexture_t	*scrap_textures[MAX_SCRAPS]; //johnfitz

/*	Returns an index into scrap_texnums[] and the position inside it
*/
int Scrap_AllocBlock (int w, int h, int *x, int *y)
{
	int		i, j;
	int		best, best2;
	int		texnum;

	for (texnum=0 ; texnum<MAX_SCRAPS ; texnum++)
	{
		best = BLOCK_HEIGHT;

		for (i=0 ; i<BLOCK_WIDTH-w ; i++)
		{
			best2 = 0;

			for (j=0 ; j<w ; j++)
			{
				if (scrap_allocated[texnum][i+j] >= best)
					break;
				if (scrap_allocated[texnum][i+j] > best2)
					best2 = scrap_allocated[texnum][i+j];
			}

			if (j == w)
			{	// this is a valid spot
				*x = i;
				*y = best = best2;
			}
		}

		if (best + h > BLOCK_HEIGHT)
			continue;

		for (i=0 ; i<w ; i++)
			scrap_allocated[texnum][*x + i] = best + h;

		return texnum;
	}

	Sys_Error ("Scrap_AllocBlock: full"); //johnfitz -- correct function name
	return 0; //johnfitz -- shut up compiler
}

void Scrap_Upload (void)
{
	char name[8];
	int	i;

	for (i=0; i<MAX_SCRAPS; i++)
	{
		sprintf (name, "scrap%i", i);
		scrap_textures[i] = TexMgr_LoadImage (NULL, name, BLOCK_WIDTH, BLOCK_HEIGHT, SRC_INDEXED, scrap_texels[i],
			"", (unsigned int)scrap_texels[i], TEXPREF_ALPHA | TEXPREF_OVERWRITE | TEXPREF_NOPICMIP);
	}

	scrap_dirty = false;
}

//==================================================================

void Draw_MaterialSurface(Material_t *mMaterial, int iSkin,	int x, int y, int w, int h,	float fAlpha)
{
	VideoVertex_t voSurface[4];

	// Sloppy, but in the case that there's nothing valid...
	if (!mMaterial)
	{
		Draw_Rectangle(x, y, w, h, pl_red);
		Draw_String(x, y, "INVALID MATERIAL!");
		return;
	}

	// Disable depth testing.
	vlDisable(VIDEO_DEPTH_TEST);

	// Set the colour.
	Video_ObjectColour(&voSurface[0], 1.0f, 1.0f, 1.0f, fAlpha);
	Video_ObjectColour(&voSurface[1], 1.0f, 1.0f, 1.0f, fAlpha);
	Video_ObjectColour(&voSurface[2], 1.0f, 1.0f, 1.0f, fAlpha);
	Video_ObjectColour(&voSurface[3], 1.0f, 1.0f, 1.0f, fAlpha);

	// Set the texture coords.
	Video_ObjectTexture(&voSurface[0], 0, 0, 0);
	Video_ObjectTexture(&voSurface[1], 0, 1.0f, 0);
	Video_ObjectTexture(&voSurface[2], 0, 1.0f, 1.0f);
	Video_ObjectTexture(&voSurface[3], 0, 0, 1.0f);

	// Set the vertex coords.
	Video_ObjectVertex(&voSurface[0], x, y, 0);
	Video_ObjectVertex(&voSurface[1], x+w, y, 0);
	Video_ObjectVertex(&voSurface[2], x+w, y+h, 0);
	Video_ObjectVertex(&voSurface[3], x, y+h, 0);

	// Throw it off to the rendering pipeline.
	Video_DrawFill(voSurface, mMaterial, iSkin);

	vlEnable(VIDEO_DEPTH_TEST);
}

/*	TODO: Make me obsolete!
*/
void Draw_ExternPic(char *path,float alpha,int x,int y,int w,int h)
{
#if 0
	int	i;

	// [15/9/2013] Fixed somewhat (clean this up) ~hogsy
	for(i = 0; i < sizeof(gMenuTexture); i++)
		if(!gMenuTexture[i])
		{
			Video_SetTexture(notexture);
			break;
		}
		else if(!strcmp(gMenuTexture[i]->name,path))
		{
			Video_SetTexture(gMenuTexture[i]);
			break;
		}

	Video_ResetCapabilities(false);
	Video_EnableCapabilities(VIDEO_BLEND);
	Video_DisableCapabilities(VIDEO_DEPTH_TEST);

	{
		VideoVertex_t	voPicture[4];

		for(i = 0; i < 4; i++)
		{
			// Set RGB values to 1.0f...
			Math_VectorSet(1.0f,voPicture[i].mvColour);

			// Alpha needs to be set last, to the given value...
			voPicture[i].mvColour[3] = alpha;
		}

		// Give each vertex the coord it needs...
		voPicture[0].mvPosition[1]	=
		voPicture[1].mvPosition[1]	= y;
		voPicture[2].mvPosition[1]	=
		voPicture[3].mvPosition[1]	= y+h;
		voPicture[1].mvPosition[0]	=
		voPicture[2].mvPosition[0]	= x+w;
		voPicture[0].mvPosition[0]	=
		voPicture[3].mvPosition[0]	= x;

		// Give each texture coord the coord it needs...
		voPicture[0].mvST[0][0]	=
		voPicture[0].mvST[0][1]	=
		voPicture[1].mvST[0][1]	=
		voPicture[3].mvST[0][0]	= 0;
		voPicture[1].mvST[0][0]	=
		voPicture[2].mvST[0][0]	=
		voPicture[2].mvST[0][1]	=
		voPicture[3].mvST[0][1]	= 1.0f;

		// Throw it off to the rendering pipeline.
		Video_DrawFill(voPicture,NULL, 0);
	}

	Video_ResetCapabilities(true);
#endif
}
//==================================================================

void SwapPic (qpic_t *pic)
{
	pic->width	= LittleLong(pic->width);
	pic->height = LittleLong(pic->height);
}

qpic_t	*Draw_CachePic(char *path)
{
	cachepic_t	*pic;
	int			i;
	glpic_t		*gl;

	for (pic=menu_cachepics, i=0 ; i<menu_numcachepics ; pic++, i++)
		if (!strcmp (path, pic->name))
			return &pic->pic;

	if (menu_numcachepics == MAX_CACHED_PICS)
		Sys_Error ("menu_numcachepics == MAX_CACHED_PICS");

	menu_numcachepics++;
	strncpy(pic->name, path, sizeof(pic->name));

	// load the pic from disk
	qpic_t *dat = COM_LoadHeapFile(path);
	if(!dat)
	{
		Con_Warning("Failed to load cached texture (%s)!\n", path);
		return NULL;
	}

	SwapPic(dat);

	pic->pic.width = dat->width;
	pic->pic.height = dat->height;

	gl = (glpic_t *)pic->pic.data;
	gl->gltexture = TexMgr_LoadImage (NULL, path, dat->width, dat->height, SRC_INDEXED, dat->data, path,
									  sizeof(int)*2, TEXPREF_ALPHA | TEXPREF_PAD | TEXPREF_NOPICMIP); //johnfitz -- TexMgr
	gl->sl = 0;
	gl->sh = (float)dat->width/(float)TexMgr_PadConditional(dat->width); //johnfitz
	gl->tl = 0;
	gl->th = (float)dat->height/(float)TexMgr_PadConditional(dat->height); //johnfitz

	free(dat);

	return &pic->pic;
}

qpic_t *Draw_MakePic (char *name, int width, int height, byte *data)
{
	int			flags = TEXPREF_NEAREST|TEXPREF_ALPHA|TEXPREF_PERSIST|TEXPREF_NOPICMIP|TEXPREF_PAD;
	qpic_t		*pic;
	glpic_t		*gl;

	pic = (qpic_t*)Hunk_Alloc (sizeof(qpic_t) - 4 + sizeof (glpic_t));
	pic->width	= width;
	pic->height = height;

	gl = (glpic_t *)pic->data;
	gl->gltexture = TexMgr_LoadImage (NULL, name, width, height, SRC_INDEXED, data, "", (unsigned int)data, flags);
	gl->sl = 0;
	gl->sh = (float)width/(float)TexMgr_PadConditional(width);
	gl->tl = 0;
	gl->th = (float)height/(float)TexMgr_PadConditional(height);

	return pic;
}

//==============================================================================
//
//  INIT
//
//==============================================================================

void Draw_NewGame (void)
{
	cachepic_t	*pic;
	int			i;

	// Empty scrap and reallocate gltextures
	memset(&scrap_allocated,0,sizeof(scrap_allocated));
	memset(&scrap_texels,255,sizeof(scrap_texels));

	Scrap_Upload (); //creates 2 empty gltextures

	// empty lmp cache
	for (pic = menu_cachepics, i = 0; i < menu_numcachepics; pic++, i++)
		pic->name[0] = 0;
	menu_numcachepics = 0;
}

void Draw_Init (void)
{
	Cvar_RegisterVariable(&cvConsoleAlpha,NULL);

	// clear scrap and allocate gltextures
	memset(&scrap_allocated, 0, sizeof(scrap_allocated));
	memset(&scrap_texels, 255, sizeof(scrap_texels));
	Scrap_Upload (); //creates 2 empty textures
}

//==============================================================================
//
//  2D DRAWING
//
//==============================================================================

void Draw_Character(int x, int y, int num)
{
	VideoVertex_t		voCharacter[4] = { { { 0 } } };
	int					row, col;
	float				frow,fcol,size;

	if(y <= -8)
		return;			// totally off screen

	num &= 255;
	if(num == 32)
		return; //don't waste verts on spaces

	row = num>>4;
	col = num&15;

	frow = row*0.0625f;
	fcol = col*0.0625f;
	size = 0.0625f;

	vlDisable(VIDEO_DEPTH_TEST);

	Video_ObjectVertex(&voCharacter[0], x, y, 0);
	Video_ObjectColour(&voCharacter[0], 1.0f, 1.0f, 1.0f, 1.0f);
	Video_ObjectTexture(&voCharacter[0], VIDEO_TEXTURE_DIFFUSE, fcol, frow);

	Video_ObjectVertex(&voCharacter[1], x+8, y, 0);
	Video_ObjectColour(&voCharacter[1], 1.0f, 1.0f, 1.0f, 1.0f);
	Video_ObjectTexture(&voCharacter[1], VIDEO_TEXTURE_DIFFUSE, fcol+size, frow);

	Video_ObjectVertex(&voCharacter[2], x+8, y+8, 0);
	Video_ObjectColour(&voCharacter[2], 1.0f, 1.0f, 1.0f, 1.0f);
	Video_ObjectTexture(&voCharacter[2], VIDEO_TEXTURE_DIFFUSE, fcol+size, frow+size);

	Video_ObjectVertex(&voCharacter[3], x, y+8, 0);
	Video_ObjectColour(&voCharacter[3], 1.0f, 1.0f, 1.0f, 1.0f);
	Video_ObjectTexture(&voCharacter[3], VIDEO_TEXTURE_DIFFUSE, fcol, frow+size);

	Video_DrawFill(voCharacter, g_mGlobalConChars, 0);

	vlEnable(VIDEO_DEPTH_TEST);
}

/*	Draws a simple string of text on the screen.
*/
void Draw_String(int x, int y, const char *msg)
{
	if (y <= -8)
		return;

	Material_Draw(g_mGlobalColour, NULL, 0, 0, false);

	while (*msg)
	{
		Draw_Character(x, y, *msg);
		msg++;
		x += 8;
	}

	Material_Draw(g_mGlobalColour, NULL, 0, 0, true);
}

void Draw_ConsoleBackground(void)
{
	float		alpha = cvConsoleAlpha.value;
	Colour_t	black, lightblack;

	if (cls.state != ca_connected)
		// TODO: we're not clearing buffers when disconnected...
		alpha = 1;

	Math_VectorSet(0, black);
	Math_VectorSet(0, lightblack);

	black[3] = 255.0f;
	lightblack[3] = alpha;

	GL_SetCanvas(CANVAS_CONSOLE);

	Draw_GradientFill(0, 0, vid.conwidth, vid.conheight, black, lightblack);
}

void Draw_GradientBackground(void)
{
#ifdef KATANA_CORE_GLIDE
#else
	VideoCanvasType_t vctOldCanvas; //johnfitz

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	Draw_ResetCanvas();

	vctOldCanvas = (VideoCanvasType_t)currentcanvas;
	GL_SetCanvas(CANVAS_DEFAULT);
	currentcanvas = vctOldCanvas;

	Colour_t
		cTop	= { 0.1f, 0.1f, 0.1f, 1.0f },
		cBottom = { 0.5f, 0.5f, 0.5f, 1.0f };
	Draw_GradientFill(0, 0, Video.iWidth, Video.iHeight, cTop, cBottom);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glViewport(0, 0, Video.iWidth, Video.iHeight);
#endif
}

/*	This repeats a 64*64 tile graphic to fill the screen around a sized down
	refresh window.
*/
void Draw_TileClear (int x, int y, int w, int h)
{
#if 0
	glpic_t	*gl;

	gl = (glpic_t *)draw_backtile->data;

	glColor3f(1,1,1);

	Video_SetTexture(gl->gltexture);

	glBegin(GL_QUADS);
	glTexCoord2f(x/64.0,y/64.0);
	glVertex2f(x,y);
	glTexCoord2f((x+w)/64.0,y/64.0);
	glVertex2f(x+w,y);
	glTexCoord2f((x+w)/64.0,(y+h)/64.0);
	glVertex2f(x+w,y+h);
	glTexCoord2f(x/64.0,(y+h)/64.0);
	glVertex2f(x,y+h);
	glEnd();
#else
// TODO: implement this using new pipeline
#endif
}

void Draw_Line(MathVector3f_t mvStart, MathVector3f_t mvEnd)
{
	VideoVertex_t	voLine[2] = { { { 0 } } };

	Video_ObjectVertex(&voLine[0], mvStart[0], mvStart[1], mvStart[2]);
	Video_ObjectColour(&voLine[0], 1.0f, 0, 0, 1.0f);
	Video_ObjectVertex(&voLine[1], mvEnd[0], mvEnd[1], mvEnd[2]);
	Video_ObjectColour(&voLine[1], 1.0f, 0, 0, 1.0f);

	Video_DrawObject(voLine, VIDEO_PRIMITIVE_LINES, 2, NULL, 0);
}

/*	Debugging tool.
*/
void Draw_CoordinateAxes(float x,float y,float z)
{
	VideoVertex_t voLine[2] = { { { 0 } } };

	Video_ObjectVertex(&voLine[0], 0, 0, 0);
	Video_ObjectColour(&voLine[0], 1.0f, 0, 0, 1.0f);
	Video_ObjectVertex(&voLine[1], 10, 0, 0);
	Video_ObjectColour(&voLine[1], 1.0f, 0, 0, 1.0f);
	Video_DrawObject(voLine, VIDEO_PRIMITIVE_LINES, 2, NULL, 0);

	Video_ObjectVertex(&voLine[0], 0, 0, 0);
	Video_ObjectColour(&voLine[0], 0, 1.0f, 0, 1.0f);
	Video_ObjectVertex(&voLine[1], 0, 10, 0);
	Video_ObjectColour(&voLine[1], 0, 1.0f, 0, 1.0f);
	Video_DrawObject(voLine, VIDEO_PRIMITIVE_LINES, 2, NULL, 0);

	Video_ObjectVertex(&voLine[0], 0, 0, 0);
	Video_ObjectColour(&voLine[0], 0, 0, 1.0f, 1.0f);
	Video_ObjectVertex(&voLine[1], 0, 0, 10);
	Video_ObjectColour(&voLine[1], 0, 0, 1.0f, 1.0f);
	Video_DrawObject(voLine, VIDEO_PRIMITIVE_LINES, 2, NULL, 0);
}

void Draw_Grid(float x, float y, float z, int grid_size)
{
#ifdef KATANA_CORE_GL
	int i;

	glPushMatrix();

	glTranslatef(x, y, z);

	vlEnable(VIDEO_BLEND);
	vlDisable(VIDEO_TEXTURE_2D);

//	vlBlendFunc(VIDEO_BLEND_DEFAULT);

	glColor4f(0, 0, 1.0f, 0.5f);

	glLineWidth(1.0f);
	glBegin(GL_LINES);

	for (i = 0; i <= (4096 / grid_size); i++)
	{
		glVertex2i(-4096, (i * grid_size) * -1);
		glVertex2i(4096, (i * grid_size) * -1);
		glVertex2i(-4096, i * grid_size);
		glVertex2i(4096, i * grid_size);

		glVertex2i((i * grid_size) * -1, -4096);
		glVertex2i((i * grid_size) * -1, 4096);
		glVertex2i(i * grid_size, -4096);
		glVertex2i(i * grid_size, 4096);
	}

	glEnd();

	glColor4f(0, 0, 0.3f, 1.0f);
	glLineWidth(2.0f);
	glBegin(GL_LINES);

	for (i = 0; i <= 64; i++)
	{
		glVertex2i(-4096, (i * 64) * -1);
		glVertex2i(4096, (i * 64) * -1);
		glVertex2i(-4096, i * 64);
		glVertex2i(4096, i * 64);

		glVertex2i((i * 64) * -1, -4096);
		glVertex2i((i * 64) * -1, 4096);
		glVertex2i(i * 64, -4096);
		glVertex2i(i * 64, 4096);
	}

	glEnd();

	glLineWidth(1.0f);
	glColor3f(1, 1, 1);

	vlDisable(VIDEO_BLEND);
	vlEnable(VIDEO_TEXTURE_2D);

	glPopMatrix();
}

void Draw_Rectangle(int x, int y, int w, int h, Colour_t colour)
{
	VideoVertex_t	voFill[4];

	Math_Vector4Copy(colour, voFill[0].mvColour);
	Math_Vector4Copy(colour, voFill[1].mvColour);
	Math_Vector4Copy(colour, voFill[2].mvColour);
	Math_Vector4Copy(colour, voFill[3].mvColour);

	if (colour[3] < 1)
		vlEnable(VIDEO_BLEND);
	vlDisable(VIDEO_DEPTH_TEST | VIDEO_TEXTURE_2D);

	Video_ObjectVertex(&voFill[0], x, y, 0);
	Video_ObjectTexture(&voFill[0], 0, h, w);
	Video_ObjectVertex(&voFill[1], x+w, y, 0);
	Video_ObjectTexture(&voFill[1], 0, h + w, w);
	Video_ObjectVertex(&voFill[2], x+w, y+h, 0);
	Video_ObjectTexture(&voFill[2], 0, h + w, w + h);
	Video_ObjectVertex(&voFill[3], x, y+h, 0);
	Video_ObjectTexture(&voFill[0], 0, h, w + h);

	Video_DrawFill(voFill, NULL, 0);

	if (colour[3] < 1)
		vlDisable(VIDEO_BLEND);
	vlEnable(VIDEO_DEPTH_TEST | VIDEO_TEXTURE_2D);
}

void Draw_GradientFill(int x, int y, int w, int h, Colour_t mvTopColour, Colour_t mvBottomColour)
{
	VideoVertex_t	voFill[4];

	if ((mvTopColour[3] < 1) || (mvBottomColour[3] < 1))
		vlEnable(VIDEO_BLEND);
	vlDisable(VIDEO_DEPTH_TEST | VIDEO_TEXTURE_2D);

	Video_ObjectVertex(&voFill[0], x, y, 0);
	Video_ObjectColour(&voFill[0], mvTopColour[0], mvTopColour[1], mvTopColour[2], mvTopColour[3]);
	Video_ObjectVertex(&voFill[1], x + w, y, 0);
	Video_ObjectColour(&voFill[1], mvTopColour[0], mvTopColour[1], mvTopColour[2], mvTopColour[3]);
	Video_ObjectVertex(&voFill[2], x + w, y + h, 0);
	Video_ObjectColour(&voFill[2], mvBottomColour[0], mvBottomColour[1], mvBottomColour[2], mvBottomColour[3]);
	Video_ObjectVertex(&voFill[3], x, y + h, 0);
	Video_ObjectColour(&voFill[3], mvBottomColour[0], mvBottomColour[1], mvBottomColour[2], mvBottomColour[3]);

	Video_DrawFill(voFill, NULL, 0);

	if ((mvTopColour[3] < 1) || (mvBottomColour[3] < 1))
		vlDisable(VIDEO_BLEND);
	vlEnable(VIDEO_DEPTH_TEST | VIDEO_TEXTURE_2D);
}

void Draw_FadeScreen (void)
{
	VideoVertex_t	voFade[4];

	GL_SetCanvas(CANVAS_DEFAULT);

	vlEnable(VIDEO_BLEND);

	Video_ObjectVertex(&voFade[0], 0, 0, 0);
	Video_ObjectColour(&voFade[0], 1.0f, 1.0f, 1.0f, 0.5f);

	Video_ObjectVertex(&voFade[1], glwidth, 0, 0);
	Video_ObjectColour(&voFade[1], 1.0f, 1.0f, 1.0f, 0.5f);

	Video_ObjectVertex(&voFade[2], glwidth, glheight, 0);
	Video_ObjectColour(&voFade[2], 1.0f, 1.0f, 1.0f, 0.5f);

	Video_ObjectVertex(&voFade[3], 0, glheight, 0);
	Video_ObjectColour(&voFade[3], 1.0f, 1.0f, 1.0f, 0.5f);

	Video_DrawFill(voFade, NULL, 0);

	vlDisable(VIDEO_BLEND);
}

/*	Draws the little blue disc in the corner of the screen.
	Call before beginning any disc IO.
*/
void Draw_BeginDisc(void)
{
#ifdef KATANA_CORE_GL
	//static float fDiscRotation = 0;
	int	iViewport[4]; //johnfitz
	VideoCanvasType_t oldcanvas; //johnfitz

	// Don't draw if it's dedicated.
	if (!Video.bInitialized || (cls.state == ca_dedicated))
		return;

	//johnfitz -- canvas and matrix stuff
	glGetIntegerv(GL_VIEWPORT,iViewport);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	oldcanvas = (VideoCanvasType_t)currentcanvas;
	GL_SetCanvas (CANVAS_TOPRIGHT);
	currentcanvas = oldcanvas; // a bit of a hack, since GL_SetCanvas doesn't know we are going to pop the stack
	//johnfitz

	glDrawBuffer(GL_FRONT);
	Draw_MaterialSurface(g_mHDAccess, 0, 320 - 74, 10, 64, 64, 1);
	glDrawBuffer(GL_BACK);

	//johnfitz -- restore everything so that 3d rendering isn't fucked up
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glViewport(iViewport[0],iViewport[1],iViewport[2],iViewport[3]);
	//johnfitz
#endif
}

void GL_SetCanvas (VideoCanvasType_t newcanvas)
{
#ifdef KATANA_CORE_GL
	extern vrect_t		scr_vrect;
	float				s;
	int					lines;

	if (newcanvas == currentcanvas)
		return;
	currentcanvas = newcanvas;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();

	switch(newcanvas)
	{
	case CANVAS_DEFAULT:
		glOrtho (0, glwidth, glheight, 0, -99999, 99999);
		glViewport (glx, gly, glwidth, glheight);
		break;
	case CANVAS_CONSOLE:
		lines = vid.conheight - (scr_con_current * vid.conheight / glheight);
		glOrtho(0,vid.conwidth,vid.conheight + lines,lines,-99999,99999);
		glViewport (glx, gly, glwidth, glheight);
		break;
	case CANVAS_MENU:
		s = Math_Min((float)glwidth / 640.0, (float)glheight / 480.0);
		s = Math_Clamp(1.0, scr_menuscale.value, s);
		glOrtho (0, 640, 480, 0, -99999, 99999);
		glViewport (glx, gly, glwidth, glheight);
		break;
	case CANVAS_SBAR:
		s = Math_Clamp(1.0, scr_sbarscale.value, (float)glwidth / 320.0);
		glOrtho (0, 320, 48, 0, -99999, 99999);
		glViewport (glx + (glwidth - 320*s) / 2, gly, 320*s, 48*s);
		break;
	case CANVAS_WARPIMAGE:
		glOrtho (0, 128, 0, 128, -99999, 99999);
		glViewport (glx, gly+glheight-gl_warpimagesize, gl_warpimagesize, gl_warpimagesize);
		break;
	case CANVAS_CROSSHAIR: //0,0 is center of viewport
		s = Math_Clamp(1.0, scr_crosshairscale.value, 10.0);
		glOrtho (scr_vrect.width/-2/s, scr_vrect.width/2/s, scr_vrect.height/2/s, scr_vrect.height/-2/s, -99999, 99999);
		glViewport (scr_vrect.x, glheight - scr_vrect.y - scr_vrect.height, scr_vrect.width & ~1, scr_vrect.height & ~1);
		break;
	case CANVAS_BOTTOMLEFT: //used by devstats
		s = (float)glwidth/vid.conwidth; //use console scale
		glOrtho (0, 320, 200, 0, -99999, 99999);
		glViewport (glx, gly, 320*s, 200*s);
		break;
	case CANVAS_BOTTOMRIGHT: //used by fps/clock
		s = (float)glwidth/vid.conwidth; //use console scale
		glOrtho (0, 320, 200, 0, -99999, 99999);
		glViewport (glx+glwidth-320*s, gly, 320*s, 200*s);
		break;
	case CANVAS_TOPRIGHT: //used by disc
		s = 1;
		glOrtho (0, 320, 200, 0, -99999, 99999);
		glViewport (glx+glwidth-320*s, gly+glheight-200*s, 320*s, 200*s);
		break;
	default:
		Sys_Error ("GL_SetCanvas: bad canvas type");
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();
#endif
}

void Draw_ResetCanvas(void)
{
	currentcanvas = (VideoCanvasType_t)-1;

	GL_SetCanvas(CANVAS_DEFAULT);
}

/*
	Entities
*/

/*	Calls up buffer and draws it.
*/
void Draw_StaticEntity(ClientEntity_t *entity)
{
	// TODO: TEMPORARY DEBUGGING STUFF!!!!
	vlPushMatrix();

	R_RotateForEntity(entity->origin, entity->angles);

	Material_Draw(entity->model->mAssignedMaterials, 0, 0, 0, false);
	VideoObject_DrawImmediate(&entity->model->objects[entity->frame]);
	Material_Draw(entity->model->mAssignedMaterials, 0, 0, 0, true);

	vlPopMatrix();
	// TODO: TEMPORARY DEBUGGING STUFF!!!!
}

void Draw_VertexEntity(ClientEntity_t *entity)
{
	vlPushMatrix();

	R_RotateForEntity(entity->origin, entity->angles);

	VideoObject_DrawImmediate(&entity->model->objects[entity->frame]);

	vlPopMatrix();
}

void Draw_Entity(ClientEntity_t *entity)
{
	if (!entity->model)
	{
		MathVector3f_t start, end;

		Math_VectorCopy(entity->origin, start);
		Math_VectorCopy(entity->origin, end);
		start[0] += 5;
		end[0] -= 5;
		Draw_Line(start, end);

		Math_VectorCopy(entity->origin, start);
		Math_VectorCopy(entity->origin, end);
		start[1] += 5;
		end[1] -= 5;
		Draw_Line(start, end);

		Math_VectorCopy(entity->origin, start);
		Math_VectorCopy(entity->origin, end);
		start[2] += 5;
		end[2] -= 5;
		Draw_Line(start, end);
		return;
	}

	switch (entity->model->type)
	{
#if 1	// Old legacy draws
	case MODEL_TYPE_MD2:
		Alias_Draw(entity);
		break;
#endif
	case MODEL_TYPE_STATIC:
		Draw_StaticEntity(entity);
		break;
	case MODEL_TYPE_SKELETAL:
		break;
	case MODEL_TYPE_VERTEX:
		Draw_VertexEntity(entity);
		break;
	case MODEL_TYPE_LEVEL:
		Brush_Draw(entity);
		break;
	default:
		Console_ErrorMessage(false, entity->model->name, "Unrecognised model type.");
	}
}
