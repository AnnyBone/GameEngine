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

	PLuint flags;
} PLImage;

enum
{
	PLIMAGE_FLAG_RGBA,
	PLIMAGE_FLAG_BGRA,

	// S3TC Compression
	PLIMAGE_FLAG_DXT1 = (1 << 0),
	PLIMAGE_FLAG_DXT2 = (1 << 1),
	PLIMAGE_FLAG_DXT3 = (1 << 2),
	PLIMAGE_FLAG_DXT4 = (1 << 3),
	PLIMAGE_FLAG_DXT5 = (1 << 4),
};

plEXTERN_C_START

extern PLresult plLoadFTXImage(FILE *fin, PLImage *out);	// Ritual's FTX image format.
extern PLresult plLoadPPMImage(FILE *fin, PLImage *out);	// Load PPM image format.
extern PLresult plLoadDTXImage(FILE *fin, PLImage *out);	// Lithtech DTX image format.

plEXTERN_C_END