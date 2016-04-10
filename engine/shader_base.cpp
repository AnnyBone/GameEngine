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

#include "client/shader_base.h"

BaseShader::BaseShader() : ShaderProgram("base")
{
	alpha_test		= false;
	alpha_clamp		= 0.5f;
}

void BaseShader::Initialize()
{
	Enable();

	RegisterShader("base", VL_SHADER_VERTEX);
	RegisterShader("base", VL_SHADER_FRAGMENT);

	Link();

	RegisterAttributes();

	// Textures
	SHADER_REGISTER_UNIFORM(u_diffusemap, 0);
	SHADER_REGISTER_UNIFORM(u_detailmap, 0);
	SHADER_REGISTER_UNIFORM(u_fullbrightmap, 0);
	SHADER_REGISTER_UNIFORM(u_spheremap, 0);
	
	// Alpha
	SHADER_REGISTER_UNIFORM(u_alphaclamp, alpha_clamp);
	SHADER_REGISTER_UNIFORM(u_alphatest, alpha_test);

	// Lighting
	SHADER_REGISTER_UNIFORM(u_lightcolour, pl_white);
	SHADER_REGISTER_UNIFORM(u_lightposition, pl_origin3f);

	// Vertex scaling
	SHADER_REGISTER_UNIFORM(u_vertexscale, 1.0f);

	Disable();
}

void BaseShader::EnableAlphaTest()
{
VIDEO_FUNCTION_START
	if (alpha_test)
		return;
	SetUniformVariable(u_alphatest, 1);
	alpha_test = true;
VIDEO_FUNCTION_END
}

void BaseShader::DisableAlphaTest()
{
VIDEO_FUNCTION_START
	if (!alpha_test)
		return;
	SetUniformVariable(u_alphatest, 0);
	alpha_test = false;
VIDEO_FUNCTION_END
}