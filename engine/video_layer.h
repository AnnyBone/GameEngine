/*	
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#pragma once

#include "platform_graphics.h"

typedef enum PLFBOTarget
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
    PL_FRAMEBUFFER_DEFAULT	= GL_FRAMEBUFFER,
	VL_FRAMEBUFFER_DRAW		= GL_DRAW_FRAMEBUFFER,
	VL_FRAMEBUFFER_READ		= GL_READ_FRAMEBUFFER
#else
	VL_FRAMEBUFFER_DEFAULT,
	VL_FRAMEBUFFER_DRAW,
	VL_FRAMEBUFFER_READ
#endif
} PLFBOTarget;

typedef enum
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	VL_TEXTURECLAMP_CLAMP	= GL_CLAMP_TO_EDGE,
	VL_TEXTURECLAMP_WRAP	= GL_REPEAT,
#elif defined (VL_MODE_GLIDE)
	VL_TEXTURECLAMP_CLAMP	= GR_TEXTURECLAMP_CLAMP,
	VL_TEXTURECLAMP_WRAP	= GR_TEXTURECLAMP_WRAP,
#else
	VL_TEXTURECLAMP_CLAMP,
	VL_TEXTURECLAMP_WRAP,
#endif
} vlTextureClamp_t;

plEXTERN_C_START

void vlInit(void);

char *vlGetErrorString(unsigned int er);

void vlPushMatrix(void);
void vlPopMatrix(void);

void vlDepthMask(bool mode);

//-----------------
// Shaders

void vlLinkShaderProgram(PLShaderProgram *program);
void vlAttachShader(PLShaderProgram program, PLShader shader);

PLAttribute vlGetAttributeLocation(PLShaderProgram *program, const char *name);

//-----------------
// Drawing

PLDraw *vlCreateDraw(PLPrimitive primitive, uint32_t num_tris, uint32_t num_verts);
void vlDeleteDraw(PLDraw *draw);

void vlBeginDraw(PLDraw *draw);
void vlEndDraw(PLDraw *draw);
void vlDrawVertex3f(float x, float y, float z);
void vlDrawVertex3fv(plVector3f_t position);
void vlDrawColour4f(float r, float g, float b, float a);
void vlDrawColour4fv(PLColour rgba);
void vlDrawNormal3fv(plVector3f_t position);
void vlDrawTexCoord2f(unsigned int target, float s, float t);
void vlDrawVertexNormals(PLDraw *draw);

//-----------------
// Lighting

void vlApplyLighting(PLDraw *object, PLLight *light, plVector3f_t position);

//-----------------

// Frame Buffer
void vlGenerateFrameBuffer(unsigned int *buffer);
void vlCheckFrameBufferStatus();
void vlBindFrameBuffer(PLFBOTarget target, unsigned int buffer);
void vlAttachFrameBufferRenderBuffer(unsigned int attachment, unsigned int buffer);
void vlAttachFrameBufferTexture(gltexture_t *buffer);
void vlDeleteFrameBuffer(unsigned int *buffer);

// Render Buffer
void vlGenerateRenderBuffer(unsigned int *buffer);
void vlBindRenderBuffer(unsigned int buffer);
void vlRenderBufferStorage(int format, int samples, unsigned int width, unsigned int height);
void vlDeleteRenderBuffer(unsigned int *buffer);

void vlSwapBuffers(void);
void vlScissor(int x, int y, unsigned int width, unsigned int height);
void vlColourMask(bool red, bool green, bool blue, bool alpha);

plEXTERN_C_END