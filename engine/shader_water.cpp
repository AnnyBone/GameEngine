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

class WaterShader : public ShaderInstance
{
	SHADER_IMPLEMENT(WaterShader);

	void SetTime(float curtime);
private:
	uniform u_diffusemap;
	uniform	u_normalmap;

	uniform	u_time;
};

SHADER_REGISTER_START(WaterShader)

	SHADER_REGISTER_SCRIPT(base, VL_SHADER_VERTEX)
	SHADER_REGISTER_SCRIPT(water, VL_SHADER_FRAGMENT)

	SHADER_REGISTER_LINK()

	SHADER_REGISTER_UNIFORM(u_diffusemap, 0)
	SHADER_REGISTER_UNIFORM(u_normalmap, 0)
	SHADER_REGISTER_UNIFORM(u_time, 0)

SHADER_REGISTER_END()

void WaterShader::SetTime(float curtime)
{
VIDEO_FUNCTION_START
	static float time = 0;
	if (curtime == time)
		return;
	program->SetVariable(u_time, curtime);
	time = curtime;
VIDEO_FUNCTION_END
}

