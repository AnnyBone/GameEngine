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

typedef struct VLState
{
	unsigned int num_cards;		// Number of video cards.

	VLCullMode	current_cullmode;

	plColour_t		current_clearcolour;
	plColour_t		current_colour;			// Current global colour.

	unsigned int	current_capabilities;	// Enabled capabilities.
	VLTexture		current_texture;

	// Shader state.
	vlShaderProgram_t	current_program;

	// Hardware / Driver information

	const PLchar *hw_vendor;
	const PLchar *hw_renderer;
	const PLchar *hw_version;
	const PLchar *hw_extensions;

	PLint hw_maxtexturesize;

	////////////////////////////////////////

	PLint viewport_x, viewport_y;
	PLuint viewport_width, viewport_height;

	PLbool mode_debug;
} VLState;

VLState vl_state;

/*	TODO:
- Add somewhere we can store tracking
data for each of these functions
- Do this in another thread if possible
- Display that data as an overlay
*/
#define	_VL_UTIL_TRACK(name)									\
	{															\
		unsigned static int _t = 0;								\
		if(vl_state.mode_debug)									\
		{														\
			plWriteLog("pl_video_layer_log", " "#name"\n");		\
			_t++;												\
		}														\
	}

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
GrColor_t _vlConvertColour4fv(VLColourFormat format, plColour_t colour)
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

#elif defined (VL_MODE_OPENGL)

PLbool vl_gl_generate_mipmap			= false;
PLbool vl_gl_depth_texture				= false;
PLbool vl_gl_shadow						= false;
PLbool vl_gl_vertex_buffer_object		= false;
PLbool vl_gl_texture_compression		= false;
PLbool vl_gl_texture_compression_s3tc	= false;

PLuint vl_gl_version_major = 0;
PLuint vl_gl_version_minor = 0;

#define VL_GL_VERSION(maj, min)	((maj == vl_gl_version_major && min <= vl_gl_version_minor) || maj < vl_gl_version_major)

void _vlInitOpenGL(void)
{
	unsigned int err = glewInit();
	if (err != GLEW_OK)
		Sys_Error("Failed to initialize glew!\n%s\n", glewGetErrorString(err));

	// Check that the required capabilities are supported.
	if (!GLEW_ARB_multitexture) Sys_Error("Video hardware incapable of multi-texturing!\n");
	else if (!GLEW_ARB_texture_env_combine && !GLEW_EXT_texture_env_combine) Sys_Error("ARB/EXT_texture_env_combine isn't supported by your hardware!\n");
	else if (!GLEW_ARB_texture_env_add && !GLEW_EXT_texture_env_add) Sys_Error("ARB/EXT_texture_env_add isn't supported by your hardware!\n");
	//else if (!GLEE_EXT_fog_coord) Sys_Error("EXT_fog_coord isn't supported by your hardware!\n");
	else if (!GLEW_ARB_vertex_program || !GLEW_ARB_fragment_program) Sys_Error("Shaders aren't supported by this hardware!\n");

	// Optional capabilities...

	if (GLEW_SGIS_generate_mipmap) vl_gl_generate_mipmap = true;
	else Con_Warning("Hardware mipmap generation isn't supported!\n");
	if (GLEW_ARB_depth_texture) vl_gl_depth_texture = true;
	else Con_Warning("ARB_depth_texture isn't supported by your hardware!\n");
	if (GLEW_ARB_shadow) vl_gl_shadow = true;
	else Con_Warning("ARB_shadow isn't supported by your hardware!\n");
	if (GLEW_ARB_vertex_buffer_object) vl_gl_vertex_buffer_object = true;
	else Con_Warning("Hardware doesn't support Vertex Buffer Objects!\n");

	if (GLEW_ARB_texture_compression)
	{
		if (GLEW_EXT_texture_compression_s3tc) vl_gl_texture_compression_s3tc = true;
	}

	const char *version = vlGetString(VL_STRING_VERSION);
	vl_gl_version_major = (unsigned int)atoi(&version[0]);
	vl_gl_version_minor = (unsigned int)atoi(&version[2]);
}

void _vlShutdownOpenGL(void)
{
	
}

#endif

/*	Function used for initialization in general.
*/
void vlInit(void)
{
	_VL_UTIL_TRACK(vlInit);

	Con_Printf("Initializing Video Abstraction Layer...\n");
	
	memset(&vl_state, 0, sizeof(vl_state));

#if defined (VL_MODE_OPENGL)
	_vlInitOpenGL();
#elif defined (VL_MODE_GLIDE)
	_vlInitGlide();
#elif defined (VL_MODE_DIRECT3D)
	_vlInitDirect3D();
#endif

	// Get any information that will be presented later.
	vl_state.hw_extensions	= vlGetString(VL_STRING_EXTENSIONS);
	vl_state.hw_renderer	= vlGetString(VL_STRING_RENDERER);
	vl_state.hw_vendor		= vlGetString(VL_STRING_VENDOR);
	vl_state.hw_version		= vlGetString(VL_STRING_VERSION);
	Con_Printf(" HARDWARE/DRIVER INFORMATION\n");
	Con_Printf("  RENDERER: %s\n", vl_state.hw_renderer);
	Con_Printf("  VENDOR:   %s\n", vl_state.hw_vendor);
	Con_Printf("  VERSION:  %s\n\n", vl_state.hw_version);
}

void vlShutdown(void)
{
	_VL_UTIL_TRACK(vlShutdown);

#if defined (VL_MODE_OPENGL)
#elif defined (VL_MODE_GLIDE)
#elif defined (VL_MODE_DIRECT3D)
	_vlShutdownDirect3D();
#endif
}

/*===========================
	GET
===========================*/

/*	Returns supported num of texture width.
*/
int vlGetMaxTextureSize(void)
{
	_VL_UTIL_TRACK(vlGetMaxTextureSize);

	if (vl_state.hw_maxtexturesize != 0)
		return vl_state.hw_maxtexturesize;

#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &vl_state.hw_maxtexturesize);
#elif defined (VL_MODE_GLIDE)
	grGet(GR_MAX_TEXTURE_SIZE, sizeof(vl_state.hw_maxtexturesize), &vl_state.hw_maxtexturesize);
#endif

	return vl_state.hw_maxtexturesize;
}

void vlGetMaxTextureImageUnits(int *param)
{
	_VL_UTIL_TRACK(vlGetMaxTextureImageUnits);

#ifdef VL_MODE_OPENGL
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, param);
#elif defined (VL_MODE_GLIDE)
	grGet(GR_NUM_TMU, sizeof(param), (FxI32*)param);
#else
	param = 0;
#endif
}

