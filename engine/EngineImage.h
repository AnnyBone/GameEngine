/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
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

plEXTERN_C_START

void Image_InitializePNG();
void Image_Shutdown();

//be sure to free the hunk after using these loading functions
uint8_t *Image_LoadTGA (FILE *f, unsigned int *width, unsigned int *height);
uint8_t *Image_LoadImage (char *name, unsigned int *width, unsigned int *height);

bool Image_WriteTGA(char *name, uint8_t *data,int width,int height,int bpp,bool upsidedown);

plEXTERN_C_END
