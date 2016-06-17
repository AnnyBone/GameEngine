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
#include "client/video_camera.h"
#include "video_shadow.h"

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

using namespace Core;

void Draw::SetDefaultState()
{
	plColour_t clear = { 0, 0, 0, 1 };
	vlSetClearColour4fv(clear);

	vlSetCullMode(VL_CULL_NEGATIVE);
#if defined (VL_MODE_OPENGL)
	glAlphaFunc(GL_GREATER, 0.5f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glDepthRange(0, 1);
	glDepthFunc(GL_LEQUAL);
	glClearStencil(1);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Overbrights.
	vlActiveTexture(VIDEO_TEXTURE_LIGHT);
	vlSetTextureEnvironmentMode(VIDEO_TEXTUREMODE_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 4);
#endif
	vlActiveTexture(0);

	// This is always active, since our viewports need it.
	vlEnable(VL_CAPABILITY_SCISSOR_TEST);
}

void Draw::ClearBuffers()
{
	if (!cv_video_clearbuffers.bValue)
		return;

	vlClearBuffers(VL_MASK_DEPTH | VL_MASK_COLOUR | VL_MASK_STENCIL);
}

void Draw::DepthBuffer()
{
#if 0
	static gltexture_t	*depth_texture = NULL;
	float				*uByte;

	if (!cv_video_drawdepth.bValue)
		return;

	// Allocate the pixel data.
	uByte = (float*)malloc(Video.iWidth*Video.iHeight*sizeof(float));
	if (!uByte)
		return;

	// Read le pixels, and copy them to uByte.
	glReadPixels(0, 0, Video.iWidth, Video.iHeight, GL_DEPTH_COMPONENT, GL_FLOAT, uByte);

	// Create our depth texture.
	depth_texture = TexMgr_NewTexture();

	// Set the texture.
	Video_SetTexture(depth_texture);

	// Copy it to the texture.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, Video.iWidth, Video.iHeight, 0, GL_LUMINANCE, GL_FLOAT, uByte);

	// Draw the buffer to the bottom left corner of the screen.
	GL_SetCanvas(CANVAS_BOTTOMLEFT);
	Draw_Rectangle(0, 0, 512, 512, pl_white);
	GL_SetCanvas(CANVAS_DEFAULT);

	// Delete the texture, so we can recreate it later.
	TexMgr_FreeTexture(depth_texture);

	// Free the pixel data.
	free(uByte);
#endif
}

////////////////////////////////////////////////////////////////////////////////

void Draw::WireBox(plVector3f_t mins, plVector3f_t maxs, float r, float g, float b)
{
	// todo, rewrite this function...

#if defined (VL_MODE_OPENGL)
	glBegin(GL_QUADS);
	glVertex3f(mins[0], mins[1], maxs[2]);
	glVertex3f(maxs[0], mins[1], maxs[2]);
	glVertex3fv(maxs);
	glVertex3f(mins[0], maxs[1], maxs[2]);
	glVertex3fv(mins);
	glVertex3f(mins[0], maxs[1], mins[2]);
	glVertex3f(maxs[0], maxs[1], mins[2]);
	glVertex3f(maxs[0], mins[1], mins[2]);
	glVertex3f(mins[0], maxs[1], mins[2]);
	glVertex3f(mins[0], maxs[1], maxs[2]);
	glVertex3fv(maxs);
	glVertex3f(maxs[0], maxs[1], mins[2]);
	glVertex3fv(mins);
	glVertex3f(maxs[0], mins[1], mins[2]);
	glVertex3f(maxs[0], mins[1], maxs[2]);
	glVertex3f(mins[0], mins[1], maxs[2]);
	glVertex3f(maxs[0], mins[1], mins[2]);
	glVertex3f(maxs[0], maxs[1], mins[2]);
	glVertex3fv(maxs);
	glVertex3f(maxs[0], mins[1], maxs[2]);
	glVertex3fv(mins);
	glVertex3f(mins[0], mins[1], maxs[2]);
	glVertex3f(mins[0], maxs[1], maxs[2]);
	glVertex3f(mins[0], maxs[1], mins[2]);
	glEnd();

	glColor4f(r, g, b, 1.0f);

	glBegin(GL_LINES);
	glVertex3fv(mins);
	glVertex3f(maxs[0], mins[1], mins[2]);
	glVertex3fv(mins);
	glVertex3f(mins[0], maxs[1], mins[2]);
	glVertex3f(maxs[0], maxs[1], mins[2]);
	glVertex3f(maxs[0], mins[1], mins[2]);
	glVertex3f(maxs[0], maxs[1], mins[2]);
	glVertex3f(mins[0], maxs[1], mins[2]);
	glVertex3f(mins[0], mins[1], maxs[2]);
	glVertex3f(maxs[0], mins[1], maxs[2]);
	glVertex3f(mins[0], mins[1], maxs[2]);
	glVertex3f(mins[0], maxs[1], maxs[2]);
	glVertex3fv(maxs);
	glVertex3f(maxs[0], mins[1], maxs[2]);
	glVertex3f(maxs[0], maxs[1], maxs[2]);
	glVertex3f(mins[0], maxs[1], maxs[2]);
	glVertex3fv(mins);
	glVertex3f(mins[0], mins[1], maxs[2]);
	glVertex3f(maxs[0], maxs[1], mins[2]);
	glVertex3fv(maxs);
	glVertex3f(mins[0], maxs[1], mins[2]);
	glVertex3f(mins[0], maxs[1], maxs[2]);
	glVertex3f(maxs[0], mins[1], mins[2]);
	glVertex3f(maxs[0], mins[1], maxs[2]);
	glEnd();

	glColor3f(1, 1, 1);
#endif
}

extern "C" ConsoleVariable_t r_showbboxes;
extern "C" ServerEntity_t *sv_player;

void Draw::BoundingBoxes()
{
	if (!r_showbboxes.value || (cl.maxclients > 1) || !r_drawentities.value || (!sv.active && !g_state.embedded))
		return;

	vlDisable(VL_CAPABILITY_DEPTH_TEST | VL_CAPABILITY_TEXTURE_2D);
	vlEnable(VL_CAPABILITY_BLEND);

	unsigned int i;
	ServerEntity_t *ed;
	for (i = 0, ed = NEXT_EDICT(sv.edicts); i < sv.num_edicts; i++, ed = NEXT_EDICT(ed))
	{
		if (ed == sv_player && !chase_active.value)
			continue;

#ifdef VL_MODE_OPENGL
		glColor3f(1, 1, 1);
#endif

		Draw::CoordinateAxes(ed->v.origin);

		plVector3f_t mins, maxs;
		Math_VectorAdd(ed->v.mins, ed->v.origin, mins);
		Math_VectorAdd(ed->v.maxs, ed->v.origin, maxs);

#ifdef VL_MODE_OPENGL
		glColor4f(0, 0.5f, 0, 0.5f);
#endif

		Draw::WireBox(mins, maxs, 1, 1, 1);
	}

	vlDisable(VL_CAPABILITY_BLEND);
	vlEnable(VL_CAPABILITY_TEXTURE_2D | VL_CAPABILITY_DEPTH_TEST);
}

void Draw::Shadows()
{
	if (!r_shadows.value		|| 
		!r_drawentities.value	|| 
		!cl_numvisedicts		||
		r_drawflat_cheatsafe	|| 
		r_lightmap_cheatsafe)
		return;

	for (unsigned int i = 0; i < cl_numvisedicts; i++)
	{
		currententity = cl_visedicts[i];
		if (currententity == &cl.viewent)
			return;

		Shadow_Draw(currententity);
	}

	// Allow us to also render the players own shadow too.
	if (cv_video_drawplayershadow.bValue)
		Shadow_Draw(&cl_entities[cl.viewentity]);
}

void Draw::Entities(bool alphapass)
{
	if (!r_drawentities.value)
		return;

#if 1
	for (unsigned int i = 0; i < cl_numvisedicts; i++)
	{
		//johnfitz -- if alphapass is true, draw only alpha entites this time
		//if alphapass is false, draw only nonalpha entities this time
		if ((ENTALPHA_DECODE(cl_visedicts[i]->alpha) < 1 && !alphapass) ||
			(ENTALPHA_DECODE(cl_visedicts[i]->alpha) == 1 && alphapass))
			continue;

		currententity = cl_visedicts[i];	// todo, legacy, needs to go!
		Draw::Entity(cl_visedicts[i]);
	}
#else	// Draw per-material
	for (int i = 0; i < material_count; i++)
	{
		for (unsigned int j = 0; j < cl_numvisedicts; j++)
		{
			currententity = cl_visedicts[j];
			if (!currententity->model)
				continue;

			if (currententity->model->materials == &g_materials[i])
			{
				//johnfitz -- if alphapass is true, draw only alpha entites this time
				//if alphapass is false, draw only nonalpha entities this time
				if ((ENTALPHA_DECODE(currententity->alpha) < 1 && !bAlphaPass) ||
					(ENTALPHA_DECODE(currententity->alpha) == 1 && bAlphaPass))
					continue;

				Draw::Entity(currententity);
			}
		}
	}
#endif
}

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
	vlVertex_t voSurface[4];

	// Sloppy, but in the case that there's nothing valid...
	if (!mMaterial)
	{
		Draw_Rectangle(x, y, w, h, pl_red);
		Draw_String(x, y, "INVALID MATERIAL!");
		return;
	}

	// Disable depth testing.
	vlDisable(VL_CAPABILITY_DEPTH_TEST);

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

	vlEnable(VL_CAPABILITY_DEPTH_TEST);
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
	qpic_t *dat = (qpic_t*)COM_LoadHeapFile(path);
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

extern "C" void Draw_NewGame(void);

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
	vlVertex_t		voCharacter[4] = { { { 0 } } };
	int				row, col;
	float			frow,fcol,size;

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

	vlDisable(VL_CAPABILITY_DEPTH_TEST);

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

	vlEnable(VL_CAPABILITY_DEPTH_TEST);
}

