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

		void Entities(bool alphapass);
		void Shadows();
		void BoundingBoxes();
	}
}

#endif