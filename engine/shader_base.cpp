/*	Copyright (C) 2011-2015 OldTimes Software

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
public:
	BaseShader();

	virtual void Initialize();
	virtual void Draw();
	virtual void Shutdown();

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

BaseShader::BaseShader()
{
	alpha_test		= false;
	alpha_clamp		= 0.5f;
}

void BaseShader::Initialize()
{
	PROGRAM_REGISTER_SHADER_START();
	PROGRAM_REGISTER_SHADER(base, VIDEO_SHADER_VERTEX);
	PROGRAM_REGISTER_SHADER(base, VIDEO_SHADER_FRAGMENT);
	PROGRAM_REGISTER_SHADER_END();

	PROGRAM_REGISTER_UNIFORM(u_diffusemap);
	PROGRAM_REGISTER_UNIFORM(u_detailmap);
	PROGRAM_REGISTER_UNIFORM(u_fullbrightmap);
	PROGRAM_REGISTER_UNIFORM(u_spheremap);

	PROGRAM_REGISTER_UNIFORM(u_alphaclamp);
	PROGRAM_REGISTER_UNIFORM(u_alphatest);
	
	PROGRAM_REGISTER_UNIFORM(u_lightcolour);
	PROGRAM_REGISTER_UNIFORM(u_lightposition);

	PROGRAM_REGISTER_UNIFORM(u_vertexscale);

	// Set defaults...
	program->SetVariable(u_alphaclamp, alpha_clamp);
}

void BaseShader::EnableAlphaTest()
{
	if (alpha_test)
		return;
	program->SetVariable(u_alphatest, 1);
	alpha_test = true;
}

void BaseShader::DisableAlphaTest()
{
	if (!alpha_test)
		return;
	program->SetVariable(u_alphatest, 0);
	alpha_test = false;
}