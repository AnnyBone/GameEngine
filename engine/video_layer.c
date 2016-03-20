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

	TODO:
		- This shouldn't rely on Katana.
		- Move this into platform?
		- Add OpenGL style lighting pipeline.
		- Matrix functionality, linked in with platform lib.
		- Add some... Object manager of sorts?
*/

typedef struct
{
	plColour_t	buffer_clearcolour;

	unsigned int capabilities;

	VLCull_t	current_cullmode;

	unsigned int num_cards;	// Number of video cards.
} VLstate_t;

VLstate_t vl_state;

/*===========================
	INITIALIZATION
===========================*/

#if defined (VL_MODE_GLIDE)
/*	Convert RGBA colour to something glide can understand.
*/
GrColor_t _vlConvertColour4f(VLColourFormat_t format, float r, float g, float b, float a)
{
	GrColor_t
		gr = (GrColor_t)r,
		gg = (GrColor_t)g,
		gb = (GrColor_t)b,
		ga = (GrColor_t)a;
	switch (format)
	{
	case VL_COLOURFORMAT_ABGR:
		return (ga << 24) | (gb << 16) | (gg << 8) | gr;
	case VL_COLOURFORMAT_ARGB:
		return (ga << 24) | (gr << 16) | (gg << 8) | gb;
	case VL_COLOURFORMAT_BGRA:
		return (gb << 24) | (gg << 16) | (gr << 8) | ga;
	case VL_COLOURFORMAT_RGBA:
		return (gr << 24) | (gg << 16) | (gb << 8) | ga;
	default:return 0;
	}
}

/*	Convert RGBA colour to something glide can understand.
*/
GrColor_t _vlConvertColour4fv(VLColourFormat_t format, plColour_t colour)
{
	return _vlConvertColour4f(format, colour[0], colour[1], colour[2], colour[3]);
}

void _vlGlideErrorCallback(const char *string, FxBool fatal)
{
	if (fatal)
		Sys_Error(string);

	Con_Warning(string);
}
#endif

/*	Function used for initialization in general.
*/
void vlInit(void)
{
	plVectorSet3f(vl_state.buffer_clearcolour, 0, 0, 0);
	vl_state.buffer_clearcolour[3] = 1;

	vl_state.capabilities		= 0;
	vl_state.current_cullmode	= -1;

#if defined (VL_MODE_OPENGL)
	unsigned int err = glewInit();
	if(err != GLEW_OK)
		Sys_Error("Failed to initialize glew!\n%s\n", glewGetErrorString(err));

	// Check that the required capabilities are supported.
	if (!GLEW_ARB_multitexture) Sys_Error("Video hardware incapable of multi-texturing!\n");
	else if (!GLEW_ARB_texture_env_combine && !GLEW_EXT_texture_env_combine) Sys_Error("ARB/EXT_texture_env_combine isn't supported by your hardware!\n");
	else if (!GLEW_ARB_texture_env_add && !GLEW_EXT_texture_env_add) Sys_Error("ARB/EXT_texture_env_add isn't supported by your hardware!\n");
	//else if (!GLEE_EXT_fog_coord) Sys_Error("EXT_fog_coord isn't supported by your hardware!\n");
	else if (!GLEW_ARB_vertex_program || !GLEW_ARB_fragment_program) Sys_Error("Shaders aren't supported by this hardware!\n");

	// Optional capabilities.
	if (GLEW_SGIS_generate_mipmap) Video.extensions.generate_mipmap = true;
	else Con_Warning("Hardware mipmap generation isn't supported!\n");
	if (GLEW_ARB_depth_texture) Video.extensions.depth_texture = true;
	else Con_Warning("ARB_depth_texture isn't supported by your hardware!\n");
	if (GLEW_ARB_shadow) Video.extensions.shadow = true;
	else Con_Warning("ARB_shadow isn't supported by your hardware!\n");
	if (GLEW_ARB_vertex_buffer_object) Video.extensions.vertex_buffer_object = true;
	else Con_Warning("Hardware doesn't support Vertex Buffer Objects!\n");
#elif defined (VL_MODE_GLIDE)
	grGet(GR_NUM_BOARDS, sizeof(vl_state.num_cards), (FxI32*)&vl_state.num_cards);
	if (vl_state.num_cards == 0)
		Sys_Error("No Glide capable hardware detected!\n");

	// Initialize Glide.
	grGlideInit();

	grErrorSetCallback(_vlGlideErrorCallback);
#endif

	// Set default states.
	vlSetClearColour4fv(vl_state.buffer_clearcolour);
	vlSetCullMode(VL_CULL_NEGATIVE);
}