void vlGetMaxTextureAnistropy(float *params)
{
	_VL_UTIL_TRACK(vlGetMaxTextureAnistropy);

#ifdef VL_MODE_OPENGL
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, params);
#else	// Not supported in core?
	params = 0;
#endif
}

const char *vlGetExtensions(void)
{
	_VL_UTIL_TRACK(vlGetExtensions);

#if defined(VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	return (const char*)glGetString(GL_EXTENSIONS);
	// TODO: this works differently in core; use glGetStringi instead!
#elif defined (VL_MODE_GLIDE)
	return grGetString(GR_EXTENSION);
#else
	return "";
#endif
}

const char *vlGetString(VLString string)
{
	_VL_UTIL_TRACK(vlGetString);

	switch (string)
	{
	case VL_STRING_EXTENSIONS:
		return vlGetExtensions();
	default:
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
		return (const char*)glGetString(string);
#elif defined (VL_MODE_GLIDE)
		return grGetString(string);
#endif
	}
}

/*===========================
	ERROR HANDLING
===========================*/

/*	Returns a generic string describing the fault.
*/
char *vlGetErrorString(unsigned int er)
{
	_VL_UTIL_TRACK(vlGetErrorString);

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
	_VL_UTIL_TRACK(vlPushMatrix);

	if (vl_matrixpushed)
		return;
#ifdef VL_MODE_OPENGL
	glPushMatrix();
#endif
	vl_matrixpushed = true;
}

