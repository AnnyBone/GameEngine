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

#include "engine_base.h"

#include "video.h"

#include "platform_log.h"

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
- Simulate quads primitive type
*/

/*===========================
	INITIALIZATION
===========================*/

#if defined (VL_MODE_GLIDE)

/*	Convert RGBA colour to something glide can understand.
*/
GrColor_t _vlConvertColour4f(VLColourFormat format, float r, float g, float b, float a)
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
GrColor_t _vlConvertColour4fv(VLColourFormat format, PLColour colour)
{
	return _vlConvertColour4f(format, colour[0], colour[1], colour[2], colour[3]);
}

void _vlGlideErrorCallback(const char *string, FxBool fatal)
{
	if (fatal) Sys_Error(string);

	Con_Warning(string);
}

void _vlInitGlide(void)
{
	grGet(GR_NUM_BOARDS, sizeof(vl_state.num_cards), (FxI32*)&vl_state.num_cards);
	if (vl_state.num_cards == 0)
		Sys_Error("No Glide capable hardware detected!\n");

	// Initialize Glide.
	grGlideInit();

	grErrorSetCallback(_vlGlideErrorCallback);
}

#elif defined (VL_MODE_DIRECT3D)

IDXGISwapChain			*vl_d3d_swapchain;
ID3D11Device			*vl_d3d_device;
ID3D11DeviceContext		*vl_d3d_context;
ID3D11RasterizerState	*vl_d3d_state;

ID3D11RenderTargetView	*vl_d3d_backbuffer;

void _vlInitDirect3D(void)
{
	DXGI_SWAP_CHAIN_DESC spdesc;
	
	memset(&spdesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
	spdesc.BufferCount			= 1;
	spdesc.BufferDesc.Format	= DXGI_FORMAT_R8G8B8A8_UNORM;
	spdesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	spdesc.SampleDesc.Count		= 1;
	spdesc.SampleDesc.Quality	= 0;
	//spdesc.OutputWindow =	// todo, api call to get window handle for video layer??
	//spdesc.Windowed = // todo, api call to get window handle for video layer??
	spdesc.Flags				= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,NULL,NULL,NULL,
		D3D11_SDK_VERSION,
		&spdesc,
		&vl_d3d_swapchain,
		&vl_d3d_device,
		D3D_FEATURE_LEVEL_11_0,
		&vl_d3d_context);

	// Create and assign the backbuffer.
	ID3D11Texture2D *backbuffer;
	vl_d3d_swapchain->lpVtbl->GetBuffer(vl_d3d_swapchain, 0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);
	vl_d3d_device->lpVtbl->CreateRenderTargetView(vl_d3d_device, backbuffer, NULL, &vl_d3d_backbuffer);
	vl_d3d_context->lpVtbl->OMSetRenderTargets(vl_d3d_context, 1, &vl_d3d_backbuffer, NULL);

	backbuffer->lpVtbl->Release(backbuffer);

	D3D11_RASTERIZER_DESC rasterizerdesc;
	memset(&rasterizerdesc, 0, sizeof(D3D11_RASTERIZER_DESC));
	vl_d3d_device->lpVtbl->CreateRasterizerState(
		vl_d3d_device,
		&rasterizerdesc,
		vl_d3d_state);
}

void _vlShutdownDirect3D(void)
{
	vl_d3d_swapchain->lpVtbl->SetFullscreenState(vl_d3d_swapchain, false, NULL);

	vl_d3d_backbuffer->lpVtbl->Release(vl_d3d_backbuffer);

	vl_d3d_state->lpVtbl->Release(vl_d3d_state);
	vl_d3d_swapchain->lpVtbl->Release(vl_d3d_swapchain);
	vl_d3d_device->lpVtbl->Release(vl_d3d_device);
	vl_d3d_context->lpVtbl->Release(vl_d3d_context);
}

#endif

/*===========================
	ERROR HANDLING
===========================*/

/*	Returns a generic string describing the fault.
*/
char *vlGetErrorString(unsigned int er)
{
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
}

/*===========================
	MATRICES
===========================*/

static bool vl_matrixpushed = false;

void vlPushMatrix(void)
{
	if (vl_matrixpushed)
		return;
#ifdef VL_MODE_OPENGL
	glPushMatrix();
#endif
	vl_matrixpushed = true;
}

void vlPopMatrix(void)
{
	if (!vl_matrixpushed)
		return;
#ifdef VL_MODE_OPENGL
	glPopMatrix();
#endif
	vl_matrixpushed = false;
}

/*===========================
	SHADERS
===========================*/

void vlLinkShaderProgram(PLShaderProgram *program)
{
#ifdef VL_MODE_OPENGL
	glLinkProgram(*program);

	int status;
	glGetProgramiv(*program, GL_LINK_STATUS, &status);
	if (!status)
	{
		int length = 0;
		glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &length);
		if (length > 1)
		{
			char *cLog = (char*)calloc_or_die(sizeof(char), length);
			glGetProgramInfoLog(*program, length, NULL, cLog);
			Con_Warning("%s\n", cLog);
			free(cLog);
		}

		Sys_Error("Shader program linking failed!\nCheck log for details.\n");
	}