/*===========================
	GET
===========================*/

/*	Returns supported num of texture width.
*/
unsigned int vlGetMaxTextureSize(void)
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	int size = 0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
	return (unsigned)size;
#elif defined (VL_MODE_GLIDE)
	FxI32 size = 0;
	grGet(GR_MAX_TEXTURE_SIZE, sizeof(size), &size);
	return (unsigned int)size;
#else
	// TODO: Is this even safe?
	return 4096;
#endif
}

void vlGetMaxTextureImageUnits(int *param)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, param);
#elif defined (VL_MODE_GLIDE)
	grGet(GR_NUM_TMU, sizeof(param), (FxI32*)param);
#else
	param = 0;
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
#elif defined (VL_MODE_GLIDE)
	return grGetString(GR_VENDOR);
#else
	return "Unknown";
#endif
	VIDEO_FUNCTION_END
}

const char *vlGetExtensions(void)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	return (const char*)glGetString(GL_EXTENSIONS);
	// TODO: this works differently in core; use glGetStringi instead!
#elif defined (VL_MODE_GLIDE)
	return grGetString(GR_EXTENSION);
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
#elif defined (VL_MODE_GLIDE)
	return grGetString(GR_VERSION);
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
#elif defined (VL_MODE_GLIDE)
	return grGetString(GR_RENDERER);
#else
	return "";
#endif
	VIDEO_FUNCTION_END
}

const char *vlGetString(VLString_t string)
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
		Sys_Error("Attempted to select an invalid texture image unit! (%i)\n", target);
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
	if (texunit == Video.current_textureunit)
		return;

	// Ensure it's valid.
	if (texunit > (unsigned)Video.num_textureunits)
		Sys_Error("Attempted to select a texture image unit beyond what's supported by your hardware! (%i)\n", texunit);

#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
	glActiveTexture(vlGetTextureUnit(texunit));
#endif

	// Keep us up-to-date.
	Video.current_textureunit = texunit;
	VIDEO_FUNCTION_END
}

/*	TODO:
		Modify this so it works as a replacement for TexMgr_SetFilterModes.
*/
void vlSetTextureFilter(VLTextureFilter_t filter)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
#endif
	VIDEO_FUNCTION_END
}

int _vlTranslateTextureEnvironmentMode(VideoTextureEnvironmentMode_t TextureEnvironmentMode)
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
		_vlTranslateTextureEnvironmentMode(TextureEnvironmentMode));
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
} VLCapabilities_t;

VLCapabilities_t vl_capabilities[] =
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	{ VL_CAPABILITY_ALPHA_TEST, GL_ALPHA_TEST, "ALPHA_TEST" },
	{ VL_CAPABILITY_BLEND, GL_BLEND, "BLEND" },
	{ VL_CAPABILITY_DEPTH_TEST, GL_DEPTH_TEST, "DEPTH_TEST" },
	{ VL_CAPABILITY_TEXTURE_2D, GL_TEXTURE_2D, "TEXTURE_2D" },
	{ VL_CAPABILITY_TEXTURE_GEN_S, GL_TEXTURE_GEN_S, "TEXTURE_GEN_S" },
	{ VL_CAPABILITY_TEXTURE_GEN_T, GL_TEXTURE_GEN_T, "TEXTURE_GEN_T" },
	{ VL_CAPABILITY_CULL_FACE, GL_CULL_FACE, "CULL_FACE" },
	{ VL_CAPABILITY_STENCIL_TEST, GL_STENCIL_TEST, "STENCIL_TEST" },
	{ VL_CAPABILITY_MULTISAMPLE, GL_MULTISAMPLE, "MULTISAMPLE" },
