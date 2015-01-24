/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
	Copyright (C) 2011-2015 OldTimes Software

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
#include "quakedef.h"

#include "engine_videoshadow.h"
#include "engine_console.h"
#include "engine_video.h"
#include "engine_client.h"	// [28/7/2013] Added for precache functions ~hogsy

#define	BLOCK_WIDTH		256
#define	BLOCK_HEIGHT	BLOCK_WIDTH

cvar_t		cvConsoleAlpha		= { "screen_consolealpha",	"0.7",						true,	false,  "Sets the alpha value for the console background."  }; //johnfitz
cvar_t		cvConsoleBackground	= { "screen_consoleback",	"",							true,	false,  "Sets the path for the console background."         };
cvar_t		cvConsoleChars		= { "screen_consolechars",	"textures/engine/conchars",	true,   false,  "Sets the path for the console font."	            };

qpic_t	*draw_backtile;

gltexture_t *gCharTexture; //johnfitz

typedef struct
{
	gltexture_t *gltexture;
	float		sl, tl, sh, th;
} glpic_t;

int currentcanvas = CANVAS_NONE; //johnfitz -- for GL_SetCanvas

//==============================================================================
//
//  PIC CACHING
//
//==============================================================================

typedef struct cachepic_s
{
	char		name[MAX_QPATH];
	qpic_t		pic;
	byte		padding[32];	// for appended glpic
} cachepic_t;

#define	MAX_CACHED_PICS		128
cachepic_t	menu_cachepics[MAX_CACHED_PICS];
int			menu_numcachepics;

//  scrap allocation
//  Allocate all the little status bar obejcts into a single texture
//  to crutch up stupid hardware / drivers

#define	MAX_SCRAPS		2

int			scrap_allocated[MAX_SCRAPS][BLOCK_WIDTH];
byte		scrap_texels[MAX_SCRAPS][BLOCK_WIDTH*BLOCK_HEIGHT]; //johnfitz -- removed *4 after BLOCK_HEIGHT
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
			"", (unsigned)scrap_texels[i], TEXPREF_ALPHA | TEXPREF_OVERWRITE | TEXPREF_NOPICMIP);
	}

	scrap_dirty = false;
}

//==================================================================

extern gltexture_t	*gMenuTexture[128];

/*	TODO: Make me obsolete!
*/
void Draw_ExternPic(char *path,float alpha,int x,int y,int w,int h)
{
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
		VideoObject_t	voPicture[4] = { 0 };

		for(i = 0; i < 4; i++)
		{
			// Set RGB values to 1.0f...
			Math_VectorSet(1.0f,voPicture[i].vColour);

			// Alpha needs to be set last, to the given value...
			voPicture[i].vColour[3] = alpha;
		}

		// Give each vertex the coord it needs...
		voPicture[0].vVertex[1]	=
		voPicture[1].vVertex[1]	= y;
		voPicture[2].vVertex[1]	=
		voPicture[3].vVertex[1]	= y+h;
		voPicture[1].vVertex[0]	=
		voPicture[2].vVertex[0]	= x+w;
		voPicture[0].vVertex[0]	=
		voPicture[3].vVertex[0]	= x;

		// Give each texture coord the coord it needs...
		voPicture[0].vTextureCoord[0][0]	=
		voPicture[0].vTextureCoord[0][1]	= 
		voPicture[1].vTextureCoord[0][1]	= 
		voPicture[3].vTextureCoord[0][0]	= 0;
		voPicture[1].vTextureCoord[0][0]	=
		voPicture[2].vTextureCoord[0][0]	= 
		voPicture[2].vTextureCoord[0][1]	= 
		voPicture[3].vTextureCoord[0][1]	= 1.0f;

		// Throw it off to the rendering pipeline.
		Video_DrawFill(voPicture,NULL);
	}

	Video_ResetCapabilities(true);
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
	qpic_t		*dat;
	glpic_t		*gl;

	for (pic=menu_cachepics, i=0 ; i<menu_numcachepics ; pic++, i++)
		if (!strcmp (path, pic->name))
			return &pic->pic;

	if (menu_numcachepics == MAX_CACHED_PICS)
		Sys_Error ("menu_numcachepics == MAX_CACHED_PICS");

	menu_numcachepics++;
	strcpy(pic->name, path);

	// load the pic from disk
	dat = (qpic_t *)COM_LoadTempFile (path);
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
	gl->gltexture = TexMgr_LoadImage (NULL, name, width, height, SRC_INDEXED, data, "", (unsigned)data, flags);
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

