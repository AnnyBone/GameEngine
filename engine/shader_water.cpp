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