#else
	{ VL_CAPABILITY_ALPHA_TEST, 0, "ALPHA_TEST" },
	{ VL_CAPABILITY_BLEND, 0, "BLEND" },
	{ VL_CAPABILITY_DEPTH_TEST, 0, "DEPTH_TEST" },
	{ VL_CAPABILITY_TEXTURE_2D, 0, "TEXTURE_2D" },
	{ VL_CAPABILITY_TEXTURE_GEN_S, 0, "TEXTURE_GEN_S" },
	{ VL_CAPABILITY_TEXTURE_GEN_T, 0, "TEXTURE_GEN_T" },
	{ VL_CAPABILITY_CULL_FACE, 0, "CULL_FACE" },
	{ VL_CAPABILITY_STENCIL_TEST, 0, "STENCIL_TEST" },
	{ VL_CAPABILITY_MULTISAMPLE, 0, "MULTISAMPLE" },
#endif

	{ 0 }
};

bool vlIsEnabled(unsigned int caps)
{
	if (!caps)
		return false;

	for (int i = 0; i < sizeof(vl_capabilities); i++)
	{
		if (!vl_capabilities[i].vl_parm)
			break;

		if (caps & vl_state.capabilities)
			return true;
	}

	return false;
}

/*	Enables video capabilities.
*/
void vlEnable(unsigned int cap)
{
	VIDEO_FUNCTION_START
	int i;
	for (i = 0; i < sizeof(vl_capabilities); i++)
	{
		// Check if we reached the end of the list yet.
		if (!vl_capabilities[i].vl_parm)
			break;

		if (Video.debug_frame)
			plWriteLog(VIDEO_LOG, "Enabling %s (%i)\n", vl_capabilities[i].ident, Video.current_textureunit);
		
		if (cap & VL_CAPABILITY_TEXTURE_2D)
			Video.textureunits[Video.current_textureunit].isactive = true;
#if defined (VL_MODE_GLIDE)
		if (cap & VL_CAPABILITY_FOG)
			// TODO: need to check this is supported...
			grFogMode(GR_FOG_WITH_TABLE_ON_FOGCOORD_EXT);
		if (cap & VL_CAPABILITY_DEPTH_TEST)
			grDepthBufferMode(GR_DEPTHBUFFER_ZBUFFER);
		if (cap & VL_CAPABILITY_CULL_FACE)
			grCullMode(vl_state.current_cullmode);
#endif

		if (cap & vl_capabilities[i].vl_parm)
#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
			glEnable(vl_capabilities[i].to_parm);
#elif defined (VL_MODE_GLIDE)
			// Hacky, but just to be safe...
			if (vl_capabilities[i].to_parm != 0)
				grEnable(vl_capabilities[i].to_parm);
#endif

		vl_state.capabilities |= vl_capabilities[i].vl_parm;
	}
	VIDEO_FUNCTION_END
}

void vlDisable(unsigned int cap)
{
	VIDEO_FUNCTION_START
	if (!cap)
		return;

	for (int i = 0; i < sizeof(vl_capabilities); i++)
	{
		// Check if we reached the end of the list yet.
		if (!vl_capabilities[i].vl_parm)
			break;

		if (Video.debug_frame)
			plWriteLog(VIDEO_LOG, "Disabling %s (%i)\n", vl_capabilities[i].ident, Video.current_textureunit);

		if (cap & VL_CAPABILITY_TEXTURE_2D)
			Video.textureunits[Video.current_textureunit].isactive = false;
#if defined (VL_MODE_GLIDE)
		if (cap & VL_CAPABILITY_FOG)
			grFogMode(GR_FOG_DISABLE);
		if (cap & VL_CAPABILITY_DEPTH_TEST)
			grDepthBufferMode(GR_DEPTHBUFFER_DISABLE);
		if (cap & VL_CAPABILITY_CULL_FACE)
			grCullMode(vl_state.current_cullmode);
#endif
		
		if (cap & vl_capabilities[i].vl_parm)
#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
			glDisable(vl_capabilities[i].to_parm);
#elif defined (VL_MODE_GLIDE)
			// Hacky, but just to be safe...
			if (vl_capabilities[i].to_parm != 0)
				grDisable(vl_capabilities[i].to_parm);
#endif

		vl_state.capabilities &= ~vl_capabilities[i].vl_parm;
	}
	VIDEO_FUNCTION_END
}

