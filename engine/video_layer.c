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

#include "video.h"

/*
	This document acts as a layer between the video sub-system
	and the graphics API.
	
	All graphics API functionality should be here.
*/

/*===========================
	GET
===========================*/

void vlGetMaxTextureImageUnits(int *params)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, params);
#else
	params = 0;
#endif
	VIDEO_FUNCTION_END
}

void vlGetMaxTextureAnistropy(float *params)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, params);
#else	// Not supported in core?
	params = 0;
#endif
	VIDEO_FUNCTION_END
}

const char *vlGetVendor(void)
{
	VIDEO_FUNCTION_START
#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
	return (const char*)glGetString(GL_VENDOR);
#else
	return "";
#endif
	VIDEO_FUNCTION_END
}

const char *vlGetExtensions(void)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	return (const char*)glGetString(GL_EXTENSIONS);
	// TODO: this works differently in core; use glGetStringi instead!
#else
	return "";
#endif
	VIDEO_FUNCTION_END
}

const char *vlGetVersion(void)
{
	VIDEO_FUNCTION_START
#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
	return (const char*)glGetString(GL_VERSION);
#else
	return "";
#endif
	VIDEO_FUNCTION_END
}

const char *vlGetRenderer(void)
{
	VIDEO_FUNCTION_START
#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
	return (const char*)glGetString(GL_RENDERER);
#else
	return "";
#endif
	VIDEO_FUNCTION_END
}

const char *vlGetString(vlString_t string)
{
	VIDEO_FUNCTION_START
#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
	if (string == VL_STRING_EXTENSIONS)
		// This works differently in core.
		return vlGetExtensions();
	return (const char*)glGetString(string);
#elif defined VL_MODE_GLIDE
	return grGetString(string);
#else
	return "";
#endif
	VIDEO_FUNCTION_END
}

// String

/*===========================
	ERROR HANDLING
===========================*/

/*	Returns a generic string describing the fault.
*/
char *vlGetErrorString(unsigned int er)
{
	VIDEO_FUNCTION_START
	switch (er)
	{
#ifdef VL_MODE_OPENGL
	case GL_NO_ERROR:
		return "No error has been recorded.";
	case GL_INVALID_ENUM:
		return "An unacceptable value is specified for an enumerated argument.";
	case GL_INVALID_VALUE:
		return "A numeric argument is out of range.";
	case GL_INVALID_OPERATION:
		return "The specified operation is not allowed in the current state.";
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return "The framebuffer object is not complete.";
	case GL_OUT_OF_MEMORY:
		return "There is not enough memory left to execute the command.";
	case GL_STACK_UNDERFLOW:
		return "An attempt has been made to perform an operation that would	cause an internal stack to underflow.";
	case GL_STACK_OVERFLOW:
		return "An attempt has been made to perform an operation that would	cause an internal stack to overflow.";
#endif
	default:
		return "An unknown error occured.";
	}
	VIDEO_FUNCTION_END
}

/*===========================
	MATRICES
===========================*/

static bool vl_matrixpushed = false;

void vlPushMatrix(void)
{
	VIDEO_FUNCTION_START
	if (vl_matrixpushed)
		return;
#ifdef VL_MODE_OPENGL
	glPushMatrix();
#endif
	vl_matrixpushed = true;
	VIDEO_FUNCTION_END
}

void vlPopMatrix(void)
{
	VIDEO_FUNCTION_START
	if (!vl_matrixpushed)
		return;
#ifdef VL_MODE_OPENGL
	glPopMatrix();
#endif
	vl_matrixpushed = false;
	VIDEO_FUNCTION_END
}

/*===========================
	SHADERS
===========================*/

void vlUseProgram(unsigned int program)
{
	VIDEO_FUNCTION_START
	if (program == Video.current_program)
		return;
#ifdef VL_MODE_OPENGL
	glUseProgram(program);
#endif
	Video.current_program = program;
	VIDEO_FUNCTION_END
}

