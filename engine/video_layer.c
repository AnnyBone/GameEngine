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

const char *vlGetString(unsigned int name)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
	return grGetString(name);
#else
#endif
	VIDEO_FUNCTION_END
}

const char *vlGetVendor(void)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
	return grGetString(GR_VENDOR);
#else
	return glGetString(GL_VENDOR);
#endif
	VIDEO_FUNCTION_END
}

const char *vlGetRenderer(void)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
	return grGetString(GR_RENDERER);
#else
	return glGetString(GL_RENDERER);
#endif
	VIDEO_FUNCTION_END
}

const char *vlGetVersion(void)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
	return grGetString(GR_VERSION);
#else
	return glGetString(GL_VERSION);
#endif
	VIDEO_FUNCTION_END
}

const char *vlGetExtensions(void)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
	return grGetString(GR_EXTENSION);
#else
	return glGetString(GL_EXTENSION);
#endif
	VIDEO_FUNCTION_END
}

void vlGetMaxTextureImageUnits(int *params)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
	grGet(GR_NUM_TMU, sizeof(params), (FxI32*)params);
#else
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, params);
#endif
	VIDEO_FUNCTION_END
}

void vlGetMaxTextureAnistropy(float *params)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
	params = 0;
#else
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, params);
#endif
	VIDEO_FUNCTION_END
}

// String

/*===========================
	OPENGL ERROR HANDLING
===========================*/

