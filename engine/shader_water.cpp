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

/*
	Implementation for a water shader.
*/

class WaterShader : public VideoShaderManager
{
public:
	WaterShader();
	~WaterShader();

	virtual void Initialize();
	virtual void Draw();
	virtual void Shutdown();

protected:

private:
	uniform u_diffusemap;
	uniform	u_normalmap;

	uniform	u_time;
};

WaterShader::WaterShader()
{
	program	= NULL;
}

void WaterShader::Initialize()
{
	PROGRAM_REGISTER_SHADER_START();
	PROGRAM_REGISTER_SHADER(base, VIDEO_SHADER_VERTEX);
	PROGRAM_REGISTER_SHADER(water, VIDEO_SHADER_FRAGMENT);
	PROGRAM_REGISTER_SHADER_END();

	PROGRAM_REGISTER_UNIFORM(u_diffusemap);
	PROGRAM_REGISTER_UNIFORM(u_normalmap);

	PROGRAM_REGISTER_UNIFORM(u_time);
}

void WaterShader::Shutdown()
{
}