/*	Draws a simple string of text on the screen.
*/
void Draw_String(int x, int y, const char *msg)
{
	if (y <= -8)
		return;

	Material_Draw(g_mGlobalColour, NULL, VL_PRIMITIVE_IGNORE, 0, false);

	while (*msg)
	{
		Draw_Character(x, y, *msg);
		msg++;
		x += 8;
	}

	Material_Draw(g_mGlobalColour, NULL, VL_PRIMITIVE_IGNORE, 0, true);
}

void Draw_ConsoleBackground(void)
{
	float		alpha = cvConsoleAlpha.value;
	plColour_t	black, lightblack;

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
	Camera *camera = g_cameramanager->GetCurrentCamera();
	if (!camera)
		return;

	Viewport *viewport = dynamic_cast<Viewport*>(camera->GetViewport());
	if (!viewport)
		return;

#if defined (VL_MODE_OPENGL)
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
#endif

	Draw_ResetCanvas();

	VideoCanvasType_t vctOldCanvas = (VideoCanvasType_t)currentcanvas;
	GL_SetCanvas(CANVAS_DEFAULT);
	currentcanvas = vctOldCanvas;

	plColour_t
		cTop	= { 0.1f, 0.1f, 0.1f, 1.0f },
		cBottom = { 0.5f, 0.5f, 0.5f, 1.0f };
	Draw_GradientFill(0, 0, viewport->GetWidth(), viewport->GetHeight(), cTop, cBottom);

#if defined (VL_MODE_OPENGL)
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
#endif

	vlViewport(0, 0, viewport->GetWidth(), viewport->GetHeight());
}

