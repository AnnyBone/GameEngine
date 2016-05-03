/*	Copyright (C) 2011-2016 OldTimes Software
*/

#pragma once

#define		VL_MODE_OPENGL
//			VL_MODE_OPENGL_CORE
//			VL_MODE_OPENGL_ES
//#define	VL_MODE_GLIDE
//			VL_MODE_DIRECT3D
//			VL_MODE_VULKAN

typedef unsigned int vlVertexArray_t;
typedef unsigned int vlRenderBuffer_t;
typedef unsigned int vlFrameBuffer_t;

typedef enum vlCullMode_s
{
	VL_CULL_START = -1,

	VL_CULL_POSTIVE,
	VL_CULL_NEGATIVE,

	VL_CULL_END
} vlCullMode_t;

typedef enum vlCapability_s
{
	VL_CAPABILITY_START = -1,

	VL_CAPABILITY_FOG				= (1 << 0),	// Fog.
	VL_CAPABILITY_ALPHA_TEST		= (1 << 1),	// Alpha-testing.
	VL_CAPABILITY_BLEND				= (1 << 2), // Blending.
	VL_CAPABILITY_TEXTURE_2D		= (1 << 3),	// Enables/disables textures.
	VL_CAPABILITY_TEXTURE_GEN_S		= (1 << 4),	// Generate S coordinate.
	VL_CAPABILITY_TEXTURE_GEN_T		= (1 << 5), // Generate T coordinate.
	VL_CAPABILITY_DEPTH_TEST		= (1 << 6),	// Depth-testing.
	VL_CAPABILITY_STENCIL_TEST		= (1 << 7),	// Stencil-testing.
	VL_CAPABILITY_MULTISAMPLE		= (1 << 8), // Multisampling.
	VL_CAPABILITY_CULL_FACE			= (1 << 9),	// Automatically cull faces.

	VL_CAPABILITY_END
} vlCapability_t;

// Blending
#define	VL_BLEND_ADDITIVE	VL_BLEND_SRC_ALPHA, VL_BLEND_ONE
#define	VL_BLEND_DEFAULT	VL_BLEND_SRC_ALPHA, VL_BLEND_ONE_MINUS_SRC_ALPHA

//-----------------
// Textures

// Texture Environment Modes
typedef enum vlTextureEnvironmentMode_s
{
	VL_TEXTUREMODE_START = -1,

	VIDEO_TEXTUREMODE_ADD,
	VIDEO_TEXTUREMODE_MODULATE,
	VIDEO_TEXTUREMODE_DECAL,
	VIDEO_TEXTUREMODE_BLEND,
	VIDEO_TEXTUREMODE_REPLACE,
	VIDEO_TEXTUREMODE_COMBINE,

	VL_TEXTUREMODE_END
} vlTextureEnvironmentMode_t;

//-----------------
// Drawing

// Primitive Types
typedef enum vlPrimitive_s
{
	VL_PRIMITIVE_IGNORE = -1,

	VL_PRIMITIVE_LINES,
	VL_PRIMITIVE_LINE_STRIP,
	VL_PRIMITIVE_POINTS,
	VL_PRIMITIVE_TRIANGLES,
	VL_PRIMITIVE_TRIANGLE_STRIP,
	VL_PRIMITIVE_TRIANGLE_FAN,
	VL_PRIMITIVE_TRIANGLE_FAN_LINE,
	VL_PRIMITIVE_QUADS,				// Advised to avoid this.

	VL_PRIMITIVE_END
} vlPrimitive_t;

typedef struct vlVertex_s
{
	plVector3f_t position;
	plVector3f_t normal;

	plVector2f_t ST[16];

	plColour_t colour;
} vlVertex_t;

typedef struct vlDraw_s
{
	vlVertex_t *vertices;							// Array of vertices for the object.

	unsigned int numverts;							// Number of vertices.
	unsigned int numtriangles;						// Number of triangles.

	uint8_t	*indices;								// List of indeces.

	vlPrimitive_t primitive, primitive_restore;		// Type of primitive, and primitive to restore to.

	unsigned int _gl_vbo[16];						// Vertex buffer object.
} vlDraw_t;

//-----------------
// Shaders

typedef unsigned int vlShaderProgram_t;
typedef unsigned int vlShader_t;

typedef enum vlUnformType_s
{
	VL_UNIFORM_START = -1,

	VL_UNIFORM_FLOAT,
	VL_UNIFORM_INT,
	VL_UNIFORM_UINT,
	VL_UNIFORM_BOOL,
	VL_UNIFORM_DOUBLE,

	// Textures
	VL_UNIFORM_TEXTURE1D,
	VL_UNIFORM_TEXTURE2D,
	VL_UNIFORM_TEXTURE3D,
	VL_UNIFORM_TEXTURECUBE,
	VL_UNIFORM_TEXTUREBUFFER,

	// Vectors
	VL_UNIFORM_VEC2,
	VL_UNIFORM_VEC3,
	VL_UNIFORM_VEC4,

	// Matrices
	VL_UNIFORM_MAT3,

	VL_UNIFORM_END
} vlUniformType_t;

#if 0
typedef int vlUniform_t;
#else
typedef struct vlUniform_s
{
	int location;	// Location within the shader.

	vlUniformType_t type;	// Data type.

	char def[16];
} vlUniform_t;
#endif

typedef int vlAttribute_t;

typedef enum
{
	VL_SHADER_START = -1,

	VL_SHADER_FRAGMENT,
	VL_SHADER_VERTEX,
	VL_SHADER_GEOMETRY,

	VL_SHADER_END
} vlShaderType_t;

//-----------------
// Lighting

typedef struct vlLight_s
{
	plVector3f_t	position;
	plColour_t		colour;
	float			radius;
} vlLight_t;

//-----------------