#endif
}

void vlAttachShader(PLShaderProgram program, PLShader shader)
{
#if defined (VL_MODE_OPENGL)
	glAttachShader(program, shader);
#endif
}

PLAttribute vlGetAttributeLocation(PLShaderProgram *program, const char *name)
{
#if defined (VL_MODE_OPENGL)
	return glGetAttribLocation(*program, name);
#else
	return 0;
#endif
}

/*===========================
	TEXTURES
===========================*/

int _vlTranslateTextureEnvironmentMode(PLTextureEnvironmentMode TextureEnvironmentMode)
{
	switch (TextureEnvironmentMode)
	{
#ifdef VL_MODE_OPENGL
	case PL_TEXTUREMODE_ADD:
		return GL_ADD;
	case PL_TEXTUREMODE_MODULATE:
		return GL_MODULATE;
	case PL_TEXTUREMODE_DECAL:
		return GL_DECAL;
	case PL_TEXTUREMODE_BLEND:
		return GL_BLEND;
	case PL_TEXTUREMODE_REPLACE:
		return GL_REPLACE;
	case PL_TEXTUREMODE_COMBINE:
		return GL_COMBINE;
#endif
	default:
		Sys_Error("Unknown texture environment mode! (%i)\n", TextureEnvironmentMode);
	}

	// Won't be hit but meh, compiler will complain otherwise.
	return 0;
}

void vlSetTextureEnvironmentMode(PLTextureEnvironmentMode TextureEnvironmentMode)
{
	// Ensure there's actually been a change.
	if (Video.textureunits[Video.current_textureunit].current_envmode == TextureEnvironmentMode)
		return;

#ifdef VL_MODE_OPENGL
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, 
		_vlTranslateTextureEnvironmentMode(TextureEnvironmentMode));
#endif

	Video.textureunits[Video.current_textureunit].current_envmode = TextureEnvironmentMode;
}

/*===========================
	CAPABILITIES
===========================*/

/*	Enable or disable writing into the depth buffer.
*/
void vlDepthMask(bool mode)
{
	static bool cur_state = true;
	if (mode == cur_state) return;
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	glDepthMask(mode);
#elif defined (VL_MODE_GLIDE)
	grDepthMask(mode);
#endif
	cur_state = mode;
}

/*===========================
	OBJECTS
===========================*/

// RENDER BUFFER OBJECTS

void vlGenerateRenderBuffer(PLRenderBuffer *buffer)
{
#ifdef VL_MODE_OPENGL
	glGenRenderbuffers(1, buffer);
#endif
}

void vlDeleteRenderBuffer(PLRenderBuffer *buffer)
{
#ifdef VL_MODE_OPENGL
	glDeleteRenderbuffers(1, buffer);
#endif
}

void vlBindRenderBuffer(PLRenderBuffer buffer)
{
#ifdef VL_MODE_OPENGL
	glBindRenderbuffer(GL_RENDERBUFFER, buffer);
#endif
}

void vlRenderBufferStorage(int format, int samples, unsigned int width, unsigned int height)
{
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
}

// FRAME BUFFER OBJECTS

void vlScissor(int x, int y, unsigned int width, unsigned int height)
{
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	glScissor(x, y, width, height);
#elif defined (VL_MODE_DIRECT3D)
	D3D11_RECT scissor_region;
	memset(&scissor_region, 0, sizeof(D3D11_RECT));
	scissor_region.bottom	= height;
	scissor_region.right	= width;
	vl_d3d_context->lpVtbl->RSSetScissorRects(vl_d3d_context, 0, &scissor_region);
#endif
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
#elif defined (VL_MODE_DIRECT3D)
	vl_d3d_swapchain->lpVtbl->Present(vl_d3d_swapchain,	0, 0);
#endif
}

/*	Generates a single framebuffer.
*/
void vlGenerateFrameBuffer(PLFrameBuffer *buffer)
{
#ifdef VL_MODE_OPENGL
	glGenFramebuffers(1, buffer);
#elif defined (VL_MODE_GLIDE)
	// No support for this.
#elif defined (VL_MODE_DIRECT3D)
#endif
}

/*	Ensures that the framebuffer is valid, otherwise throws an error.
	glCheckFramebufferStatus
*/
void vlCheckFrameBufferStatus(PLFBOTarget target)
{
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
}

/*	Binds the given framebuffer.
*/
void vlBindFrameBuffer(PLFBOTarget target, unsigned int buffer)
{
#if defined (VL_MODE_OPENGL)
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
}

/*	Deletes the given framebuffer.
*/
void vlDeleteFrameBuffer(unsigned int *uiBuffer)
{
#ifdef VL_MODE_OPENGL
	glDeleteFramebuffers(1, uiBuffer);
#endif
}