/*	Checks glGetError and returns a generic string describing the fault.
*/
char *vlGetErrorString(unsigned int er)
{
	VIDEO_FUNCTION_START
	switch (er)
	{
#ifdef KATANA_CORE_GLIDE
#else
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
	OPENGL MATRICES
===========================*/

static bool vl_matrixpushed = false;

void vlPushMatrix(void)
{
	VIDEO_FUNCTION_START
	if (vl_matrixpushed)
		return;
#ifdef KATANA_CORE_GLIDE
#else
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
#ifdef KATANA_CORE_GLIDE
#else
	glPopMatrix();
#endif
	vl_matrixpushed = false;
	VIDEO_FUNCTION_END
}

/*===========================
	SHADERS
===========================*/

void VideoLayer_UseProgram(unsigned int program)
{
	VIDEO_FUNCTION_START
	if (program == Video.current_program)
		return;
#ifndef KATANA_CORE_GLIDE
	glUseProgram(program);
#endif
	Video.current_program = program;
	VIDEO_FUNCTION_END
}

/*===========================
	OPENGL TEXTURES
===========================*/

unsigned int VideoLayer_TranslateFormat(VideoTextureFormat_t Format)
{
	VIDEO_FUNCTION_START
	switch (Format)
	{
	case VIDEO_TEXTURE_FORMAT_BGR:
#ifdef KATANA_CORE_GLIDE
#else
		return GL_BGR;
#endif
	case VIDEO_TEXTURE_FORMAT_BGRA:
#ifdef KATANA_CORE_GLIDE
#else
		return GL_BGRA;
#endif
	case VIDEO_TEXTURE_FORMAT_LUMINANCE:
#ifdef KATANA_CORE_GLIDE
#else
		return GL_LUMINANCE;
#endif
	case VIDEO_TEXTURE_FORMAT_RGB:
#ifdef KATANA_CORE_GLIDE
#else
		return GL_RGB;
#endif
	case VIDEO_TEXTURE_FORMAT_RGBA:
#ifdef KATANA_CORE_GLIDE
#else
		return GL_RGBA;
#endif
	default:
		Sys_Error("Unknown texture format! (%i)\n", Format);
	}

	// Won't be hit but meh, compiler will complain otherwise.
	return 0;
	VIDEO_FUNCTION_END
}

void vlSetupTexture(VideoTextureFormat_t InternalFormat, VideoTextureFormat_t Format, unsigned int Width, unsigned int Height)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
#else
	glTexImage2D(GL_TEXTURE_2D, 0, 
		VideoLayer_TranslateFormat(InternalFormat),
		Width, Height, 0, 
		VideoLayer_TranslateFormat(Format), 
		GL_UNSIGNED_BYTE, NULL);
#endif
	VIDEO_FUNCTION_END
}

/*	TODO:
		Modify this so it works as a replacement for TexMgr_SetFilterModes.
*/
void vlSetTextureFilter(VideoTextureFilter_t FilterMode)
{
	VIDEO_FUNCTION_START
	unsigned int filter = 0;
	switch (FilterMode)
	{
	case VIDEO_TEXTURE_FILTER_LINEAR:
#ifdef KATANA_CORE_GLIDE
		filter = GR_TEXTUREFILTER_BILINEAR;
#else
		filter = GL_LINEAR;
#endif
		break;
	case VIDEO_TEXTURE_FILTER_NEAREST:
#ifdef KATANA_CORE_GLIDE
		filter = GR_TEXTUREFILTER_POINT_SAMPLED;
#else
		filter = GL_NEAREST;
#endif
		break;
	default:
		Sys_Error("Unknown texture filter type! (%i)\n", FilterMode);
	}

#ifdef KATANA_CORE_GLIDE
	grTexFilterMode(Video.current_textureunit, filter, filter);
#else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
#endif
	VIDEO_FUNCTION_END
}

int VideoLayer_TranslateTextureEnvironmentMode(VideoTextureEnvironmentMode_t tem)
{
	VIDEO_FUNCTION_START
	switch (tem)
	{
	case VIDEO_TEXTURE_MODE_ADD:
#ifdef KATANA_CORE_GLIDE
#else
		return GL_ADD;
#endif
	case VIDEO_TEXTURE_MODE_MODULATE:
#ifdef KATANA_CORE_GLIDE
#else
		return GL_MODULATE;
#endif
	case VIDEO_TEXTURE_MODE_DECAL:
#ifdef KATANA_CORE_GLIDE
#else
		return GL_DECAL;
#endif
	case VIDEO_TEXTURE_MODE_BLEND:
#ifdef KATANA_CORE_GLIDE
#else
		return GL_BLEND;
#endif
	case VIDEO_TEXTURE_MODE_REPLACE:
#ifdef KATANA_CORE_GLIDE
#else
		return GL_REPLACE;
#endif
	case VIDEO_TEXTURE_MODE_COMBINE:
#ifdef KATANA_CORE_GLIDE
#else
		return GL_COMBINE;
#endif
	default:
		Sys_Error("Unknown texture environment mode! (%i)\n", tem);
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

#ifdef KATANA_CORE_GLIDE
#else
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, 
		VideoLayer_TranslateTextureEnvironmentMode(TextureEnvironmentMode));
#endif

	Video.textureunits[Video.current_textureunit].current_envmode = TextureEnvironmentMode;
	VIDEO_FUNCTION_END
}

/*===========================
	OPENGL CAPABILITIES
===========================*/

typedef struct
{
	unsigned int first, second;

	const char *ident;
} VideoLayerCapabilities_t;

VideoLayerCapabilities_t capabilities[] =
{
#ifdef KATANA_CORE_GLIDE
#else
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
#ifdef KATANA_CORE_GLIDE
#else
	for (int i = 0; i < sizeof(capabilities); i++)
	{
		// Check if we reached the end of the list yet.
		if (!capabilities[i].first)
			break;

		if (uiCapabilities & VIDEO_TEXTURE_2D)
			Video.textureunit_state[Video.current_textureunit] = true;

		if (uiCapabilities & capabilities[i].first)
		{
			if (Video.debug_frame)
				plWriteLog(VIDEO_LOG, "Enabling %s (%i)\n", capabilities[i].ident, Video.current_textureunit);

			glEnable(capabilities[i].second);
		}
	}
#endif
	VIDEO_FUNCTION_END
}

void vlDisable(unsigned int uiCapabilities)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
#else
	for (int i = 0; i < sizeof(capabilities); i++)
	{
		// Check if we reached the end of the list yet.
		if (!capabilities[i].first)
			break;

		if (uiCapabilities & VIDEO_TEXTURE_2D)
			Video.textureunit_state[Video.current_textureunit] = false;

		if (uiCapabilities & capabilities[i].first)
		{
			// TODO: Implement debugging support.

			glDisable(capabilities[i].second);
		}
	}
#endif
	VIDEO_FUNCTION_END
}