void Draw_LoadPics (void)
{
	unsigned int	w,h;
	byte		    *data;

	data = Image_LoadImage(cvConsoleChars.string,&w,&h);
	if(!data)
		Sys_Error("Failed to load %s!",cvConsoleChars.string);

	gCharTexture = TexMgr_LoadImage(NULL,cvConsoleChars.string,w,h,SRC_RGBA,data,cvConsoleChars.string,0,TEXPREF_NEAREST|TEXPREF_ALPHA);
}

void Draw_NewGame (void)
{
	cachepic_t	*pic;
	int			i;

	// Empty scrap and reallocate gltextures
	memset(&scrap_allocated,0,sizeof(scrap_allocated));
	memset(&scrap_texels,255,sizeof(scrap_texels));
	memset(&gEffectTexture,0,sizeof(gEffectTexture));

	// First slot is reserved.
	gEffectTexture[0] = notexture;

	Scrap_Upload (); //creates 2 empty gltextures

	// reload wad pics
	Draw_LoadPics();

	// empty lmp cache
	for (pic = menu_cachepics, i = 0; i < menu_numcachepics; pic++, i++)
		pic->name[0] = 0;
	menu_numcachepics = 0;
}

void Draw_PrecacheConsoleBackground(void)
{
	Client_PrecacheResource(RESOURCE_TEXTURE,cvConsoleBackground.string);
}

void Draw_Init (void)
{
	Cvar_RegisterVariable(&cvConsoleAlpha,NULL);
	Cvar_RegisterVariable(&cvConsoleBackground,Draw_PrecacheConsoleBackground);

	// clear scrap and allocate gltextures
	memset(&scrap_allocated, 0, sizeof(scrap_allocated));
	memset(&scrap_texels, 255, sizeof(scrap_texels));
	Scrap_Upload (); //creates 2 empty textures

	// load game pics
	Draw_LoadPics();
}

//==============================================================================
//
//  2D DRAWING
//
//==============================================================================

void Draw_Character(int x,int y,int num)
{
	int		row, col;
	float	frow,fcol,size;

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

	{
		VideoObject_t	voCharacter[4] = { 0 };

		Video_ResetCapabilities(false);

		Video_EnableCapabilities(VIDEO_ALPHA_TEST);
		Video_DisableCapabilities(VIDEO_DEPTH_TEST);

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

		Video_SetTexture(gCharTexture);
		Video_DrawFill(voCharacter,NULL);

		Video_ResetCapabilities(true);
	}
}

void Draw_Pic(int x,int y,qpic_t *pic)
{
#if 0
	glpic_t	*gl;

	if(scrap_dirty)
		Scrap_Upload ();

	gl = (glpic_t*)pic->data;

	Video_ResetCapabilities(false);

	Video_SetTexture(gl->gltexture);

    // [1/4/2014] Use new rendering system ~hogsy
	{
        VideoObject_t voPicture	[]=
		{
			{	{	x,		        y,		        0	},	{	{	0,		0		}	},	{	1.0f,	1.0f,	1.0f,	1.0f	}	},
			{	{	x+pic->width,	y,		        0	},	{	{	1.0f,	0		}	},	{	1.0f,	1.0f,	1.0f,	1.0f	}	},
			{	{	x+pic->width,   y+pic->height,	0	},	{	{	1.0f,	1.0f	}	},	{	1.0f,	1.0f,	1.0f,	1.0f	}	},
			{	{	x,		        y+pic->height,  0	},	{	{	0,		1.0f	}	},	{	1.0f,	1.0f,	1.0f,	1.0f	}	}
		};

        Video_DrawFill(voPicture,NULL);
    }

	Video_ResetCapabilities(true);
#endif
}