/*===========================
	TEXTURES
===========================*/

void vlTexImage2D(vlTextureTarget_t target, vlTextureFormat_t internal_format, vlTextureFormat_t format, int width, int height, const void *data)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glTexImage2D(target, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
#endif
	VIDEO_FUNCTION_END
}

/*	Checks and returns texture unit for target.
*/
unsigned int vlGetTextureUnit(unsigned int target)
{
#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
	unsigned int out = GL_TEXTURE0 + target;
	if (out >(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS - 1))
		Sys_Error("Attempted to select an invalid texture image unit! (%i)\n", out);
	return out;
#else
	return target;
#endif
}

/*	Selects the current active TMU.
*/
void vlActiveTexture(unsigned int texunit)
{
	VIDEO_FUNCTION_START
#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
	glActiveTexture(vlGetTextureUnit(texunit));
#endif
	VIDEO_FUNCTION_END
}

/*	TODO:
		Modify this so it works as a replacement for TexMgr_SetFilterModes.
*/
void vlSetTextureFilter(vlTextureFilter_t filter)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
#endif
	VIDEO_FUNCTION_END
}

int VideoLayer_TranslateTextureEnvironmentMode(VideoTextureEnvironmentMode_t TextureEnvironmentMode)
{
	VIDEO_FUNCTION_START
	switch (TextureEnvironmentMode)
	{
#ifdef VL_MODE_OPENGL
	case VIDEO_TEXTURE_MODE_ADD:
		return GL_ADD;
	case VIDEO_TEXTURE_MODE_MODULATE:
		return GL_MODULATE;
	case VIDEO_TEXTURE_MODE_DECAL:
		return GL_DECAL;
	case VIDEO_TEXTURE_MODE_BLEND:
		return GL_BLEND;
	case VIDEO_TEXTURE_MODE_REPLACE:
		return GL_REPLACE;
	case VIDEO_TEXTURE_MODE_COMBINE:
		return GL_COMBINE;
#endif
	default:
		Sys_Error("Unknown texture environment mode! (%i)\n", TextureEnvironmentMode);
	}

	// Won't be hit but meh, compiler will complain otherwise.
	return 0;
	VIDEO_FUNCTION_END
}

void vlSetTextureEnvironmentMode(VideoTextureEnvironmentMode_t TextureEnvironmentMode)
{
	VIDEO_FUNCTION_START
	// Ensure there's actually been a change.
	if (Video.textureunits[Video.current_textureunit].current_envmode == TextureEnvironmentMode)
		return;

#ifdef VL_MODE_OPENGL
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, 
		VideoLayer_TranslateTextureEnvironmentMode(TextureEnvironmentMode));
#endif

	Video.textureunits[Video.current_textureunit].current_envmode = TextureEnvironmentMode;
	VIDEO_FUNCTION_END
}

/*===========================
	CAPABILITIES
===========================*/

typedef struct
{
	unsigned int vl_parm, to_parm;

	const char *ident;
} VideoLayerCapabilities_t;

VideoLayerCapabilities_t capabilities[] =
{
#ifdef VL_MODE_OPENGL
	{ VIDEO_ALPHA_TEST, GL_ALPHA_TEST, "ALPHA_TEST" },
	{ VIDEO_BLEND, GL_BLEND, "BLEND" },
	{ VIDEO_DEPTH_TEST, GL_DEPTH_TEST, "DEPTH_TEST" },
	{ VIDEO_TEXTURE_2D, GL_TEXTURE_2D, "TEXTURE_2D" },
	{ VIDEO_TEXTURE_GEN_S, GL_TEXTURE_GEN_S, "TEXTURE_GEN_S" },
	{ VIDEO_TEXTURE_GEN_T, GL_TEXTURE_GEN_T, "TEXTURE_GEN_T" },
	{ VIDEO_CULL_FACE, GL_CULL_FACE, "CULL_FACE" },
	{ VIDEO_STENCIL_TEST, GL_STENCIL_TEST, "STENCIL_TEST" },
	{ VIDEO_NORMALIZE, GL_NORMALIZE, "NORMALIZE" },
	{ VIDEO_MULTISAMPLE, GL_MULTISAMPLE, "MULTISAMPLE" },
#endif

	{ 0 }
};

