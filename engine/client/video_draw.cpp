/*
Copyright (C) 2011-2016 OldTimes Software

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

#include "video.h"
#include "shadow.h"

#define    BLOCK_WIDTH        256
#define    BLOCK_HEIGHT    BLOCK_WIDTH

ConsoleVariable_t cvConsoleAlpha = {"screen_consolealpha", "0.7", true, false,
                                    "Sets the alpha value for the console background."}; //johnfitz

typedef struct {
    gltexture_t *gltexture;
    float sl, tl, sh, th;
} glpic_t;

VideoCanvasType_t currentcanvas = CANVAS_NONE; //johnfitz -- for GL_SetCanvas

using namespace core;

void draw::SetDefaultState() {
    glClearColor(0, 0, 0, 1.0f);

    plSetCullMode(VL_CULL_NEGATIVE);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glAlphaFunc(GL_GREATER, 0.5f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glDepthRange(0, 1);
    glDepthFunc(GL_LEQUAL);
    glClearStencil(1);

    // Overbrights.
    plSetTextureUnit(VIDEO_TEXTURE_LIGHT);
    plSetTextureEnvironmentMode(PL_TEXTUREMODE_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 4);

    plSetTextureUnit(0);
    plSetTextureEnvironmentMode(PL_TEXTUREMODE_MODULATE);

    glEnable(GL_SCISSOR_TEST);
}

void draw::ClearBuffers() {
    if (!cv_video_clearbuffers.bValue) {
        return;
    }

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void draw::DepthBuffer() {
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

void draw::WireBox(plVector3f_t mins, plVector3f_t maxs, float r, float g, float b) {
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

void draw::BoundingBoxes() {
    if (!r_showbboxes.value || (cl.maxclients > 1) || !r_drawentities.value || (!sv.active && !g_state.embedded))
        return;

    plDisableGraphicsStates(VL_CAPABILITY_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);

    unsigned int i;
    ServerEntity_t *ed;
    for (i = 0, ed = SERVER_ENTITY_NEXT(sv.edicts); i < sv.num_edicts; i++, ed = SERVER_ENTITY_NEXT(ed)) {
        if (ed == sv_player && !chase_active.value)
            continue;

        glColor3f(1, 1, 1);

        draw::CoordinateAxes(ed->v.origin);

        plVector3f_t mins = {0}, maxs = {0};
        Math_VectorAdd(ed->v.mins, ed->v.origin, mins);
        Math_VectorAdd(ed->v.maxs, ed->v.origin, maxs);

        glColor4f(0, 0.5f, 0, 0.5f);

        draw::WireBox(mins, maxs, 1, 1, 1);
    }

    glDisable(GL_BLEND);

    plEnableGraphicsStates(VL_CAPABILITY_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
}

void draw::Shadows() {
    if (!r_shadows.value ||
        !r_drawentities.value ||
        !cl_numvisedicts ||
        r_drawflat_cheatsafe ||
        r_lightmap_cheatsafe)
        return;

    for (unsigned int i = 0; i < cl_numvisedicts; i++) {
        currententity = cl_visedicts[i];
        if (currententity == &cl.viewent)
            return;

        Shadow_Draw(currententity);
    }

    // Allow us to also render the players own shadow too.
    if (cv_video_drawplayershadow.bValue)
        Shadow_Draw(&cl_entities[cl.viewentity]);
}

void draw::Entities(bool alphapass) {
    if (!r_drawentities.value)
        return;

#if 1
    for (unsigned int i = 0; i < cl_numvisedicts; i++) {
        //johnfitz -- if alphapass is true, draw only alpha entites this time
        //if alphapass is false, draw only nonalpha entities this time
        if ((ENTALPHA_DECODE(cl_visedicts[i]->alpha) < 1 && !alphapass) ||
            (ENTALPHA_DECODE(cl_visedicts[i]->alpha) == 1 && alphapass))
            continue;

        currententity = cl_visedicts[i];    // todo, legacy, needs to go!
        draw::Entity(cl_visedicts[i]);
    }
#else	// Draw per-material
    for (int i = 0; i < material_count; i++) {
        for (unsigned int j = 0; j < cl_numvisedicts; j++) {
            currententity = cl_visedicts[j];
            if (!currententity->model)
                continue;

            if (currententity->model->materials == &g_materials[i]) {
                //johnfitz -- if alphapass is true, draw only alpha entites this time
                //if alphapass is false, draw only nonalpha entities this time
                if ((ENTALPHA_DECODE(currententity->alpha) < 1 && !alphapass) ||
                    (ENTALPHA_DECODE(currententity->alpha) == 1 && alphapass))
                    continue;

                draw::Entity(currententity);
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

typedef struct cachepic_s {
    char name[MAX_QPATH];
    qpic_t pic;
    //uint8_t		padding[32];	// for appended glpic
} cachepic_t;

#define    MAX_CACHED_PICS        128
cachepic_t menu_cachepics[MAX_CACHED_PICS];
int menu_numcachepics;

//  scrap allocation
//  Allocate all the little status bar obejcts into a single texture
//  to crutch up stupid hardware / drivers

#define    MAX_SCRAPS        2

int scrap_allocated[MAX_SCRAPS][BLOCK_WIDTH];
uint8_t scrap_texels[MAX_SCRAPS][BLOCK_WIDTH * BLOCK_HEIGHT]; //johnfitz -- removed *4 after BLOCK_HEIGHT
bool scrap_dirty;
gltexture_t *scrap_textures[MAX_SCRAPS]; //johnfitz

#if 0
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
        scrap_textures[i] = TexMgr_LoadImage
                (
                        NULL,
                        name,
                        BLOCK_WIDTH, BLOCK_HEIGHT,
                        SRC_INDEXED,
                        scrap_texels[i],
                        "",
                        (unsigned int)scrap_texels[i],
                        TEXPREF_ALPHA | TEXPREF_OVERWRITE | TEXPREF_NOPICMIP
                );
    }

    scrap_dirty = false;
}
#endif

//==================================================================

void draw::MaterialSurface(Material_t *material, int x, int y, unsigned int w, unsigned int h, float alpha) {
    // Sloppy, but in the case that there's nothing valid...
    if (!material) {
        draw::Rectangle(x, y, w, h, pl_red);
        draw::String(x, y, "INVALID MATERIAL!");
        return;
    }

    // Disable depth testing.
    glDisable(GL_DEPTH_TEST);

    PLVertex vertices[4];

    // Set the colour.
    Video_ObjectColour(&vertices[0], 1.0f, 1.0f, 1.0f, alpha);
    Video_ObjectColour(&vertices[1], 1.0f, 1.0f, 1.0f, alpha);
    Video_ObjectColour(&vertices[2], 1.0f, 1.0f, 1.0f, alpha);
    Video_ObjectColour(&vertices[3], 1.0f, 1.0f, 1.0f, alpha);

    // Set the texture coords.
    Video_ObjectTexture(&vertices[0], 0, 0, 0);
    Video_ObjectTexture(&vertices[1], 0, 1.0f, 0);
    Video_ObjectTexture(&vertices[2], 0, 1.0f, 1.0f);
    Video_ObjectTexture(&vertices[3], 0, 0, 1.0f);

    // Set the vertex coords.
    Video_ObjectVertex(&vertices[0], x, y, 0);
    Video_ObjectVertex(&vertices[1], x + w, y, 0);
    Video_ObjectVertex(&vertices[2], x + w, y + h, 0);
    Video_ObjectVertex(&vertices[3], x, y + h, 0);

    // Throw it off to the rendering pipeline.
    Material_Draw(material, vertices, VL_PRIMITIVE_TRIANGLE_FAN, 4, false);
    Video_DrawFill(vertices, material, material->current_skin);
    Material_Draw(material, vertices, VL_PRIMITIVE_TRIANGLE_FAN, 4, true);

    glEnable(GL_DEPTH_TEST);
}

void Draw_MaterialSurface(Material_t *mMaterial, PLuint skin, int x, int y, PLuint w, PLuint h, float fAlpha) {
    Material_SetSkin(mMaterial, skin);

    draw::MaterialSurface(mMaterial, x, y, w, h, fAlpha);
}

//==============================================================================
//
//  INIT
//
//==============================================================================

extern "C" void Draw_NewGame(void);

void Draw_NewGame(void) {
    cachepic_t *pic;
    int i;

    // Empty scrap and reallocate gltextures
    memset(&scrap_allocated, 0, sizeof(scrap_allocated));
    memset(&scrap_texels, 255, sizeof(scrap_texels));

//	Scrap_Upload (); //creates 2 empty gltextures

    // empty lmp cache
    for (pic = menu_cachepics, i = 0; i < menu_numcachepics; pic++, i++)
        pic->name[0] = 0;
    menu_numcachepics = 0;
}

void Draw_Init(void) {
    Cvar_RegisterVariable(&cvConsoleAlpha, NULL);

    // clear scrap and allocate gltextures
    memset(&scrap_allocated, 0, sizeof(scrap_allocated));
    memset(&scrap_texels, 255, sizeof(scrap_texels));
//	Scrap_Upload (); //creates 2 empty textures
}

//==============================================================================
//
//  2D DRAWING
//
//==============================================================================

void draw::Character(int x, int y, int num) {
    PLVertex voCharacter[4] = {{{0}}};
    int row, col;
    float frow, fcol, size;

    if (y <= -8)
        return;            // totally off screen

    num &= 255;
    if (num == 32)
        return; //don't waste verts on spaces

    row = num >> 4;
    col = num & 15;

    frow = row * 0.0625f;
    fcol = col * 0.0625f;
    size = 0.0625f;

    Video_ObjectVertex(&voCharacter[0], x, y, 0);
    Video_ObjectColour(&voCharacter[0], 1.0f, 1.0f, 1.0f, 1.0f);
    Video_ObjectTexture(&voCharacter[0], VIDEO_TEXTURE_DIFFUSE, fcol, frow);

    Video_ObjectVertex(&voCharacter[1], x + 8, y, 0);
    Video_ObjectColour(&voCharacter[1], 1.0f, 1.0f, 1.0f, 1.0f);
    Video_ObjectTexture(&voCharacter[1], VIDEO_TEXTURE_DIFFUSE, fcol + size, frow);

    Video_ObjectVertex(&voCharacter[2], x + 8, y + 8, 0);
    Video_ObjectColour(&voCharacter[2], 1.0f, 1.0f, 1.0f, 1.0f);
    Video_ObjectTexture(&voCharacter[2], VIDEO_TEXTURE_DIFFUSE, fcol + size, frow + size);

    Video_ObjectVertex(&voCharacter[3], x, y + 8, 0);
    Video_ObjectColour(&voCharacter[3], 1.0f, 1.0f, 1.0f, 1.0f);
    Video_ObjectTexture(&voCharacter[3], VIDEO_TEXTURE_DIFFUSE, fcol, frow + size);

    Video_DrawFill(voCharacter, g_mGlobalConChars, 0);
}

void draw::String(int x, int y, const char *msg) {
    if (y <= -8)
        return;

    glDisable(GL_DEPTH_TEST);

    Material_Draw(g_mGlobalConChars, NULL, VL_PRIMITIVE_IGNORE, 0, false);

    while (*msg) {
        draw::Character(x, y, *msg);
        msg++;
        x += 8;
    }

    Material_Draw(g_mGlobalConChars, NULL, VL_PRIMITIVE_IGNORE, 0, true);

    glEnable(GL_DEPTH_TEST);
}

// Draws a simple string of text on the screen.
void Draw_String(int x, int y, const char *msg) {
    draw::String(x, y, msg);
}

void draw::GradientBackground(PLColour top, PLColour bottom) {
    Viewport *viewport = GetCurrentViewport();
    if (!viewport)
        return;

#if defined (VL_MODE_OPENGL)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
#endif

    draw::ResetCanvas();

    VideoCanvasType_t oldcanvas = (VideoCanvasType_t) currentcanvas;
    GL_SetCanvas(CANVAS_DEFAULT);
    currentcanvas = oldcanvas;

    draw::GradientFill(0, 0, viewport->GetWidth(), viewport->GetHeight(), top, bottom);

#if defined (VL_MODE_OPENGL)
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
#endif

    glViewport(0, 0, viewport->GetWidth(), viewport->GetHeight());
}

void draw::Line(plVector3f_t start, plVector3f_t end) {
    PLVertex line[2] = {{{0}}};
    Video_ObjectVertex(&line[0], start[0], start[1], start[2]);
    Video_ObjectColour(&line[0], 1.0f, 0, 0, 1.0f);
    Video_ObjectVertex(&line[1], end[0], end[1], end[2]);
    Video_ObjectColour(&line[1], 1.0f, 0, 0, 1.0f);

    Video_DrawObject(line, VL_PRIMITIVE_LINES, 2, NULL, 0);
}

void draw::CoordinateAxes(plVector3f_t position) {
    PLVector3f start = {0}, end = {0};
    plVectorCopy(position, start);
    plVectorCopy(position, end);
    start[0] += 10;
    end[0] -= 10;
    draw::Line(start, end);

    plVectorCopy(position, start);
    plVectorCopy(position, end);
    start[1] += 10;
    end[1] -= 10;
    draw::Line(start, end);

    plVectorCopy(position, start);
    plVectorCopy(position, end);
    start[2] += 10;
    end[2] -= 10;
    draw::Line(start, end);
}

void draw::Grid(plVector3f_t position, PLuint grid_size) {
    glEnable(GL_BLEND);
    plDisableGraphicsStates(VL_CAPABILITY_TEXTURE_2D);

    plSetBlendMode(PL_BLEND_DEFAULT);

    glPushMatrix();

    glTranslatef(position[0], position[1], position[2]);

    glEnable(GL_LINE_SMOOTH);

    glColor4f(0, 0, 0.5f, 1);

    glLineWidth(0.1f);
    glBegin(GL_LINES);

    for (PLuint i = 0; i <= (4096 / grid_size); i++) {
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

    for (PLuint i = 0; i <= 64; i++) {
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

    glDisable(GL_BLEND);
    plEnableGraphicsStates(VL_CAPABILITY_TEXTURE_2D);
}

void draw::Rectangle(PLint x, PLint y, PLuint w, PLuint h, PLColour colour) {
    PLVertex fill[4] = {0};

    Math_Vector4Copy(colour, fill[0].colour);
    Math_Vector4Copy(colour, fill[1].colour);
    Math_Vector4Copy(colour, fill[2].colour);
    Math_Vector4Copy(colour, fill[3].colour);

    if (colour[3] < 1) {
        glEnable(GL_BLEND);
    }

    glDisable(GL_DEPTH_TEST);
    plDisableGraphicsStates(VL_CAPABILITY_TEXTURE_2D);

    Video_ObjectVertex(&fill[0], x, y, 0);
    Video_ObjectTexture(&fill[0], 0, h, w);
    Video_ObjectVertex(&fill[1], x + w, y, 0);
    Video_ObjectTexture(&fill[1], 0, h + w, w);
    Video_ObjectVertex(&fill[2], x + w, y + h, 0);
    Video_ObjectTexture(&fill[2], 0, h + w, w + h);
    Video_ObjectVertex(&fill[3], x, y + h, 0);
    Video_ObjectTexture(&fill[0], 0, h, w + h);

    Video_DrawFill(fill, NULL, 0);

    if (colour[3] < 1) {
        glDisable(GL_BLEND);
    }

    glEnable(GL_DEPTH_TEST);
    plEnableGraphicsStates(VL_CAPABILITY_TEXTURE_2D);
}

// C wrapper for draw::rectangle
void Draw_Rectangle(int x, int y, PLuint w, PLuint h, PLColour colour) {
    draw::Rectangle(x, y, w, h, colour);
}

void draw::GradientFill(int x, int y, PLuint width, PLuint height, PLColour top, PLColour bottom) {
    PLVertex voFill[4];

    if ((top[3] < 1) || (bottom[3] < 1)) {
        glEnable(GL_BLEND);
    }

    glDisable(GL_DEPTH_TEST);
    plDisableGraphicsStates(VL_CAPABILITY_TEXTURE_2D);

    Video_ObjectVertex(&voFill[0], x, y, 0);
    Video_ObjectColour(&voFill[0], top[0], top[1], top[2], top[3]);
    Video_ObjectVertex(&voFill[1], x + width, y, 0);
    Video_ObjectColour(&voFill[1], top[0], top[1], top[2], top[3]);
    Video_ObjectVertex(&voFill[2], x + width, y + height, 0);
    Video_ObjectColour(&voFill[2], bottom[0], bottom[1], bottom[2], bottom[3]);
    Video_ObjectVertex(&voFill[3], x, y + height, 0);
    Video_ObjectColour(&voFill[3], bottom[0], bottom[1], bottom[2], bottom[3]);

    Video_DrawFill(voFill, NULL, 0);

    if ((top[3] < 1) || (bottom[3] < 1)) {
        glDisable(GL_BLEND);
    }

    glEnable(GL_DEPTH_TEST);
    plEnableGraphicsStates(VL_CAPABILITY_TEXTURE_2D);
}

void draw::ScreenFade() {
    PLVertex voFade[4];

    Viewport *viewport = GetCurrentViewport();
    if (!viewport) {
        return;
    }

    GL_SetCanvas(CANVAS_DEFAULT);

    glEnable(GL_BLEND);

    Video_ObjectVertex(&voFade[0], 0, 0, 0);
    Video_ObjectColour(&voFade[0], 1.0f, 1.0f, 1.0f, 0.5f);

    Video_ObjectVertex(&voFade[1], viewport->GetWidth(), 0, 0);
    Video_ObjectColour(&voFade[1], 1.0f, 1.0f, 1.0f, 0.5f);

    Video_ObjectVertex(&voFade[2], viewport->GetWidth(), viewport->GetHeight(), 0);
    Video_ObjectColour(&voFade[2], 1.0f, 1.0f, 1.0f, 0.5f);

    Video_ObjectVertex(&voFade[3], 0, viewport->GetHeight(), 0);
    Video_ObjectColour(&voFade[3], 1.0f, 1.0f, 1.0f, 0.5f);

    Video_DrawFill(voFade, NULL, 0);

    glDisable(GL_BLEND);
}

/*	Draws the little blue disc in the corner of the screen.
	Call before beginning any disc IO.
*/
void Draw_BeginDisc(void) {
#ifdef VL_MODE_OPENGL
    int iViewport[4]; //johnfitz
    VideoCanvasType_t oldcanvas; //johnfitz

    // Don't draw if it's dedicated.
    if (!g_video.initialized || (cls.state == ca_dedicated))
        return;

    //johnfitz -- canvas and matrix stuff
    glGetIntegerv(GL_VIEWPORT, iViewport);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    oldcanvas = (VideoCanvasType_t) currentcanvas;
    GL_SetCanvas(CANVAS_TOPRIGHT);
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
    glViewport(iViewport[0], iViewport[1], (PLuint) iViewport[2], (PLuint) iViewport[3]);
    //johnfitz
#endif
}