void Draw_Line(plVector3f_t mvStart, plVector3f_t mvEnd)
{
	vlVertex_t voLine[2] = { { { 0 } } };

	Video_ObjectVertex(&voLine[0], mvStart[0], mvStart[1], mvStart[2]);
	Video_ObjectColour(&voLine[0], 1.0f, 0, 0, 1.0f);
	Video_ObjectVertex(&voLine[1], mvEnd[0], mvEnd[1], mvEnd[2]);
	Video_ObjectColour(&voLine[1], 1.0f, 0, 0, 1.0f);

	Video_DrawObject(voLine, VL_PRIMITIVE_LINES, 2, NULL, 0);
}

void Draw::CoordinateAxes(plVector3f_t position)
{
	plVector3f_t start, end;
	plVectorCopy(position, start);
	plVectorCopy(position, end);
	start[0] += 10;
	end[0] -= 10;
	Draw_Line(start, end);

	plVectorCopy(position, start);
	plVectorCopy(position, end);
	start[1] += 10;
	end[1] -= 10;
	Draw_Line(start, end);

	plVectorCopy(position, start);
	plVectorCopy(position, end);
	start[2] += 10;
	end[2] -= 10;
	Draw_Line(start, end);
}

void Draw_Grid(float x, float y, float z, int grid_size)
{
	vlEnable(VL_CAPABILITY_BLEND);
	vlDisable(VL_CAPABILITY_TEXTURE_2D);

	vlBlendFunc(VL_BLEND_DEFAULT);

#ifdef VL_MODE_OPENGL
	glPushMatrix();

	glTranslatef(x, y, z);

	glEnable(GL_LINE_SMOOTH);

	glColor4f(0, 0, 0.5f, 1);

	glLineWidth(0.1f);
	glBegin(GL_LINES);

	int i;
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

	glColor4f(0, 0, 1, 1);
	glLineWidth(1.0f);
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

	glDisable(GL_LINE_SMOOTH);

	glPopMatrix();
#endif

	vlDisable(VL_CAPABILITY_BLEND);
	vlEnable(VL_CAPABILITY_TEXTURE_2D);
}

