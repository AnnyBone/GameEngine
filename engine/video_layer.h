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

PL_EXTERN_C

void vlInit(void);

char *vlGetErrorString(unsigned int er);

void vlPushMatrix(void);
void vlPopMatrix(void);

void vlDepthMask(bool mode);

//-----------------
// Drawing

PLMesh *vlCreateDraw(PLPrimitive primitive, uint32_t num_tris, uint32_t num_verts);
void vlDeleteDraw(PLMesh *draw);

void vlBeginDraw(PLMesh *draw);
void vlEndDraw(PLMesh *draw);
void vlDrawVertex3f(float x, float y, float z);
void vlDrawVertex3fv(PLVector3D position);
void vlDrawColour4f(float r, float g, float b, float a);
void vlDrawColour4fv(PLColour rgba);
void vlDrawNormal3fv(PLVector3D position);
void vlDrawTexCoord2f(unsigned int target, float s, float t);
void vlDrawVertexNormals(PLMesh *draw);

//-----------------

// Frame Buffer
void vlGenerateFrameBuffer(unsigned int *buffer);
void vlCheckFrameBufferStatus(PLFBOTarget target);
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
void vlColourMask(bool red, bool green, bool blue, bool alpha);

PL_EXTERN_C_END