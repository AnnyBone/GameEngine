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

#include "client/shader_water.h"

/*
	Implementation for a water shader.
*/

WaterShader::WaterShader() : ShaderProgram("water")
{
}

void WaterShader::Initialize()
{
	RegisterShader("base", VL_SHADER_VERTEX);
	RegisterShader("water", VL_SHADER_FRAGMENT);

	Link();

	RegisterAttributes();

	// Textures
	SHADER_REGISTER_UNIFORM(u_diffusemap, VL_UNIFORM_TEXTURE2D, 0);
	SHADER_REGISTER_UNIFORM(u_normalmap, VL_UNIFORM_TEXTURE2D, 0);

	SHADER_REGISTER_UNIFORM(u_time, VL_UNIFORM_FLOAT, 0);
}

void WaterShader::SetTime(float curtime)
{
	static float time = 0;
	if (curtime == time)
		return;
	SetUniformVariable(u_time, curtime);
	time = curtime;
}

