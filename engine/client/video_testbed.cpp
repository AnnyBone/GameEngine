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

#include "engine_base.h"

#include "video.h"

/*
	TEST BED
	The following code is for testing the design
	of the material system, rendering API and other
	fun toybox stuff.
*/

PLDraw *base_draw = nullptr;

void Debug_Initialize()
{
	base_draw = vlCreateDraw(VL_PRIMITIVE_TRIANGLE_FAN, 2, 4);
	if (!base_draw)
		Sys_Error("Failed to create debug draw object!\n");

	vlBeginDraw(base_draw);
	vlEndDraw(base_draw);
}

void Debug_DrawVBO()
{
}