void GL_SetCanvas(VideoCanvasType_t newcanvas) {
#ifdef VL_MODE_OPENGL
    float s;
    int lines;

    Viewport *viewport = GetCurrentViewport();
    if (!viewport)
        return;

    if (newcanvas == currentcanvas)
        return;
    currentcanvas = newcanvas;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    switch (newcanvas) {
        case CANVAS_DEFAULT:
            glOrtho(0, viewport->GetWidth(), viewport->GetHeight(), 0, -99999, 99999);
            glViewport(
                    viewport->GetPosition()[0],
                    viewport->GetPosition()[1],
                    viewport->GetWidth(),
                    viewport->GetHeight());
            break;
        case CANVAS_CONSOLE:
            lines = vid.conheight - ((PLint) scr_con_current * vid.conheight / viewport->GetHeight());
            glOrtho(0, vid.conwidth, vid.conheight + lines, lines, -99999, 99999);
            glViewport(
                    viewport->GetPosition()[0],
                    viewport->GetPosition()[1],
                    viewport->GetWidth(),
                    viewport->GetHeight());
            break;
        case CANVAS_MENU:
            //s = std::fminf((float)viewport->GetWidth() / 640.0f, (float)viewport->GetHeight() / 480.0f);
            //s = plClamp(1.0f, scr_menuscale.value, s);
            glOrtho(0, 640, 480, 0, -99999, 99999);
            glViewport(
                    viewport->GetPosition()[0],
                    viewport->GetPosition()[1],
                    viewport->GetWidth(),
                    viewport->GetHeight());
            break;
        case CANVAS_SBAR:
            s = plClamp(1.0f, scr_sbarscale.value, (PLfloat) viewport->GetWidth() / 320.0f);
            glOrtho(0, 320, 48, 0, -99999, 99999);
            glViewport(
                    viewport->GetPosition()[0] + (viewport->GetWidth() - 320 * (PLint) s) / 2,
                    viewport->GetPosition()[1],
                    320 * (PLuint) s,
                    48 * (PLuint) s);
            break;
        case CANVAS_WARPIMAGE:
            glOrtho(0, 128, 0, 128, -99999, 99999);
            glViewport(
                    viewport->GetPosition()[0],
                    viewport->GetPosition()[1] + viewport->GetHeight() - gl_warpimagesize,
                    (PLuint) gl_warpimagesize,
                    (PLuint) gl_warpimagesize);
            break;
        case CANVAS_CROSSHAIR: //0,0 is center of viewport
            s = plClamp(1.0f, scr_crosshairscale.value, 10.0f);
            glOrtho(viewport->GetWidth() / -2 / s, viewport->GetWidth() / 2 / s, viewport->GetHeight() / 2 / s,
                    viewport->GetHeight() / -2 / s, -99999, 99999);
            glViewport(
                    viewport->GetPosition()[0],
                    viewport->GetHeight() - viewport->GetPosition()[1] - viewport->GetHeight(),
                    viewport->GetWidth() & ~1,
                    viewport->GetHeight() & ~1);
            break;
        case CANVAS_BOTTOMLEFT: //used by devstats
            s = (float) viewport->GetWidth() / vid.conwidth; //use console scale
            glOrtho(0, 320, 200, 0, -99999, 99999);
            glViewport(
                    viewport->GetPosition()[0],
                    viewport->GetPosition()[1],
                    320 * (PLuint) s,
                    200 * (PLuint) s);
            break;
        case CANVAS_BOTTOMRIGHT: //used by fps/clock
            s = (float) viewport->GetWidth() / vid.conwidth; //use console scale
            glOrtho(0, 320, 200, 0, -99999, 99999);
            glViewport(
                    viewport->GetPosition()[0] + viewport->GetWidth() - 320 * (PLint) s,
                    viewport->GetPosition()[1],
                    320 * (PLuint) s,
                    200 * (PLuint) s);
            break;
        case CANVAS_TOPRIGHT: //used by disc
            s = 1;
            glOrtho(0, 320, 200, 0, -99999, 99999);
            glViewport(
                    viewport->GetPosition()[0] + viewport->GetWidth() - 320 * (PLint) s,
                    viewport->GetPosition()[1] + viewport->GetHeight() - 200 * (PLint) s,
                    320 * (PLuint) s,
                    200 * (PLuint) s);
            break;
        default:
            Sys_Error("GL_SetCanvas: bad canvas type");
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#endif
}

void draw::ResetCanvas() {
    currentcanvas = (VideoCanvasType_t) -1;

    GL_SetCanvas(CANVAS_DEFAULT);
}

/*	Entities	*/

PL_MODULE_EXPORT void draw::EntityBoundingBox(ClientEntity_t *entity) {
    if (!entity->model || ((entity == &cl_entities[cl.viewentity]) && !chase_active.bValue) || (entity == &cl.viewent))
        return;

    plVector3f_t mins = {0}, maxs = {0};
    switch (entity->model->type) {
        case MODEL_TYPE_LEVEL:
            // Only draw wires for the BSP, since otherwise it's difficult to see anything else.
#ifdef VL_MODE_OPENGL
            glColor4f(0, 0, 0, 0);
#endif

            Math_VectorAdd(entity->model->mins, entity->origin, mins);
            Math_VectorAdd(entity->model->maxs, entity->origin, maxs);
            draw::WireBox(mins, maxs, 0, 1, 0);
            break;
        default: {
#ifdef VL_MODE_OPENGL
            glColor4f(0.5f, 0, 0, 0.5f);
#endif
            Math_VectorAdd(entity->model->rmins, entity->origin, mins);
            Math_VectorAdd(entity->model->rmaxs, entity->origin, maxs);
            draw::WireBox(mins, maxs, 1, 0, 0);

            Math_VectorAdd(entity->model->ymins, entity->origin, mins);
            Math_VectorAdd(entity->model->ymaxs, entity->origin, maxs);
            draw::WireBox(mins, maxs, 0, 1, 0);

            Math_VectorAdd(entity->model->mins, entity->origin, mins);
            Math_VectorAdd(entity->model->maxs, entity->origin, maxs);
            draw::WireBox(mins, maxs, 0, 0, 1);
        }
            break;
    }
}

PL_MODULE_EXPORT void draw::Entity(ClientEntity_t *entity) {
    if (!entity->model) {
        draw::CoordinateAxes(entity->origin);
        return;
    }

    entity->distance_alpha = 1.0f;
    if (cv_video_entity_fade.bValue) {
        Camera *camera = g_cameramanager->GetCurrentCamera();
        if (camera) {
            plVector3f_t vdist = {0};
            plVectorSubtract3fv(&camera->GetPosition()[0], entity->origin, vdist);
            float distance = plLengthf(vdist);
            if (distance > cv_video_entity_distance.value) {
                entity->distance_alpha = 1.0f - (((distance - cv_video_entity_distance.value) / 100.0f) / 1.0f);
                if (entity->distance_alpha < 0.01)
                    return;
            }
        }
    }

    switch (entity->model->type) {
#if 1    // Old legacy draws
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

    if (r_showbboxes.bValue) {
        glEnable(GL_BLEND);
        draw::EntityBoundingBox(entity);
        glDisable(GL_BLEND);
    }
}