void vlPopMatrix(void)
{
	_VL_UTIL_TRACK(vlPopMatrix);

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

vlShaderProgram_t vlCreateShaderProgram(void)
{
	_VL_UTIL_TRACK(vlCreateShaderProgram);

#ifdef VL_MODE_OPENGL
	return glCreateProgram();
#endif
}

vlShaderProgram_t vlGetCurrentShaderProgram(void)
{
	_VL_UTIL_TRACK(vlGetCurrentShaderProgram);
	return vl_state.current_program;
}

void vlDeleteShaderProgram(vlShaderProgram_t *program)
{
	_VL_UTIL_TRACK(vlDeleteShaderProgram);

#ifdef VL_MODE_OPENGL
	glDeleteProgram(*program);
	program = NULL;
#endif
}

void vlUseShaderProgram(vlShaderProgram_t program)
{
	_VL_UTIL_TRACK(vlUseShaderProgram);

	if (program == vl_state.current_program)
		return;
#ifdef VL_MODE_OPENGL
	glUseProgram(program);
#endif
	vl_state.current_program = program;
}

void vlLinkShaderProgram(vlShaderProgram_t *program)
{
	_VL_UTIL_TRACK(vlLinkShaderProgram);

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

void vlAttachShader(vlShaderProgram_t program, vlShader_t shader)
{
	_VL_UTIL_TRACK(vlAttachShader);

#if defined (VL_MODE_OPENGL)
	glAttachShader(program, shader);
#endif
}

void vlDeleteShader(vlShader_t *shader)
{
	_VL_UTIL_TRACK(vlDeleteShader);

#if defined (VL_MODE_OPENGL)
	glDeleteShader(*shader);
#endif
	shader = NULL;
}

vlAttribute_t vlGetAttributeLocation(vlShaderProgram_t *program, const char *name)
{
	_VL_UTIL_TRACK(vlGetAttributeLocation);

#if defined (VL_MODE_OPENGL)
	return glGetAttribLocation(*program, name);
#else
	return 0;
#endif
}

/*===========================
	TEXTURES
===========================*/

VLTexture vlGenerateTexture(void)
{
#if defined (VL_MODE_OPENGL)
	PLuint id;
	glGenTextures(1, &id);
	return id;
#endif
}

VLTexture vlGetCurrentTexture(void)
{
	return vl_state.current_texture;
}

void vlDeleteTexture(VLTexture *texture)
{
#if defined (VL_MODE_OPENGL)
	glDeleteTextures(1, texture);
#endif
}

bool vlIsEnabled(unsigned int caps);

void vlUploadTexture(VLTexture texture, const VLTextureInfo *upload)
{
	vlBindTexture(VL_TEXTURE_2D, texture);

	PLuint storage_type = upload->storage_type;
	if (storage_type == 0) storage_type = GL_UNSIGNED_BYTE;

#if defined (VL_MODE_OPENGL_CORE)
#elif defined (VL_MODE_OPENGL)
	PLuint levels = upload->levels;
	if (vlIsEnabled(VL_CAPABILITY_GENERATEMIPMAP))
	{
		if(levels <= 1) levels = 4;
		if (!VL_GL_VERSION(3,0) && VL_GL_VERSION(1,4))
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levels);
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		}
	}

	PLbool compressed = false;
	// Check the format, to see if we're getting a compressed
	// format type.
	switch (upload->format)
	{
	case VL_TEXTUREFORMAT_RGBA_DXT1:
	case VL_TEXTUREFORMAT_RGBA_DXT3:
	case VL_TEXTUREFORMAT_RGBA_DXT5:
	case VL_TEXTUREFORMAT_RGB_DXT1:
	case VL_TEXTUREFORMAT_RGB_FXT1:
		compressed = true;
	default:
		break;
	}

	if(upload->initial) 
		glTexStorage2D(GL_TEXTURE_2D, levels, upload->format, upload->width, upload->height);

	if(compressed)
		glCompressedTexSubImage2D(
			GL_TEXTURE_2D, 
			0, 
			upload->x, upload->y, 
			upload->width, upload->height,
			upload->format, 
			upload->size, 
			upload->data
		);
	else
		glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			upload->x, upload->y,
			upload->width, upload->height,
			upload->pixel_format,
			storage_type,
			upload->data
		);

	if (vlIsEnabled(VL_CAPABILITY_GENERATEMIPMAP))
	{
		if (VL_GL_VERSION(3,0))
			glGenerateMipmap(GL_TEXTURE_2D);
		else if(VL_GL_VERSION(1,4))
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		else
		{
			// todo, do it in sw
		}
	}
#endif
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
	return 0;
#endif
}

/*	Selects the current active TMU.
*/
void vlActiveTexture(unsigned int texunit)
{
	_VL_UTIL_TRACK(vlActiveTexture);

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
}

void vlBindTexture(PLuint target, VLTexture texture)
{
	_VL_UTIL_TRACK(vlBindTexture);

	if (texture == vl_state.current_texture)
		return;

#if defined (VL_MODE_OPENGL)
	glBindTexture(target, texture);
#endif

	vl_state.current_texture = texture;
}

void vlSetTextureFilter(VLTexture texture, VLTextureFilter filter)
{
	_VL_UTIL_TRACK(vlSetTextureFilter);

	vlBindTexture(VL_TEXTURE_2D, texture);

#ifdef VL_MODE_OPENGL
	PLuint filtermin = filter, filtermax = filter;
	switch (filter)
	{
	case VL_TEXTUREFILTER_MIPMAP_LINEAR:
		filtermax = GL_LINEAR; filtermin = GL_LINEAR_MIPMAP_LINEAR;
		break;
	case VL_TEXTUREFILTER_MIPMAP_NEAREST:
		filtermax = GL_NEAREST; filtermin = GL_NEAREST_MIPMAP_NEAREST;
		break;
	case VL_TEXTUREFILTER_MIPMAP_LINEAR_NEAREST:
		filtermax = GL_LINEAR; filtermin = GL_LINEAR_MIPMAP_NEAREST;
		break;
	case VL_TEXTUREFILTER_MIPMAP_NEAREST_LINEAR:
		filtermax = GL_NEAREST; filtermin = GL_NEAREST_MIPMAP_LINEAR;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtermin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtermax);
#endif
}