/*	TODO: Want more control over the dynamics of this...
*/
void vlBlendFunc(vlBlend_t modea, vlBlend_t modeb)
{
	VIDEO_FUNCTION_START
	if (Video.debug_frame)
		plWriteLog(VIDEO_LOG, "Video: Setting blend mode (%i) (%i)\n", modea, modeb);
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	glBlendFunc(modea, modeb);
#elif defined (VL_MODE_GLIDE)
	grAlphaBlendFunction(modea, modeb, modea, modeb);
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
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	glDepthMask(mode);
#elif defined (VL_MODE_GLIDE)
	grDepthMask(mode);
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
		default:break;
		}
		Sys_Error("%s\n%s", vlGetErrorString(glerror), errorstring);
	}
#endif
	VIDEO_FUNCTION_END
}

// VERTEX ARRAY OBJECTS

/*	Generates a single vertex array.
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

/*	Sets clear colour for colour buffer.
*/
void vlSetClearColour(float r, float g, float b, float a)
{
	if ((r == vl_state.buffer_clearcolour[0]) &&
		(g == vl_state.buffer_clearcolour[1]) &&
		(b == vl_state.buffer_clearcolour[2]) &&
		(a == vl_state.buffer_clearcolour[3]))
		return;
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	glClearColor(r, g, b, a);
#endif
	plColourSetf(vl_state.buffer_clearcolour, r, g, b, a);
}

void vlSetClearColour4fv(plColour_t rgba)
{
	vlSetClearColour(rgba[0], rgba[1], rgba[2], rgba[3]);
}

/*	Clears all the buffers.
*/
void vlClearBuffers(unsigned int mask)
{
	VIDEO_FUNCTION_START
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	glClear(mask);
#elif defined (VL_MODE_GLIDE)
	// Glide only supports clearing a single buffer.
	grBufferClear(
		// Convert buffer_clearcolour to something that works with Glide.
		_vlConvertColour4fv(VL_COLOURFORMAT_RGBA, vl_state.buffer_clearcolour), 
		1, 1);
#endif
	VIDEO_FUNCTION_END
}

void vlColourMask(bool red, bool green, bool blue, bool alpha)
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	glColorMask(red, green, blue, alpha);
#elif defined (VL_MODE_GLIDE)
	bool rgb = false;
	if (red || green || blue) rgb = true;
	grColorMask(rgb, alpha);
#endif
}