/*	Enables video capabilities.
*/
void vlEnable(unsigned int uiCapabilities)
{
	VIDEO_FUNCTION_START
	int i;
	for (i = 0; i < sizeof(capabilities); i++)
	{
		// Check if we reached the end of the list yet.
		if (!capabilities[i].vl_parm)
			break;

		if (uiCapabilities & VIDEO_TEXTURE_2D)
			Video.textureunit_state[Video.current_textureunit] = true;

		if (uiCapabilities & capabilities[i].vl_parm)
		{
			if (Video.debug_frame)
				plWriteLog(VIDEO_LOG, "Enabling %s (%i)\n", capabilities[i].ident, Video.current_textureunit);

#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
			glEnable(capabilities[i].to_parm);
#endif
		}
	}
	VIDEO_FUNCTION_END
}

void vlDisable(unsigned int uiCapabilities)
{
	VIDEO_FUNCTION_START
	for (int i = 0; i < sizeof(capabilities); i++)
	{
		// Check if we reached the end of the list yet.
		if (!capabilities[i].vl_parm)
			break;

		if (uiCapabilities & VIDEO_TEXTURE_2D)
			Video.textureunit_state[Video.current_textureunit] = false;

		if (uiCapabilities & capabilities[i].vl_parm)
		{
			// TODO: Implement debugging support.

#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
			glDisable(capabilities[i].to_parm);
#endif
		}
	}
	VIDEO_FUNCTION_END
}

/*	TODO: Want more control over the dynamics of this...
*/
void vlBlendFunc(VideoBlend_t modea, VideoBlend_t modeb)
{
	VIDEO_FUNCTION_START
	if (Video.debug_frame)
		plWriteLog(VIDEO_LOG, "Video: Setting blend mode (%i) (%i)\n", modea, modeb);
#ifdef VL_MODE_OPENGL
	glBlendFunc(modea, modeb);
#endif
	VIDEO_FUNCTION_END
}

/*	Enable or disable writing into the depth buffer.
*/
void vlDepthMask(bool mode)
{
	VIDEO_FUNCTION_START
	static bool cur_state = true;
	if (mode == cur_state) return;
#ifdef VL_MODE_OPENGL
	glDepthMask(mode);
#endif
	cur_state = mode;
	VIDEO_FUNCTION_END
}

/*===========================
	OBJECTS
===========================*/

// RENDER BUFFER OBJECTS

void vlGenerateRenderBuffer(vlRenderBuffer_t *buffer)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glGenRenderbuffers(1, buffer);
#endif
	VIDEO_FUNCTION_END
}

void vlDeleteRenderBuffer(vlRenderBuffer_t *buffer)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glDeleteRenderbuffers(1, buffer);
#endif
	VIDEO_FUNCTION_END
}

void vlBindRenderBuffer(vlRenderBuffer_t buffer)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glBindRenderbuffer(GL_RENDERBUFFER, buffer);
#endif
	VIDEO_FUNCTION_END
}

void vlRenderBufferStorage(int format, int samples, unsigned int width, unsigned int height)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);

	// Ensure there weren't any issues.
	unsigned int glerror = glGetError();
	if (glerror != GL_NO_ERROR)
	{
		char *errorstring = "";
		switch (glerror)
		{
		case GL_INVALID_ENUM:
			errorstring = "Invalid internal format!";
			break;
		case GL_OUT_OF_MEMORY:
			errorstring = "Unable to create a data store of the requested size!";
			break;
		default:
			// This should *NEVER* occur.
			break;
		}
		Sys_Error("%s\n%s", vlGetErrorString(glerror), errorstring);
	}