void vlSetTextureAnisotropy(VLTexture texture, PLfloat amount)
{
	vlBindTexture(VL_TEXTURE_2D, texture);

#if defined (VL_MODE_OPENGL)
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);
#endif
}

int _vlTranslateTextureEnvironmentMode(VLTextureEnvironmentMode TextureEnvironmentMode)
{
	_VL_UTIL_TRACK(_vlTranslateTextureEnvironmentMode);

	switch (TextureEnvironmentMode)
	{
#ifdef VL_MODE_OPENGL
	case VIDEO_TEXTUREMODE_ADD:
		return GL_ADD;
	case VIDEO_TEXTUREMODE_MODULATE:
		return GL_MODULATE;
	case VIDEO_TEXTUREMODE_DECAL:
		return GL_DECAL;
	case VIDEO_TEXTUREMODE_BLEND:
		return GL_BLEND;
	case VIDEO_TEXTUREMODE_REPLACE:
		return GL_REPLACE;
	case VIDEO_TEXTUREMODE_COMBINE:
		return GL_COMBINE;
#endif
	default:
		Sys_Error("Unknown texture environment mode! (%i)\n", TextureEnvironmentMode);
	}

	// Won't be hit but meh, compiler will complain otherwise.
	return 0;
}

void vlSetTextureEnvironmentMode(VLTextureEnvironmentMode TextureEnvironmentMode)
{
	_VL_UTIL_TRACK(vlSetTextureEnvironmentMode);

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

typedef struct vlCapabilities_s
{
	unsigned int vl_parm, to_parm;

	const char *ident;
} vlCapabilities_t;

vlCapabilities_t vl_capabilities[] =
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
	{ VL_CAPABILITY_SCISSOR_TEST, GL_SCISSOR_TEST, "SCISSOR_TEST" },

	{ VL_CAPABILITY_GENERATEMIPMAP, NULL, "GENERATE_MIPMAP" },
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
	_VL_UTIL_TRACK(vlIsEnabled);

	if (!caps)
		return false;

	for (int i = 0; i < sizeof(vl_capabilities); i++)
	{
		if (!vl_capabilities[i].vl_parm)
			break;

		if (caps & vl_state.current_capabilities)
			return true;
	}

	return false;
}

/*	Enables video capabilities.
*/
void vlEnable(unsigned int cap)
{
	_VL_UTIL_TRACK(vlEnable);

	for (unsigned int i = 0; i < sizeof(vl_capabilities); i++)
	{
		// Check if we reached the end of the list yet.
		if (!vl_capabilities[i].vl_parm)
			break;
		
		if (vl_state.mode_debug)
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

		if ((cap & vl_capabilities[i].vl_parm) && (vl_capabilities[i].to_parm != 0))
#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
			glEnable(vl_capabilities[i].to_parm);
#elif defined (VL_MODE_GLIDE)
			grEnable(vl_capabilities[i].to_parm);
#endif

		vl_state.current_capabilities |= vl_capabilities[i].vl_parm;
	}
}

void vlDisable(unsigned int cap)
{
	_VL_UTIL_TRACK(vlDisable);

	if (!cap)
		return;

	for (unsigned int i = 0; i < sizeof(vl_capabilities); i++)
	{
		// Check if we reached the end of the list yet.
		if (!vl_capabilities[i].vl_parm)
			break;

		if (vl_state.mode_debug)
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

		vl_state.current_capabilities &= ~vl_capabilities[i].vl_parm;
	}
}

/*	TODO: Want more control over the dynamics of this...
*/
void vlBlendFunc(VLBlend modea, VLBlend modeb)
{
	_VL_UTIL_TRACK(vlBlendFunc);

	if (vl_state.mode_debug)
		plWriteLog(VIDEO_LOG, "Video: Setting blend mode (%i) (%i)\n", modea, modeb);
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	glBlendFunc(modea, modeb);
#elif defined (VL_MODE_GLIDE)
	grAlphaBlendFunction(modea, modeb, modea, modeb);
#endif
}