void vlBlendFunc(VideoBlend_t modea, VideoBlend_t modeb)
{
	VIDEO_FUNCTION_START
	if (Video.debug_frame)
		plWriteLog(VIDEO_LOG, "Video: Setting blend mode (%i) (%i)\n", modea, modeb);
#ifdef KATANA_CORE_GLIDE
#else
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
	if (mode == cur_state) 
		return;
#ifdef KATANA_CORE_GLIDE
	grDepthMask(mode);
#else
	glDepthMask(mode);
#endif
	cur_state = mode;
	VIDEO_FUNCTION_END
}

/*	Enable and disable writing of frame buffer colour components.
*/
void vlColorMask(bool red, bool green, bool blue, bool alpha)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
	bool rgb = false;
	if(red || green || blue) rgb = true;
	grColorMask(rgb, alpha);
#else
	glColorMask(red, green, blue, alpha);
#endif
	VIDEO_FUNCTION_END
}

/*===========================
	OPENGL OBJECTS
===========================*/

// RENDER BUFFER OBJECTS

void vlGenerateRenderBuffer(unsigned int *buffer)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
#else
	glGenRenderbuffers(1, buffer);
#endif
	VIDEO_FUNCTION_END
}

void vlDeleteRenderBuffer(unsigned int *buffer)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
#else
	glDeleteRenderbuffers(1, buffer);
#endif
	VIDEO_FUNCTION_END
}

void vlBindRenderBuffer(unsigned int buffer)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
#else
	glBindRenderbuffer(GL_RENDERBUFFER, buffer);
#endif
	VIDEO_FUNCTION_END
}

void vlRenderBufferStorage(int format, int samples, unsigned int width, unsigned int height)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
#else
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
*/
void vlGenerateVertexArray(unsigned int *arrays)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
#else
	glGenVertexArrays(1, arrays);
#endif
	VIDEO_FUNCTION_END
}

/*	Binds the given vertex array.
*/
void vlBindVertexArray(unsigned int array)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
#else
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
#ifdef KATANA_CORE_GLIDE
#else
	glGenBuffers(1, buffer);
#endif
	VIDEO_FUNCTION_END
}

void vlGenerateVertexBuffers(int num, unsigned int *buffers)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
#else
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
#ifdef KATANA_CORE_GLIDE
#else
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
#ifdef KATANA_CORE_GLIDE
#else
	glBindBuffer(target, buffer);
#endif
	VIDEO_FUNCTION_END
}

// FRAME BUFFER OBJECTS

void vlClearStencilBuffer(void)
{
#ifdef KATANA_CORE_GL
	glClear(GL_STENCIL_BUFFER_BIT);
#endif
}

/*	Clears buffers.
	TODO: expaaaand!!
*/
void vlClear(void)
{
#ifdef KATANA_CORE_GLIDE
	grBufferClear(0x00, 0, 0);
#else
	if (!cv_video_clearbuffers.bValue)
		return;

	int clear = 0;
	if (cv_video_drawmirrors.bValue)
		clear |= GL_STENCIL_BUFFER_BIT;

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | clear);
#endif
}

/*	Generates a single framebuffer.
	glGenFramebuffers
*/
void vlGenerateFrameBuffer(unsigned int *buffer)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
#else
	glGenFramebuffers(1, buffer);
#endif
	VIDEO_FUNCTION_END
}

/*	Ensures that the framebuffer is valid, otherwise throws an error.
	glCheckFramebufferStatus
*/
void vlCheckFrameBufferStatus()
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
#else
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
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
	glBindFramebuffer
*/
void vlBindFrameBuffer(VideoFBOTarget_t vtTarget, unsigned int uiBuffer)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
#else
	unsigned int outtarget;

	// TODO: Get these named up so we can easily debug.
	switch (vtTarget)
	{
	case VIDEO_FBO_DRAW:
		outtarget = GL_DRAW_BUFFER;
		break;
	case VIDEO_FBO_READ:
		outtarget = GL_READ_BUFFER;
		break;
	default:
		outtarget = GL_FRAMEBUFFER;
	}

	glBindFramebuffer(outtarget, uiBuffer);

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
#ifdef KATANA_CORE_GLIDE
#else
	glDeleteFramebuffers(1, uiBuffer);
#endif
	VIDEO_FUNCTION_END
}

void vlAttachFrameBufferRenderBuffer(unsigned int attachment, unsigned int buffer)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
#else
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, buffer);
#endif
	VIDEO_FUNCTION_END
}

