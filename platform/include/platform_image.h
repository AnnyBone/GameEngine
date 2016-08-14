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

typedef struct PLImage
{
	PLbyte *data;

	PLuint width, height;
	PLuint size;
	PLuint levels;

	PLchar path[PL_MAX_PATH];

	VLTextureFormat format;

	PLuint flags;
} PLImage;

#define PLIMAGE_EXTENSION_FTX	".ftx"	// Ritual's FTX image format.
#define PLIMAGE_EXTENSION_DTX	".dtx"	// Lithtech's DTX image format.
#define PLIMAGE_EXTENSION_PPM	".ppm"	// Portable Pixel Map format.
#define PLIMAGE_EXTENSION_TGA	".tga"

plEXTERN_C_START

extern PLresult plLoadFTXImage(FILE *fin, PLImage *out);	// Ritual's FTX image format.
extern PLresult plLoadPPMImage(FILE *fin, PLImage *out);	// Portable Pixel Map format.
extern PLresult plLoadDTXImage(FILE *fin, PLImage *out);	// Lithtech's DTX image format.

plEXTERN_C_END