#endif
	VIDEO_FUNCTION_END
}

// VERTEX ARRAY OBJECTS

/*	Generates a single vertex array.
	glGenVertexArrays
*/
void vlGenerateVertexArray(vlVertexArray_t *arrays)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glGenVertexArrays(1, arrays);
#endif
	VIDEO_FUNCTION_END
}

/*	Binds the given vertex array.
	glBindVertexArray
*/
void vlBindVertexArray(vlVertexArray_t array)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glBindVertexArray(array);
#endif
	VIDEO_FUNCTION_END
}

// VERTEX BUFFER OBJECTS

/*	Generates a single OpenGL buffer.
	glGenBuffers
*/
void vlGenerateVertexBuffer(unsigned int *buffer)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glGenBuffers(1, buffer);
#endif
	VIDEO_FUNCTION_END
}

void vlGenerateVertexBuffers(int num, unsigned int *buffers)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glGenBuffers(num, buffers);
#endif
	VIDEO_FUNCTION_END
}

/*	Deletes a single OpenGL buffer.
	glDeleteBuffers
*/
void vlDeleteVertexBuffer(unsigned int *buffer)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glDeleteBuffers(1, buffer);
#endif
	VIDEO_FUNCTION_END
}

/*	Binds the given buffer.
	glBindBuffer
*/
void vlBindBuffer(unsigned int target, unsigned int buffer)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glBindBuffer(target, buffer);
#endif
	VIDEO_FUNCTION_END
}

// FRAME BUFFER OBJECTS

void vlClear(unsigned int mask)
{
	VIDEO_FUNCTION_START
#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
	glClear(mask);
#elif defined VL_MODE_GLIDE
	grBufferClear(0)
#endif
	VIDEO_FUNCTION_END
}

/*	Generates a single framebuffer.
*/
void vlGenerateFrameBuffer(vlFrameBuffer_t *buffer)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glGenFramebuffers(1, buffer);
#endif
	VIDEO_FUNCTION_END
}

/*	Ensures that the framebuffer is valid, otherwise throws an error.
	glCheckFramebufferStatus
*/
void vlCheckFrameBufferStatus(vlFBOTarget_t target)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	int status = glCheckFramebufferStatus(target);
	if (status == GL_FRAMEBUFFER_COMPLETE)
		return;

	// Generic GL error.
	if (status == 0)
		Sys_Error("An error occured when checking framebuffer! (%s)\n", vlGetErrorString(glGetError()));

	switch (status)
	{
	case GL_FRAMEBUFFER_UNDEFINED:
		Sys_Error("Default framebuffer doesn't exist!\n");
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		Sys_Error("Framebuffer attachment points are incomplete!\n");
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		Sys_Error("Framebuffer doesn't have an image attached!\n");
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		Sys_Error("Invalid attachment type!\n");
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		Sys_Error("INCOMPLETE READ BUFFER\n");
	case GL_FRAMEBUFFER_UNSUPPORTED:
		Sys_Error("UNSUPPORTED\n");
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		Sys_Error("INCOMPLETE MULTISAMPLE\n");
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_ARB:
		Sys_Error("INCOMPLETE LAYER TARGETS\n");
	}

	Sys_Error("An unknown error occured when checking framebuffer status!\n");
#endif
	VIDEO_FUNCTION_END
}

/*	Binds the given framebuffer.
*/
void vlBindFrameBuffer(vlFBOTarget_t vtTarget, unsigned int uiBuffer)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glBindFramebuffer(vtTarget, uiBuffer);

	// Ensure there weren't any issues.
	unsigned int glerror = glGetError();
	if (glerror != GL_NO_ERROR)
	{
		char *errorstring = "";
		switch (glerror)
		{
		case GL_INVALID_ENUM:
			errorstring = "Invalid framebuffer target!";
			break;
		case GL_INVALID_OPERATION:
			errorstring = "Invalid framebuffer object!";
			break;
		default:
			// This should *NEVER* occur.
			break;
		}
		Sys_Error("%s\n%s", vlGetErrorString(glerror), errorstring);
	}
