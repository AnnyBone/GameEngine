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

class BaseShader : public VideoShaderManager
{
	SHADER_IMPLEMENT(BaseShader);

	void EnableAlphaTest();
	void DisableAlphaTest();
protected:
private:
	uniform u_alphaclamp;
	uniform u_alphatest;

	uniform	u_diffusemap;
	uniform	u_detailmap;
	uniform	u_fullbrightmap;
	uniform	u_spheremap;

	uniform u_lightposition;
	uniform u_lightcolour;

	uniform u_vertexscale;

	bool	alpha_test;
	float	alpha_clamp;
};

BaseShader::BaseShader() : VideoShaderManager()
{
	alpha_test		= false;
	alpha_clamp		= 0.5f;
}

SHADER_REGISTER_START(BaseShader)

	SHADER_REGISTER_SCRIPT(base, VIDEO_SHADER_VERTEX)
	SHADER_REGISTER_SCRIPT(base, VIDEO_SHADER_FRAGMENT)

	SHADER_REGISTER_LINK()

	// Textures
	SHADER_REGISTER_UNIFORM(u_diffusemap, 0)
	SHADER_REGISTER_UNIFORM(u_detailmap, 0)
	SHADER_REGISTER_UNIFORM(u_fullbrightmap, 0)
	SHADER_REGISTER_UNIFORM(u_spheremap, 0)

	// Alpha
	SHADER_REGISTER_UNIFORM(u_alphaclamp, alpha_clamp)
	SHADER_REGISTER_UNIFORM(u_alphatest, alpha_test)

	// Lighting
	SHADER_REGISTER_UNIFORM(u_lightcolour, g_colourwhite)
	SHADER_REGISTER_UNIFORM(u_lightposition, g_mvOrigin3f)

	// Vertex scaling
	SHADER_REGISTER_UNIFORM(u_vertexscale, 1.0f)

SHADER_REGISTER_END()

void BaseShader::EnableAlphaTest()
{
VIDEO_FUNCTION_START
	if (alpha_test)
		return;
	program->SetVariable(u_alphatest, 1);
	alpha_test = true;
VIDEO_FUNCTION_END
}

void BaseShader::DisableAlphaTest()
{
VIDEO_FUNCTION_START
	if (!alpha_test)
		return;
	program->SetVariable(u_alphatest, 0);
	alpha_test = false;
VIDEO_FUNCTION_END
}