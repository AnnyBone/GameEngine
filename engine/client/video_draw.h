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
		PL_MODULE_EXPORT void Entity(ClientEntity_t *entity);				// Draws a single entity.
		PL_MODULE_EXPORT void EntityBoundingBox(ClientEntity_t *entity);	// Draws entity bounding box.

		PL_MODULE_EXPORT void ClearBuffers();		// Clears the color, stencil and depth buffers.
		PL_MODULE_EXPORT void DepthBuffer();		// Draws the depth buffer.

		PL_MODULE_EXPORT void SetDefaultState();	// Sets default rendering state.

		PL_MODULE_EXPORT void GradientBackground(plColour_t top, plColour_t bottom);

		PL_MODULE_EXPORT void WireBox(plVector3f_t mins, plVector3f_t maxs, float r, float g, float b);
		PL_MODULE_EXPORT void CoordinateAxes(plVector3f_t position);
		PL_MODULE_EXPORT void Character(int x, int y, int num);
		PL_MODULE_EXPORT void String(int x, int y, const char *msg);
		PL_MODULE_EXPORT void MaterialSurface(Material_t *material, int x, int y, unsigned int w, unsigned int h, float alpha);

		void Entities(bool alphapass);
		void Shadows();
		void BoundingBoxes();
	}
}

#endif