void Draw_ConsoleBackground(void)
{
	float fAlpha = 1.0f;

	GL_SetCanvas(CANVAS_CONSOLE);

	if(cls.state == ca_connected)
		fAlpha = cvConsoleAlpha.value;

	// [27/5/2013] Simplified this check ~hogsy
	if(!cvConsoleBackground.string[0])
		Draw_Fill(0,0,vid.conwidth,vid.conheight,0,0,0,fAlpha);
	else
		Draw_ExternPic(cvConsoleBackground.string,fAlpha,0,0,vid.conwidth,vid.conheight);
}

/*	This repeats a 64*64 tile graphic to fill the screen around a sized down
	refresh window.
*/
void Draw_TileClear (int x, int y, int w, int h)
{
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
}

void Draw_Fill(int x,int y,int w,int h,float r,float g,float b,float alpha)
{
	vec4_t			vColour;
	VideoObject_t	voFill[4] = { 0 };

	vColour[0] = r; vColour[1] = g; vColour[2] = b; vColour[3] = alpha;
	Math_Vector4Copy(vColour,voFill[0].vColour);
	Math_Vector4Copy(vColour,voFill[1].vColour);
	Math_Vector4Copy(vColour,voFill[2].vColour);
	Math_Vector4Copy(vColour,voFill[3].vColour);

    Video_ResetCapabilities(false);

    Video_EnableCapabilities(VIDEO_BLEND);
    Video_DisableCapabilities(VIDEO_DEPTH_TEST|VIDEO_TEXTURE_2D);

	Video_ObjectVertex(&voFill[0], x, y, 0);

	Video_ObjectVertex(&voFill[1], x+w, y, 0);

	Video_ObjectVertex(&voFill[2], x+w, y+h, 0);

	Video_ObjectVertex(&voFill[3], x, y+h, 0);

	Video_DrawFill(voFill,NULL);

	Video_ResetCapabilities(true);
}

void Draw_FadeScreen (void)
{
	VideoObject_t	voFade[4] = { 0 };

	GL_SetCanvas(CANVAS_DEFAULT);

	Video_ResetCapabilities(false);

	Video_ObjectVertex(&voFade[0], 0, 0, 0);
	Video_ObjectColour(&voFade[0], 1.0f, 1.0f, 1.0f, 0.5f);

	Video_ObjectVertex(&voFade[1], glwidth, 0, 0);
	Video_ObjectColour(&voFade[1], 1.0f, 1.0f, 1.0f, 0.5f);

	Video_ObjectVertex(&voFade[2], glwidth, glheight, 0);
	Video_ObjectColour(&voFade[2], 1.0f, 1.0f, 1.0f, 0.5f);

	Video_ObjectVertex(&voFade[3], 0, glheight, 0);
	Video_ObjectColour(&voFade[3], 1.0f, 1.0f, 1.0f, 0.5f);

	Video_DrawFill(voFade,NULL);

	Video_ResetCapabilities(true);
}

/*	Draws the little blue disc in the corner of the screen.
	Call before beginning any disc IO.
*/
void Draw_BeginDisc(void)
{
	int			iViewport[4]; //johnfitz
	canvastype	oldcanvas; //johnfitz

	//johnfitz -- canvas and matrix stuff
	glGetIntegerv(GL_VIEWPORT,iViewport);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	oldcanvas = (canvastype)currentcanvas;
	GL_SetCanvas (CANVAS_TOPRIGHT);
	currentcanvas = oldcanvas; // a bit of a hack, since GL_SetCanvas doesn't know we are going to pop the stack
	//johnfitz

	glDrawBuffer(GL_FRONT);
	Draw_ExternPic("textures/sprites/disc",1.0f,320-32,0,32,32);
	glDrawBuffer(GL_BACK);

	//johnfitz -- restore everything so that 3d rendering isn't fucked up
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glViewport(iViewport[0],iViewport[1],iViewport[2],iViewport[3]);
	//johnfitz
}

void GL_SetCanvas (int newcanvas)
{
	extern vrect_t scr_vrect;
	float s;
	int lines;

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
		s = Math_Min((float)glwidth / 320.0, (float)glheight / 200.0);
		s = Math_Clamp(1.0, scr_menuscale.value, s);
		glOrtho (0, 320, 200, 0, -99999, 99999);
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
}

void Draw_ResetCanvas(void)
{
	currentcanvas = (canvastype)-1;

	GL_SetCanvas(CANVAS_DEFAULT);

	glColor4f(1.0f,1.0f,1.0f,1.0f);
}