void vlSwapBuffers(void)
{
#if defined (VL_MODE_OPENGL)
	// Platform library takes care of this.
	plSwapBuffers(&g_mainwindow);
#elif defined (VL_MODE_GLIDE)
	// Glide is pretty neat about this
	// and actually handles it for us.
	grBufferSwap(0);
#endif
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
void vlCheckFrameBufferStatus(VLFBOTarget_t target)
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
void vlBindFrameBuffer(VLFBOTarget_t target, unsigned int buffer)
{
	VIDEO_FUNCTION_START
#ifdef VL_MODE_OPENGL
	glBindFramebuffer(target, buffer);

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
	FOG
===========================*/

/*	Sets global colour for fog.
*/
void vlFogColour3fv(plColour_t rgba)
{
#if defined (VL_MODE_OPENGL)
	glFogfv(GL_FOG_COLOR, rgba);
#elif defined (VL_MODE_GLIDE)
	grFogColorValue(_vlConvertColour4fv(VL_COLOURFORMAT_RGBA, rgba));
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
#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
	{ VIDEO_PRIMITIVE_LINES,				GL_LINES,			"LINES" },
	{ VIDEO_PRIMITIVE_POINTS,				GL_POINTS,			"POINTS" },
	{ VIDEO_PRIMITIVE_TRIANGLES,			GL_TRIANGLES,		"TRIANGLES" },
	{ VIDEO_PRIMITIVE_TRIANGLE_FAN,			GL_TRIANGLE_FAN,	"TRIANGLE_FAN" },
	{ VIDEO_PRIMITIVE_TRIANGLE_FAN_LINE,	GL_LINES,			"TRIANGLE_FAN_LINE" },
	{ VIDEO_PRIMITIVE_TRIANGLE_STRIP,		GL_TRIANGLE_STRIP,	"TRIANGLE_STRIP" },
	{ VIDEO_PRIMITIVE_QUADS,				GL_QUADS,			"QUADS" }
#elif defined (VL_MODE_GLIDE)
	{ VIDEO_PRIMITIVE_LINES,				GR_LINES,			"LINES" },
	{ VL_PRIMITIVE_LINE_STRIP,				GR_LINE_STRIP,		"LINE_STRIP" },
	{ VIDEO_PRIMITIVE_POINTS,				GR_POINTS,			"POINTS" },
	{ VIDEO_PRIMITIVE_TRIANGLES,			GR_TRIANGLES,		"TRIANGLES" },
	{ VIDEO_PRIMITIVE_TRIANGLE_FAN,			GR_TRIANGLE_FAN,	"TRIANGLE_FAN" },
	{ VIDEO_PRIMITIVE_TRIANGLE_FAN_LINE,	GR_LINES,			"TRIANGLE_FAN_LINE" },
	{ VIDEO_PRIMITIVE_TRIANGLE_STRIP,		GR_TRIANGLE_STRIP,	"TRIANGLE_STRIP" },
	{ VIDEO_PRIMITIVE_QUADS,				0,					"QUADS" }
#elif defined (VL_MODE_DIRECT3D)
#elif defined (VL_MODE_VULKAN)
	{ VIDEO_PRIMITIVE_LINES,				VK_PRIMITIVE_TOPOLOGY_LINE_LIST,		"LINES" },
	{ VIDEO_PRIMITIVE_POINTS,				VK_PRIMITIVE_TOPOLOGY_POINT_LIST,		"POINTS" },
	{ VIDEO_PRIMITIVE_TRIANGLES,			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,	"TRIANGLES" },
	{ VIDEO_PRIMITIVE_TRIANGLE_FAN,			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,		"TRIANGLE_FAN" },
	{ VIDEO_PRIMITIVE_TRIANGLE_FAN_LINE,	VK_PRIMITIVE_TOPOLOGY_LINE_LIST,		"TRIANGLE_FAN_LINE" },
	{ VIDEO_PRIMITIVE_TRIANGLE_STRIP,		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,	"TRIANGLE_STRIP" },
	{ VIDEO_PRIMITIVE_QUADS,				0,										"QUADS" }
#else
	{ 0 }
#endif
};

unsigned int _vlTranslatePrimitiveType(VideoPrimitive_t primitive)
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

#ifdef VL_MODE_OPENGL
	glDrawArrays(_vlTranslatePrimitiveType(mode), first, count);
#endif
}

void vlDrawElements(VideoPrimitive_t mode, unsigned int count, unsigned int type, const void *indices)
{
	VIDEO_FUNCTION_START
	if ((count == 0) || !indices)
		return;

#ifdef VL_MODE_OPENGL
	glDrawElements(_vlTranslatePrimitiveType(mode), count, type, indices);
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

/*===========================
	MISC
===========================*/

void vlSetCullMode(VLCull_t mode)
{
	if (mode == vl_state.current_cullmode)
		return;

#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	glCullFace(GL_BACK);
	switch (mode)
	{
	case VL_CULL_NEGATIVE:
		glFrontFace(GL_CW);
		break;
	case VL_CULL_POSTIVE:
		glFrontFace(GL_CCW);
		break;
	}
#endif
	vl_state.current_cullmode = mode;
}

void vlFinish(void)
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	glFinish();
#elif defined (VL_MODE_GLIDE)
	grFinish();
#endif
}