void Draw_Rectangle(int x, int y, int w, int h, plColour_t colour)
{
	vlVertex_t	voFill[4];

	Math_Vector4Copy(colour, voFill[0].colour);
	Math_Vector4Copy(colour, voFill[1].colour);
	Math_Vector4Copy(colour, voFill[2].colour);
	Math_Vector4Copy(colour, voFill[3].colour);

	if (colour[3] < 1)
		vlEnable(VL_CAPABILITY_BLEND);
	vlDisable(VL_CAPABILITY_DEPTH_TEST | VL_CAPABILITY_TEXTURE_2D);

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
		vlDisable(VL_CAPABILITY_BLEND);
	vlEnable(VL_CAPABILITY_DEPTH_TEST | VL_CAPABILITY_TEXTURE_2D);
}

void Draw_GradientFill(int x, int y, int w, int h, plColour_t mvTopColour, plColour_t mvBottomColour)
{
	vlVertex_t	voFill[4];

	if ((mvTopColour[3] < 1) || (mvBottomColour[3] < 1))
		vlEnable(VL_CAPABILITY_BLEND);
	vlDisable(VL_CAPABILITY_DEPTH_TEST | VL_CAPABILITY_TEXTURE_2D);

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
		vlDisable(VL_CAPABILITY_BLEND);
	vlEnable(VL_CAPABILITY_DEPTH_TEST | VL_CAPABILITY_TEXTURE_2D);
}

void Draw_FadeScreen (void)
{
	vlVertex_t	voFade[4];

	Viewport *viewport = GetPrimaryViewport();
	if (!viewport)
		return;

	GL_SetCanvas(CANVAS_DEFAULT);

	vlEnable(VL_CAPABILITY_BLEND);

	Video_ObjectVertex(&voFade[0], 0, 0, 0);
	Video_ObjectColour(&voFade[0], 1.0f, 1.0f, 1.0f, 0.5f);

	Video_ObjectVertex(&voFade[1], viewport->GetWidth(), 0, 0);
	Video_ObjectColour(&voFade[1], 1.0f, 1.0f, 1.0f, 0.5f);

	Video_ObjectVertex(&voFade[2], viewport->GetWidth(), viewport->GetHeight(), 0);
	Video_ObjectColour(&voFade[2], 1.0f, 1.0f, 1.0f, 0.5f);

	Video_ObjectVertex(&voFade[3], 0, viewport->GetHeight(), 0);
	Video_ObjectColour(&voFade[3], 1.0f, 1.0f, 1.0f, 0.5f);

	Video_DrawFill(voFade, NULL, 0);

	vlDisable(VL_CAPABILITY_BLEND);
}

/*	Draws the little blue disc in the corner of the screen.
	Call before beginning any disc IO.
*/
void Draw_BeginDisc(void)
{
#ifdef VL_MODE_OPENGL
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
	vlViewport(iViewport[0], iViewport[1], iViewport[2], iViewport[3]);
	//johnfitz
#endif
}

