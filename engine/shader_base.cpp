/*	DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
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
#include "video_shader.h"

#include "client/shader_base.h"

BaseShader::BaseShader() : ShaderProgram("base")
{
	alpha_test	= false;
}

void BaseShader::Initialize()
{
	RegisterShader("base", VL_SHADER_VERTEX);
	RegisterShader("base", VL_SHADER_FRAGMENT);

	Link();

	RegisterAttributes();

	// Textures
	SHADER_REGISTER_UNIFORM(u_diffusemap, VL_UNIFORM_TEXTURE2D, VIDEO_TEXTURE_DIFFUSE);
	SHADER_REGISTER_UNIFORM(u_detailmap, VL_UNIFORM_TEXTURE2D, VIDEO_TEXTURE_DETAIL);
	SHADER_REGISTER_UNIFORM(u_fullbrightmap, VL_UNIFORM_TEXTURE2D, VIDEO_TEXTURE_FULLBRIGHT);
	SHADER_REGISTER_UNIFORM(u_spheremap, VL_UNIFORM_TEXTURE2D, VIDEO_TEXTURE_SPHERE);
	
	// Alpha
	SHADER_REGISTER_UNIFORM(u_alphaclamp, VL_UNIFORM_FLOAT, 0.5f);
	SHADER_REGISTER_UNIFORM(u_alphatest, VL_UNIFORM_BOOL, alpha_test);

	// Lighting
	SHADER_REGISTER_UNIFORM(u_lightcolour, VL_UNIFORM_VEC4, pl_white);
	SHADER_REGISTER_UNIFORM(u_lightposition, VL_UNIFORM_VEC3, pl_origin3f);

	// Vertex scaling
	SHADER_REGISTER_UNIFORM(u_vertexscale, VL_UNIFORM_FLOAT, 1.0f);
}

void BaseShader::EnableAlphaTest()
{
	if (alpha_test)
		return;

	alpha_test = true;
	SetUniformVariable(u_alphatest, alpha_test);	
}

void BaseShader::DisableAlphaTest()
{
	if (!alpha_test)
		return;

	alpha_test = false;
	SetUniformVariable(u_alphatest, alpha_test);
}