void vlAttachFrameBufferRenderBuffer(unsigned int attachment, unsigned int buffer)
{
#ifdef VL_MODE_OPENGL
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, buffer);
#endif
}

void vlAttachFrameBufferTexture(gltexture_t *buffer)
{
#ifdef VL_MODE_OPENGL
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer->texnum, 0);
#endif
}

/*===========================
	FOG
===========================*/

/*	Sets global colour for fog.
*/
void vlFogColour3fv(PLColour rgba)
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

#define	_VL_BUFFER_VERTICES		0
#define _VL_BUFFER_INDICES		1
#define _VL_BUFFER_TEXCOORDS	3

/*	Generates each of the buffers and other
	data necessary for the draw call.
*/
PLDraw *vlCreateDraw(PLPrimitive primitive, uint32_t num_tris, uint32_t num_verts)
{
	if ((primitive == VL_PRIMITIVE_IGNORE) || (primitive >= VL_PRIMITIVE_END))
		Sys_Error("Invalid primitive for draw object!\n");
	else if ((num_tris == 0) && (primitive == VL_PRIMITIVE_TRIANGLES))
		Sys_Error("Invalid number of triangles!\n");

	PLDraw *_draw = (PLDraw*)calloc_or_die(sizeof(PLDraw), 1);
	memset(_draw, 0, sizeof(PLDraw));
	_draw->primitive			= primitive;
	_draw->primitive_restore	= primitive;

	_draw->vertices = (PLVertex*)calloc_or_die(sizeof(PLDraw), num_verts);
	memset(_draw->vertices, 0, sizeof(PLVertex));
	_draw->numverts			= num_verts;
	_draw->numtriangles		= num_tris;

	if (primitive == VL_PRIMITIVE_TRIANGLES)
	{
		_draw->indices = (uint8_t*)calloc_or_die(_draw->numtriangles * 3, sizeof(uint8_t));
		memset(_draw->indices, 0, sizeof(uint8_t));
	}

#if defined (VL_MODE_OPENGL)
	glGenBuffers(sizeof(_draw->_gl_vbo), _draw->_gl_vbo);
#endif

	return _draw;
}

void vlDeleteDraw(PLDraw *draw)
{
	if (!draw)
		return;

#if defined (VL_MODE_OPENGL)
	glDeleteBuffers(sizeof(draw->_gl_vbo), draw->_gl_vbo);
#endif

	if (draw->vertices)	free(draw->vertices);
	if (draw->indices)	free(draw->indices);
	free(draw);

	draw = NULL;
}

PLVertex *vl_draw_vertex = NULL;

void vlBeginDraw(PLDraw *draw)
{
	if (!draw)
    {
        Sys_Error("Passed invalid draw object to vlBeginDraw!\n");
        return;
    }

	memset(draw->vertices, 0, sizeof(draw->vertices));
	vl_draw_vertex = &draw->vertices[0];
}

void vlDrawVertex3f(float x, float y, float z)
{
	plVectorSet3f(vl_draw_vertex->position, x, y, z);
	plColourSetf(vl_draw_vertex->colour, 1, 1, 1, 1);

	vl_draw_vertex++;
}

void vlDrawVertex3fv(plVector3f_t position)
{
	plVectorCopy(position, vl_draw_vertex->position);
	plColourSetf(vl_draw_vertex->colour, 1, 1, 1, 1);

	vl_draw_vertex++;
}

void vlDrawColour4f(float r, float g, float b, float a)
{
	plColourSetf(vl_draw_vertex->colour, r, g, b, a);
}

void vlDrawColour4fv(PLColour rgba)
{
	plVectorCopy(rgba, vl_draw_vertex->colour);
}

void vlDrawNormal3fv(plVector3f_t position)
{
	plVectorCopy(position, vl_draw_vertex->normal);
}

void vlDrawTexCoord2f(unsigned int target, float s, float t)
{
	plVector2Set2f(vl_draw_vertex->ST[target], s, t);
}

void vlEndDraw(PLDraw *draw)
{
	if (!draw)
		Sys_Error("Passed invalid draw object to vlBeginDraw!\n");

#if defined (VL_MODE_OPENGL)
	glBindBuffer(GL_ARRAY_BUFFER, draw->_gl_vbo[_VL_BUFFER_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, draw->numverts * sizeof(plVector3f_t) * sizeof(PLVertex), draw->vertices->position, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, draw->_gl_vbo[_VL_BUFFER_TEXCOORDS]);
	glBufferData(GL_ARRAY_BUFFER, draw->numverts * sizeof(plVector2f_t) * sizeof(PLVertex), draw->vertices->ST, GL_DYNAMIC_DRAW);

	if (draw->primitive == VL_PRIMITIVE_TRIANGLES)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw->_gl_vbo[_VL_BUFFER_INDICES]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, draw->numtriangles * 3 * sizeof(uint8_t), draw->indices, GL_DYNAMIC_DRAW);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif

	vl_draw_vertex = NULL;
}