void GL_SetCanvas (VideoCanvasType_t newcanvas)
{
#ifdef VL_MODE_OPENGL
	float s;
	int	lines;

	Camera *camera = g_cameramanager->GetCurrentCamera();
	if (!camera)
		return;

	Viewport *viewport = dynamic_cast<Viewport*>(camera->GetViewport());
	if (!viewport)
		return;

	if (newcanvas == currentcanvas)
		return;
	currentcanvas = newcanvas;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();

	unsigned int glwidth = viewport->GetWidth(), glheight = viewport->GetHeight();
	int glx = viewport->GetPosition()[0], gly = viewport->GetPosition()[1];
	switch(newcanvas)
	{
	case CANVAS_DEFAULT:
		glOrtho (0, glwidth, glheight, 0, -99999, 99999);
		vlViewport(glx, gly, glwidth, glheight);
		break;
	case CANVAS_CONSOLE:
		lines = vid.conheight - (scr_con_current * vid.conheight / glheight);
		glOrtho(0,vid.conwidth,vid.conheight + lines,lines,-99999,99999);
		vlViewport(glx, gly, glwidth, glheight);
		break;
	case CANVAS_MENU:
		s = Math_Min((float)glwidth / 640.0, (float)glheight / 480.0);
		s = Math_Clamp(1.0, scr_menuscale.value, s);
		glOrtho (0, 640, 480, 0, -99999, 99999);
		vlViewport(glx, gly, glwidth, glheight);
		break;
	case CANVAS_SBAR:
		s = Math_Clamp(1.0, scr_sbarscale.value, (float)glwidth / 320.0);
		glOrtho (0, 320, 48, 0, -99999, 99999);
		vlViewport(glx + (glwidth - 320 * s) / 2, gly, 320 * s, 48 * s);
		break;
	case CANVAS_WARPIMAGE:
		glOrtho (0, 128, 0, 128, -99999, 99999);
		vlViewport(glx, gly + glheight - gl_warpimagesize, gl_warpimagesize, gl_warpimagesize);
		break;
	case CANVAS_CROSSHAIR: //0,0 is center of viewport
		s = Math_Clamp(1.0, scr_crosshairscale.value, 10.0);
		glOrtho(glwidth / -2 / s, glwidth / 2 / s, glheight / 2 / s, glheight / -2 / s, -99999, 99999);
		vlViewport(glx, glheight - gly - glheight, glwidth & ~1, glheight & ~1);
		break;
	case CANVAS_BOTTOMLEFT: //used by devstats
		s = (float)glwidth/vid.conwidth; //use console scale
		glOrtho (0, 320, 200, 0, -99999, 99999);
		vlViewport(glx, gly, 320 * s, 200 * s);
		break;
	case CANVAS_BOTTOMRIGHT: //used by fps/clock
		s = (float)glwidth/vid.conwidth; //use console scale
		glOrtho (0, 320, 200, 0, -99999, 99999);
		vlViewport(glx + glwidth - 320 * s, gly, 320 * s, 200 * s);
		break;
	case CANVAS_TOPRIGHT: //used by disc
		s = 1;
		glOrtho (0, 320, 200, 0, -99999, 99999);
		vlViewport(glx + glwidth - 320 * s, gly + glheight - 200 * s, 320 * s, 200 * s);
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

void Draw::EntityBoundingBox(ClientEntity_t *entity)
{
	if (!entity->model || ((entity == &cl_entities[cl.viewentity]) && !chase_active.bValue) || (entity == &cl.viewent))
		return;

	plVector3f_t mins, maxs;
	switch (entity->model->type)
	{
	case MODEL_TYPE_LEVEL:
		// Only draw wires for the BSP, since otherwise it's difficult to see anything else.
#ifdef VL_MODE_OPENGL
		glColor4f(0, 0, 0, 0);
#endif

		Math_VectorAdd(entity->model->mins, entity->origin, mins);
		Math_VectorAdd(entity->model->maxs, entity->origin, maxs);
		Draw::WireBox(mins, maxs, 0, 1, 0);
		break;
	default:
	{
#ifdef VL_MODE_OPENGL
		glColor4f(0.5f, 0, 0, 0.5f);
#endif
		Math_VectorAdd(entity->model->rmins, entity->origin, mins);
		Math_VectorAdd(entity->model->rmaxs, entity->origin, maxs);
		Draw::WireBox(mins, maxs, 1, 0, 0);

		Math_VectorAdd(entity->model->ymins, entity->origin, mins);
		Math_VectorAdd(entity->model->ymaxs, entity->origin, maxs);
		Draw::WireBox(mins, maxs, 0, 1, 0);

		Math_VectorAdd(entity->model->mins, entity->origin, mins);
		Math_VectorAdd(entity->model->maxs, entity->origin, maxs);
		Draw::WireBox(mins, maxs, 0, 0, 1);
	}
	break;
	}
}

void Draw::Entity(ClientEntity_t *entity)
{
	if (!entity->model)
	{
		Draw::CoordinateAxes(entity->origin);
		return;
	}

	entity->distance_alpha = 1.0f;
	if (cv_video_entity_fade.bValue)
	{
		plVector3f_t vdist;
		Core::Camera *camera = g_cameramanager->GetCurrentCamera();
		plVectorSubtract3fv(&camera->GetPosition()[0], entity->origin, vdist);
		float distance = plLengthf(vdist);
		if (distance > cv_video_entity_distance.value)
		{
			entity->distance_alpha = 1.0f - (((distance - cv_video_entity_distance.value) / 100.0f) / 1.0f);
			if (entity->distance_alpha < 0.01)
				return;
		}
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

	if (r_showbboxes.bValue)
	{
		vlEnable(VL_CAPABILITY_BLEND);
		Draw::EntityBoundingBox(entity);
		vlDisable(VL_CAPABILITY_BLEND);
	}
}
