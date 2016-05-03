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