void vlAttachFrameBufferTexture(gltexture_t *buffer)
{
	VIDEO_FUNCTION_START
#ifdef KATANA_CORE_GLIDE
#else
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer->texnum, 0);
#endif
	VIDEO_FUNCTION_END
}

void vlReadPixels(int x, int y, unsigned int width, unsigned int height, unsigned int format, unsigned int type, void *data)
{
#ifdef KATANA_CORE_GLIDE
#else
#endif
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
#ifdef KATANA_CORE_GLIDE
	{ VIDEO_PRIMITIVE_LINES,				GR_LINES,			"LINES" },
	{ VIDEO_PRIMITIVE_POINTS,				GR_POINTS,			"POINTS" },
	{ VIDEO_PRIMITIVE_TRIANGLES,			GR_TRIANGLES,		"TRIANGLES" },
	{ VIDEO_PRIMITIVE_TRIANGLE_FAN,			GR_TRIANGLE_FAN,	"TRIANGLE_FAN" },
	{ VIDEO_PRIMITIVE_TRIANGLE_FAN_LINE,	GR_LINES,			"TRIANGLE_FAN_LINE" },
	{ VIDEO_PRIMITIVE_TRIANGLE_STRIP,		GR_TRIANGLE_STRIP,	"TRIANGLE_STRIP" },
	// TODO: Quads aren't supported; need to translate somehow.
#else
	{ VIDEO_PRIMITIVE_LINES,				GL_LINES,			"LINES" },
	{ VIDEO_PRIMITIVE_POINTS,				GL_POINTS,			"POINTS" },
	{ VIDEO_PRIMITIVE_TRIANGLES,			GL_TRIANGLES,		"TRIANGLES" },
	{ VIDEO_PRIMITIVE_TRIANGLE_FAN,			GL_TRIANGLE_FAN,	"TRIANGLE_FAN" },
	{ VIDEO_PRIMITIVE_TRIANGLE_FAN_LINE,	GL_LINES,			"TRIANGLE_FAN_LINE" },
	{ VIDEO_PRIMITIVE_TRIANGLE_STRIP,		GL_TRIANGLE_STRIP,	"TRIANGLE_STRIP" },
	{ VIDEO_PRIMITIVE_QUADS,				GL_QUADS,			"QUADS" }
#endif
};

unsigned int vlTranslatePrimitiveType(VideoPrimitive_t primitive)
{
	for (int i = 0; i < pARRAYELEMENTS(vl_primitives); i++)
		if (primitive == vl_primitives[i].primitive)
			return vl_primitives[i].gl;

#ifdef KATANA_CORE_GLIDE
	return GR_POINTS;
#else
	return GL_POINTS;
#endif
}

/*	Deals with tris view and different primitive types, then finally draws
	the given arrays.
*/
void vlDrawArrays(VideoPrimitive_t mode, unsigned int first, unsigned int count)
{
	if ((mode == VIDEO_PRIMITIVE_IGNORE) || (count == 0))
		return;

#ifdef KATANA_CORE_GLIDE
#else
	glDrawArrays(vlTranslatePrimitiveType(mode), first, count);
#endif
}

/*
*/
void vlDrawElements(VideoPrimitive_t mode, unsigned int count, unsigned int type, const void *indices)
{
	VIDEO_FUNCTION_START
	if ((mode == VIDEO_PRIMITIVE_IGNORE) || (count == 0))
		return;

	if (!indices)
		Sys_Error("Invalid indices when drawing object! (%i) (%i) (%i)\n", mode, count, type);

#ifdef KATANA_CORE_GLIDE
#else
	glDrawElements(vlTranslatePrimitiveType(mode), count, type, indices);
#endif
	VIDEO_FUNCTION_END
}

VideoObject_t video_curobject;

void vlBegin(VideoPrimitive_t mode)
{
	if ((mode <= VIDEO_PRIMITIVE_IGNORE) || (mode >= VIDEO_PRIMITIVE_END))
		Sys_Error("Invalid primitive mode for object!\n");

	// Set cur primitive.
	video_curobject.primitive = mode;
}

void vlVertex3f(float x, float y, float z)
{}

void vlNormal3f(float x, float y, float z)
{}

void vlColor3f(float r, float g, float b)
{}

void vlEnd(void)
{
	VideoObject_DrawImmediate(&video_curobject);

	// We're done, don't use this again.
	video_curobject.primitive = VIDEO_PRIMITIVE_IGNORE;
}

