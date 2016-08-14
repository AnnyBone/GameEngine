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

#pragma once

class BaseShader : public core::ShaderProgram
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
	VLUniform *u_diffusemap;
	VLUniform *u_detailmap;
	VLUniform *u_fullbrightmap;
	VLUniform *u_spheremap;

	// Alpha
	VLUniform *u_alphaclamp;
	VLUniform *u_alphatest;

	// Lighting
	VLUniform *u_lightcolour;
	VLUniform *u_lightposition;

	// Vertex scaling
	VLUniform *u_vertexscale;

	bool	alpha_test;
	float	alpha_clamp;
};