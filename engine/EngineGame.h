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

PL_EXTERN_C

extern GameExport_t	*Game;

/*	Global					*/
void Game_Initialize(void);

/*	Server-side functions	*/
void Server_Flare(PLVector3D org, float r, float g, float b, float a, float scale, char *texture);

/*	Client-side functions	*/

PL_EXTERN_C_END