/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#pragma once

#ifdef __cplusplus

namespace Core
{
	namespace Draw
	{
		static void SetDefaultState();	// Sets default rendering state.

		static void ClearBuffers();		// Clears the color, stencil and depth buffers.
		static void DepthBuffer();		// Draws the depth buffer.

		static void Entities(bool alphapass);
		static void Shadows();
		static void BoundingBoxes();

		static void WireBox(plVector3f_t mins, plVector3f_t maxs, float r, float g, float b);
		static void CoordinateAxes(plVector3f_t position);
	}
}

#endif