#endif
	VIDEO_FUNCTION_END
}

/*	Deletes the given framebuffer.
*/
void vlDeleteFrameBuffer(unsigned int *uiBuffer)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glDeleteFramebuffers(1, uiBuffer);
#endif
	VIDEO_FUNCTION_END
}

void vlAttachFrameBufferRenderBuffer(unsigned int attachment, unsigned int buffer)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, buffer);
#endif
	VIDEO_FUNCTION_END
}

void vlAttachFrameBufferTexture(gltexture_t *buffer)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer->texnum, 0);
#endif
	VIDEO_FUNCTION_END
}

/*===========================
	DRAWING
===========================*/

typedef struct
{
	VideoPrimitive_t primitive;

	unsigned int gl;

	const char *name;
} VideoPrimitives_t;

VideoPrimitives_t vl_primitives[] =
{
#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
	{ VIDEO_PRIMITIVE_LINES,				GL_LINES,			"LINES" },
	{ VIDEO_PRIMITIVE_POINTS,				GL_POINTS,			"POINTS" },
	{ VIDEO_PRIMITIVE_TRIANGLES,			GL_TRIANGLES,		"TRIANGLES" },
	{ VIDEO_PRIMITIVE_TRIANGLE_FAN,			GL_TRIANGLE_FAN,	"TRIANGLE_FAN" },
	{ VIDEO_PRIMITIVE_TRIANGLE_FAN_LINE,	GL_LINES,			"TRIANGLE_FAN_LINE" },
	{ VIDEO_PRIMITIVE_TRIANGLE_STRIP,		GL_TRIANGLE_STRIP,	"TRIANGLE_STRIP" },
	{ VIDEO_PRIMITIVE_QUADS,				GL_QUADS,			"QUADS" }
#else
	{ 0 }
#endif
};

unsigned int vlTranslatePrimitiveType(VideoPrimitive_t primitive)
{
	VIDEO_FUNCTION_START
	for (int i = 0; i < pARRAYELEMENTS(vl_primitives); i++)
		if (primitive == vl_primitives[i].primitive)
			return vl_primitives[i].gl;

	// Hacky, but just return initial otherwise.
	return vl_primitives[0].gl;
	VIDEO_FUNCTION_END
}

/*	Deals with tris view and different primitive types, then finally draws
	the given arrays.
*/
void vlDrawArrays(VideoPrimitive_t mode, unsigned int first, unsigned int count)
{
	if (count == 0)
		return;

	unsigned int target = vlTranslatePrimitiveType(mode);
#ifdef VL_MODE_OPENGL
	glDrawArrays(target, first, count);
#endif
}

void vlDrawElements(VideoPrimitive_t mode, unsigned int count, unsigned int type, const void *indices)
{
	VIDEO_FUNCTION_START
	if ((count == 0) || !indices)
		return;

	unsigned int target = vlTranslatePrimitiveType(mode);
#ifdef VL_MODE_OPENGL
	glDrawElements(target, count, type, indices);
#endif
	VIDEO_FUNCTION_END
}

VideoObject_t vl_currentobject;

void vlBegin(VideoPrimitive_t mode)
{
	if ((mode <= VIDEO_PRIMITIVE_IGNORE) || (mode >= VIDEO_PRIMITIVE_END))
		Sys_Error("Invalid primitive mode for object!\n");

	// Set cur primitive.
	vl_currentobject.primitive = mode;
}

void vlVertex3f(float x, float y, float z)
{}

void vlNormal3f(float x, float y, float z)
{}

void vlColor3f(float r, float g, float b)
{}

/*	Draws the object and then discards it.
*/
void vlEnd(void)
{
	VideoObject_DrawImmediate(&vl_currentobject);

	// We're done, don't use this again.
	vl_currentobject.primitive = VIDEO_PRIMITIVE_IGNORE;
}

