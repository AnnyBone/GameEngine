/*
Copyright (C) 2011-2016 OldTimes Software

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

#pragma once

class BaseShader : public pl::graphics::ShaderProgram
{
public:
	BaseShader();
	~BaseShader();

	void Initialize();

	void EnableAlphaTest();
	void DisableAlphaTest();
protected:
private:

	// Textures
	PLShaderUniform *u_diffusemap;
	PLShaderUniform *u_detailmap;
	PLShaderUniform *u_fullbrightmap;
	PLShaderUniform *u_spheremap;

	// Alpha
	PLShaderUniform *u_alphaclamp;
	PLShaderUniform *u_alphatest;

	// Lighting
	PLShaderUniform *u_lightcolour;
	PLShaderUniform *u_lightposition;

	// Vertex scaling
	PLShaderUniform *u_vertexscale;

	PLShader *fragment_, *vertex_;

	bool	alpha_test;
	float	alpha_clamp;
};