/*	Enable or disable writing into the depth buffer.
*/
void vlDepthMask(bool mode)
{
	_VL_UTIL_TRACK(vlDepthMask);

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

void vlGenerateRenderBuffer(VLRenderBuffer *buffer)
{
	_VL_UTIL_TRACK(vlGenerateRenderBuffer);

#ifdef VL_MODE_OPENGL
	glGenRenderbuffers(1, buffer);
#endif
}

void vlDeleteRenderBuffer(VLRenderBuffer *buffer)
{
#ifdef VL_MODE_OPENGL
	glDeleteRenderbuffers(1, buffer);
#endif
}

void vlBindRenderBuffer(VLRenderBuffer buffer)
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

/*	Sets clear colour for colour buffer.
*/
void vlSetClearColour4f(float r, float g, float b, float a)
{
	_VL_UTIL_TRACK(vlSetClearColour4f);

	if ((r == vl_state.current_clearcolour[0]) &&
		(g == vl_state.current_clearcolour[1]) &&
		(b == vl_state.current_clearcolour[2]) &&
		(a == vl_state.current_clearcolour[3]))
		return;
#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	glClearColor(r, g, b, a);
#elif defined (VL_MODE_DIRECT3D)
	// Don't need to do anything specific here, colour is set on clear call.
#endif
	plColourSetf(vl_state.current_clearcolour, r, g, b, a);
}

void vlSetClearColour4fv(plColour_t rgba)
{
	vlSetClearColour4f(rgba[0], rgba[1], rgba[2], rgba[3]);
}

void vlSetClearColour3f(float r, float g, float b)
{
	vlSetClearColour4f(r, g, b, vl_state.current_clearcolour[3]);
}

/*	Clears all the buffers.
*/
void vlClearBuffers(unsigned int mask)
{
	_VL_UTIL_TRACK(vlClearBuffers);

#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	glClear(mask);
#elif defined (VL_MODE_GLIDE)
	// Glide only supports clearing a single buffer.
	grBufferClear(
		// Convert buffer_clearcolour to something that works with Glide.
		_vlConvertColour4fv(VL_COLOURFORMAT_RGBA, vl_state.buffer_clearcolour), 
		1, 1);
#elif defined (VL_MODE_DIRECT3D)
	vl_d3d_context->lpVtbl->ClearRenderTargetView(vl_d3d_context, 
		vl_d3d_backbuffer, 
		vl_state.current_clearcolour
		);
#endif
}

void vlScissor(int x, int y, unsigned int width, unsigned int height)
{
	_VL_UTIL_TRACK(vlScissor);

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
	_VL_UTIL_TRACK(vlColourMask);

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
void vlGenerateFrameBuffer(VLFrameBuffer *buffer)
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
void vlCheckFrameBufferStatus(vlFBOTarget_t target)
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
void vlBindFrameBuffer(vlFBOTarget_t target, unsigned int buffer)
{
	_VL_UTIL_TRACK(vlBindFrameBuffer);
	
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
	VIDEO_FUNCTION_END
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
void vlFogColour3fv(plColour_t rgba)
{
	_VL_UTIL_TRACK(vlFogColour3fv);

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
vlDraw_t *vlCreateDraw(VLPrimitive primitive, uint32_t num_tris, uint32_t num_verts)
{
	_VL_UTIL_TRACK(vlCreateDraw);

	if ((primitive == VL_PRIMITIVE_IGNORE) || (primitive >= VL_PRIMITIVE_END))
		Sys_Error("Invalid primitive for draw object!\n");
	else if ((num_tris == 0) && (primitive == VL_PRIMITIVE_TRIANGLES))
		Sys_Error("Invalid number of triangles!\n");

	vlDraw_t *_draw = (vlDraw_t*)calloc_or_die(sizeof(vlDraw_t), 1);
	memset(_draw, 0, sizeof(vlDraw_t));
	_draw->primitive			= primitive;
	_draw->primitive_restore	= primitive;

	_draw->vertices = (vlVertex_t*)calloc_or_die(sizeof(vlDraw_t), num_verts);
	memset(_draw->vertices, 0, sizeof(vlVertex_t));
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

void vlDeleteDraw(vlDraw_t *draw)
{
	_VL_UTIL_TRACK(vlDeleteDraw);

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

vlVertex_t *vl_draw_vertex = NULL;

void vlBeginDraw(vlDraw_t *draw)
{
	_VL_UTIL_TRACK(vlBeginDraw);

	if (!draw)
		Sys_Error("Passed invalid draw object to vlBeginDraw!\n");

	memset(draw->vertices, 0, sizeof(draw->vertices));
	vl_draw_vertex = &draw->vertices[0];
}

void vlDrawVertex3f(float x, float y, float z)
{
	_VL_UTIL_TRACK(vlDrawVertex3f);

	plVectorSet3f(vl_draw_vertex->position, x, y, z);
	plColourSetf(vl_draw_vertex->colour, 1, 1, 1, 1);

	vl_draw_vertex++;
}

void vlDrawVertex3fv(plVector3f_t position)
{
	_VL_UTIL_TRACK(vlDrawVertex3fv);

	plVectorCopy(position, vl_draw_vertex->position);
	plColourSetf(vl_draw_vertex->colour, 1, 1, 1, 1);

	vl_draw_vertex++;
}

void vlDrawColour4f(float r, float g, float b, float a)
{
	_VL_UTIL_TRACK(vlDrawColour4f);

	plColourSetf(vl_draw_vertex->colour, r, g, b, a);
}

void vlDrawColour4fv(plColour_t rgba)
{
	_VL_UTIL_TRACK(vlDrawColour4fv);

	plVectorCopy(rgba, vl_draw_vertex->colour);
}

void vlDrawNormal3fv(plVector3f_t position)
{
	_VL_UTIL_TRACK(vlDrawNormal3fv);

	plVectorCopy(position, vl_draw_vertex->normal);
}

void vlDrawTexCoord2f(unsigned int target, float s, float t)
{
	plVector2Set2f(vl_draw_vertex->ST[target], s, t);
}

void vlEndDraw(vlDraw_t *draw)
{
	if (!draw)
		Sys_Error("Passed invalid draw object to vlBeginDraw!\n");

#if defined (VL_MODE_OPENGL)
	glBindBuffer(GL_ARRAY_BUFFER, draw->_gl_vbo[_VL_BUFFER_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, draw->numverts * sizeof(plVector3f_t) * sizeof(vlVertex_t), draw->vertices->position, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, draw->_gl_vbo[_VL_BUFFER_TEXCOORDS]);
	glBufferData(GL_ARRAY_BUFFER, draw->numverts * sizeof(plVector2f_t) * sizeof(vlVertex_t), draw->vertices->ST, GL_DYNAMIC_DRAW);

	if (draw->primitive == VL_PRIMITIVE_TRIANGLES)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw->_gl_vbo[_VL_BUFFER_INDICES]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, draw->numtriangles * 3 * sizeof(uint8_t), draw->indices, GL_DYNAMIC_DRAW);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif

	vl_draw_vertex = NULL;
}

//--

typedef struct _vlPrimitiveTranslate_s
{
	VLPrimitive primitive;

	unsigned int gl;

	const char *name;
} _vlPrimitiveTranslate_t;

_vlPrimitiveTranslate_t vl_primitives[] =
{
#if defined (VL_MODE_OPENGL) || (VL_MODE_OPENGL_CORE)
	{ VL_PRIMITIVE_LINES,					GL_LINES,			"LINES" },
	{ VL_PRIMITIVE_POINTS,					GL_POINTS,			"POINTS" },
	{ VL_PRIMITIVE_TRIANGLES,				GL_TRIANGLES,		"TRIANGLES" },
	{ VL_PRIMITIVE_TRIANGLE_FAN,			GL_TRIANGLE_FAN,	"TRIANGLE_FAN" },
	{ VL_PRIMITIVE_TRIANGLE_FAN_LINE,		GL_LINES,			"TRIANGLE_FAN_LINE" },
	{ VL_PRIMITIVE_TRIANGLE_STRIP,			GL_TRIANGLE_STRIP,	"TRIANGLE_STRIP" },
	{ VL_PRIMITIVE_QUADS,					GL_QUADS,			"QUADS" }
#elif defined (VL_MODE_GLIDE)
	{ VL_PRIMITIVE_LINES,					GR_LINES,			"LINES" },
	{ VL_PRIMITIVE_LINE_STRIP,				GR_LINE_STRIP,		"LINE_STRIP" },
	{ VL_PRIMITIVE_POINTS,					GR_POINTS,			"POINTS" },
	{ VL_PRIMITIVE_TRIANGLES,				GR_TRIANGLES,		"TRIANGLES" },
	{ VL_PRIMITIVE_TRIANGLE_FAN,			GR_TRIANGLE_FAN,	"TRIANGLE_FAN" },
	{ VL_PRIMITIVE_TRIANGLE_FAN_LINE,		GR_LINES,			"TRIANGLE_FAN_LINE" },
	{ VL_PRIMITIVE_TRIANGLE_STRIP,			GR_TRIANGLE_STRIP,	"TRIANGLE_STRIP" },
	{ VL_PRIMITIVE_QUADS,					0,					"QUADS" }
#elif defined (VL_MODE_DIRECT3D)
#elif defined (VL_MODE_VULKAN)
	{ VL_PRIMITIVE_LINES,					VK_PRIMITIVE_TOPOLOGY_LINE_LIST,		"LINES" },
	{ VL_PRIMITIVE_POINTS,					VK_PRIMITIVE_TOPOLOGY_POINT_LIST,		"POINTS" },
	{ VL_PRIMITIVE_TRIANGLES,				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,	"TRIANGLES" },
	{ VL_PRIMITIVE_TRIANGLE_FAN,			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,		"TRIANGLE_FAN" },
	{ VL_PRIMITIVE_TRIANGLE_FAN_LINE,		VK_PRIMITIVE_TOPOLOGY_LINE_LIST,		"TRIANGLE_FAN_LINE" },
	{ VL_PRIMITIVE_TRIANGLE_STRIP,			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,	"TRIANGLE_STRIP" },
	{ VL_PRIMITIVE_QUADS,					0,										"QUADS" }
#else
	{ 0 }
#endif
};

unsigned int _vlTranslatePrimitiveMode(VLPrimitive primitive)
{
	_VL_UTIL_TRACK(_vlTranslatePrimitiveMode);

	for (int i = 0; i < plArrayElements(vl_primitives); i++)
		if (primitive == vl_primitives[i].primitive)
			return vl_primitives[i].gl;

	// Hacky, but just return initial otherwise.
	return vl_primitives[0].gl;
}

/*	Deals with tris view and different primitive types, then finally draws
	the given arrays.
*/
void _vlDrawArrays(VLPrimitive mode, unsigned int first, unsigned int count)
{
	_VL_UTIL_TRACK(_vlDrawArrays);

	if (count == 0) return;
	// Ensure that first isn't going to kill us.
	else if (first >= count) first = 0;

#ifdef VL_MODE_OPENGL
	glDrawArrays(_vlTranslatePrimitiveMode(mode), first, count);
#endif
}

void _vlDrawElements(VLPrimitive mode, unsigned int count, unsigned int type, const void *indices)
{
	_VL_UTIL_TRACK(_vlDrawElements);

	if ((count == 0) || !indices) return;
#ifdef VL_MODE_OPENGL
	glDrawElements(_vlTranslatePrimitiveMode(mode), count, type, indices);
#endif
}

void vlDraw(vlDraw_t *draw)
{
	_VL_UTIL_TRACK(vlDraw);

	if(draw->numverts == 0)
		return;

#if defined (VL_MODE_GLIDE)
	// todo, glide needs its own setup here...
#else
	if (0)//(draw->_gl_vbo[_VL_BUFFER_VERTICES] != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, draw->_gl_vbo[_VL_BUFFER_VERTICES]);
		glVertexPointer(3, GL_FLOAT, sizeof(vlVertex_t), NULL);

		glBindBuffer(GL_ARRAY_BUFFER, draw->_gl_vbo[_VL_BUFFER_TEXCOORDS]);
		glTexCoordPointer(2, GL_FLOAT, sizeof(vlVertex_t), NULL);

		// todo, switch to using glInterleavedArrays?

		if (draw->primitive == VL_PRIMITIVE_TRIANGLES)
			_vlDrawElements(
				draw->primitive,
				draw->numtriangles * 3,
				GL_UNSIGNED_BYTE,
				draw->indices
			);
		else
			_vlDrawArrays(draw->primitive, 0, draw->numverts);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else
	{
#if defined (VL_MODE_OPENGL) && !defined (VL_MODE_OPENGL_CORE)
		// Immediate mode isn't supported in core!

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);

		vlVertex_t *vert = &draw->vertices[0];
		glVertexPointer(3, GL_FLOAT, sizeof(vlVertex_t), vert->position);
		glColorPointer(4, GL_FLOAT, sizeof(vlVertex_t), vert->colour);
		glNormalPointer(GL_FLOAT, sizeof(vlVertex_t), vert->normal);
		for (int i = 0; i < Video.num_textureunits; i++)
			if (Video.textureunits[i].isactive)
			{
				glClientActiveTexture(vlGetTextureUnit(i));
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2, GL_FLOAT, sizeof(vlVertex_t), vert->ST[i]);
			}

		if (draw->primitive == VL_PRIMITIVE_TRIANGLES)
			_vlDrawElements(
				draw->primitive,
				draw->numtriangles * 3,
				GL_UNSIGNED_BYTE,
				draw->indices
			);
		else
			_vlDrawArrays(draw->primitive, 0, draw->numverts);

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		for (int i = 0; i < Video.num_textureunits; i++)
			if (Video.textureunits[i].isactive)
			{
				glClientActiveTexture(vlGetTextureUnit(i));
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			}
#endif
	}
#endif
}

void vlDrawVertexNormals(vlDraw_t *draw)
{
	_VL_UTIL_TRACK(vlDrawVertexNormals);

	if (draw->primitive == VL_PRIMITIVE_LINES)
		return;

	plVector3f_t endpos;
	for (unsigned int i = 0; i < draw->numverts; i++)
	{
		plVectorClear(endpos);
		plVectorScalef(draw->vertices[i].normal, 2.0f, endpos);
		plVectorAdd3fv(endpos, draw->vertices[i].position, endpos);

		//Draw_Line(draw->vertices[i].position, endpos);
	}
}

/*===========================
	LIGHTING
===========================*/

void vlApplyLighting(vlDraw_t *object, vlLight_t *light, plVector3f_t position)
{
	_VL_UTIL_TRACK(vlApplyLighting);

	// Calculate the distance.
	plVector3f_t distvec;
	plVectorSubtract3fv(position, light->position, distvec);
	float distance = (light->radius - plLengthf(distvec)) / 100.0f;

	for (unsigned int i = 0; i < object->numverts; i++)
	{
		float x = object->vertices[i].normal[0];
		float y = object->vertices[i].normal[1];
		float z = object->vertices[i].normal[2];

		float angle = (distance*((x * distvec[0]) + (y * distvec[1]) + (z * distvec[2])));
		if (angle < 0)
			plVectorClear(object->vertices[i].colour);
		else
		{
			object->vertices[i].colour[PL_RED] = light->colour[PL_RED] * angle;
			object->vertices[i].colour[PL_GREEN] = light->colour[PL_GREEN] * angle;
			object->vertices[i].colour[PL_BLUE] = light->colour[PL_BLUE] * angle;
		}

		/*
		x = Object->Vertices_normalStat[count].x;
		y = Object->Vertices_normalStat[count].y;
		z = Object->Vertices_normalStat[count].z;

		angle = (LightDist*((x * Object->Spotlight.x) + (y * Object->Spotlight.y) + (z * Object->Spotlight.z) ));
		if (angle<0 )
		{
		Object->Vertices_screen[count].r = 0;
		Object->Vertices_screen[count].b = 0;
		Object->Vertices_screen[count].g = 0;
		}
		else
		{
		Object->Vertices_screen[count].r = Object->Vertices_local[count].r * angle;
		Object->Vertices_screen[count].b = Object->Vertices_local[count].b * angle;
		Object->Vertices_screen[count].g = Object->Vertices_local[count].g * angle;
		}
		*/
	}
}

/*===========================
	MISC
===========================*/

void vlViewport(int x, int y, unsigned int width, unsigned int height)
{
	_VL_UTIL_TRACK(vlViewport);

	if (((x == vl_state.viewport_x) && (y == vl_state.viewport_y)) &&
		((width == vl_state.viewport_width) && (height == vl_state.viewport_height)))
		return;

#if defined (VL_MODE_OPENGL)
	glViewport(x, y, width, height);

	vl_state.viewport_x = x;
	vl_state.viewport_y = y;
	vl_state.viewport_width = width;
	vl_state.viewport_height = height;
#elif defined (VL_MODE_DIRECT3D)
	D3D11_VIEWPORT viewport;
	memset(&viewport, 0, sizeof(D3D11_VIEWPORT));

	vl_state.viewport_x			= viewport.TopLeftX		= x;
	vl_state.viewport_y			= viewport.TopLeftY		= y;
	vl_state.viewport_width		= viewport.Width		= width;
	vl_state.viewport_height	= viewport.Height		= height;

	vl_d3d_context->lpVtbl->RSSetViewports(vl_d3d_context, 1, &viewport);
#endif
}

void vlSetCullMode(VLCullMode mode)
{
	_VL_UTIL_TRACK(vlSetCullMode);

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
#elif defined (VL_MODE_DIRECT3D)
	// todo, create new render state and somehow get the properties of the
	// current but update them to reflect the new cull mode.

	vl_d3d_context->lpVtbl->RSSetState(vl_d3d_context, vl_d3d_state);
#endif
	vl_state.current_cullmode = mode;
}

void vlFinish(void)
{
	_VL_UTIL_TRACK(vlFinish);

#if defined (VL_MODE_OPENGL) || defined (VL_MODE_OPENGL_CORE)
	glFinish();
#elif defined (VL_MODE_GLIDE)
	grFinish();
#elif defined (VL_MODE_DIRECT3D)
	// Not supported, or rather, we don't need this.
#endif
}