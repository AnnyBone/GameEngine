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

#include "platform_video_layer.h"

typedef enum
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	VL_FRAMEBUFFER_DEFAULT	= GL_FRAMEBUFFER,
	VL_FRAMEBUFFER_DRAW		= GL_DRAW_FRAMEBUFFER,
	VL_FRAMEBUFFER_READ		= GL_READ_FRAMEBUFFER
#else
	VL_FRAMEBUFFER_DEFAULT,
	VL_FRAMEBUFFER_DRAW,
	VL_FRAMEBUFFER_READ
#endif
} vlFBOTarget_t;

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

const char *vlGetExtensions(void);
const char *vlGetString(VLString string);

void vlPushMatrix(void);
void vlPopMatrix(void);

void vlEnable(unsigned int cap);
void vlDisable(unsigned int cap);

void vlBlendFunc(VLBlend modea, VLBlend modeb);
void vlDepthMask(bool mode);

//-----------------
// Textures

VLTexture vlGenerateTexture(void);	// Returns id for the texture generated.

void vlDeleteTexture(VLTexture *texture);

void vlGetMaxTextureAnistropy(float *params);
void vlGetMaxTextureImageUnits(int *param);

int vlGetMaxTextureSize(void);

unsigned int vlGetTextureUnit(unsigned int target);

void vlActiveTexture(unsigned int texunit);

void vlSetTextureFilter(VLTexture texture, VLTextureFilter filter);
void vlSetTextureAnisotropy(VLTexture texture, PLfloat amount);
void vlSetTextureEnvironmentMode(VLTextureEnvironmentMode TextureEnvironmentMode);

void vlBindTexture(VLTextureTarget target, VLTexture texture);

void vlUploadTexture(VLTexture texture, const VLTextureInfo *upload);

//-----------------
// Shaders

vlShaderProgram_t vlCreateShaderProgram(void);
vlShaderProgram_t vlGetCurrentShaderProgram(void);

void vlUseShaderProgram(vlShaderProgram_t program);
void vlDeleteShaderProgram(vlShaderProgram_t *program);
void vlLinkShaderProgram(vlShaderProgram_t *program);
void vlAttachShader(vlShaderProgram_t program, vlShader_t shader);
void vlDeleteShader(vlShader_t *shader);

vlAttribute_t vlGetAttributeLocation(vlShaderProgram_t *program, const char *name);

//-----------------
// Drawing

vlDraw_t *vlCreateDraw(VLPrimitive primitive, uint32_t num_tris, uint32_t num_verts);
void vlDeleteDraw(vlDraw_t *draw);

void vlBeginDraw(vlDraw_t *draw);
void vlEndDraw(vlDraw_t *draw);
void vlDrawVertex3f(float x, float y, float z);
void vlDrawVertex3fv(plVector3f_t position);
void vlDrawColour4f(float r, float g, float b, float a);
void vlDrawColour4fv(plColour_t rgba);
void vlDrawNormal3fv(plVector3f_t position);
void vlDrawTexCoord2f(unsigned int target, float s, float t);

void vlDraw(vlDraw_t *draw);
void vlDrawVertexNormals(vlDraw_t *draw);

//-----------------
// Lighting

void vlApplyLighting(vlDraw_t *object, vlLight_t *light, plVector3f_t position);

//-----------------

// Frame Buffer
void vlGenerateFrameBuffer(unsigned int *buffer);
void vlCheckFrameBufferStatus();
void vlBindFrameBuffer(vlFBOTarget_t target, unsigned int buffer);
void vlAttachFrameBufferRenderBuffer(unsigned int attachment, unsigned int buffer);
void vlAttachFrameBufferTexture(gltexture_t *buffer);
void vlDeleteFrameBuffer(unsigned int *buffer);

// Render Buffer
void vlGenerateRenderBuffer(unsigned int *buffer);
void vlBindRenderBuffer(unsigned int buffer);
void vlRenderBufferStorage(int format, int samples, unsigned int width, unsigned int height);
void vlDeleteRenderBuffer(unsigned int *buffer);

void vlViewport(int x, int y, unsigned int width, unsigned int height);

void vlSwapBuffers(void);
void vlSetClearColour4f(float r, float g, float b, float a);
void vlSetClearColour4fv(plColour_t rgba);
void vlClearBuffers(unsigned int mask);
void vlScissor(int x, int y, unsigned int width, unsigned int height);
void vlColourMask(bool red, bool green, bool blue, bool alpha);
void vlSetCullMode(VLCullMode mode);
void vlFinish(void);

plEXTERN_C_END