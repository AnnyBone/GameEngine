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

class BaseShader : public Core::ShaderProgram
{
public:
	BaseShader();
	~BaseShader();

	void Initialize();

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