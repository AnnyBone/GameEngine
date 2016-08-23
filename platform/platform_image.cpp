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

#include "platform_image.h"
#include "platform_filesystem.h"

/*	BMP Support	*/

typedef struct BMPFileHeader_s
{
	unsigned short	type;
	unsigned int	size;
	unsigned short	reserved_a;
	unsigned short	reserved_b;
	unsigned int	offset;
} BMPFileHeader_t;

typedef struct BMPRGBQuad_s
{
	unsigned char	blue;
	unsigned char	green;
	unsigned char	red;
	unsigned char	reserved;
} BMPRGBQuad_t;

typedef struct BMPInfoHeader_s
{
	unsigned int	size;
	int				width;
	int				height;
	unsigned short	planes;
	unsigned short	bpp;
	unsigned int	compression;
	unsigned int	imagedata;
	int				ppm_x;
	int				ppm_y;
	unsigned int	colours;
	unsigned int	imp_colours;
} BMPInfoHeader_t;

typedef struct BMPInfo_s
{
	BMPInfoHeader_t		header;
	BMPRGBQuad_t		colours[256];
} BMPInfo_t;

#define	BMP_TYPE	0x4D42

enum
{
	BMP_COMPRESSION_RGB,		// No compression.
	BMP_COMPRESSION_RLE8,		// 8-bit run-length compression.
	BMP_COMPRESSION_RLE4,		// 4-bit run-length compression.
	BMP_COMPRESSION_BITFIELDS,	// RGB bitmap with RGB masks.
};

#if 0
uint8_t *plLoadBMPImage(FILE *fin, unsigned int *width, unsigned int *height)
{
	BMPInfoHeader_t		*infoheader;
	BMPFileHeader_t		header;
	int					infosize, size;
	uint8_t				*image;

	plSetErrorFunction("plLoadPPMImage");

	if (fread(&header, sizeof(BMPFileHeader_t), 1, fin) < 1)
	{
		plSetError("Invalid BMP header!\n");

		fclose(fin);
		return NULL;
	}

	if (header.type != 'MB')
	{
		plSetError("Not a valid BMP file!\n");

		fclose(fin);
		return NULL;
	}

	infosize = (header.offset - sizeof(BMPFileHeader_t));
	//if ((infoheader = (BMPIn)
}
#endif

/*	PPM Format	*/

#define	PPM_HEADER_SIZE	70

PLresult plLoadPPMImage(FILE *fin, PLImage *out)
{
	plSetErrorFunction("plLoadPPMImage");

	char header[PPM_HEADER_SIZE];
	memset(&header, 0, sizeof(header));

	fgets(header, PPM_HEADER_SIZE, fin);
	if (strncmp(header, "P6", 2))
	{
		plSetError("Unsupported PPM type!\n");
		return PL_RESULT_FILEVERSION;
	}

	int i = 0, d;
	unsigned int w, h;
	while (i < 3)
	{
		fgets(header, PPM_HEADER_SIZE, fin);
		if (header[0] == '#')
			continue;

		if (i == 0)
			i += sscanf(header, "%d %d %d", &w, &h, &d);
		else if (i == 1)
			i += sscanf(header, "%d %d", &h, &d);
		else if (i == 2)
			i += sscanf(header, "%d", &d);
	}

	PLuint size = w * h * 3;
	uint8_t	*image = (uint8_t*)malloc(sizeof(uint8_t) * size);
	fread(image, sizeof(uint8_t), size, fin);
	
	memset(out, 0, sizeof(PLImage));
	out->size		= size;
	out->width		= w;
	out->height		= h;
	out->data		= image;
	out->format		= VL_TEXTUREFORMAT_RGB8;

	return PL_RESULT_SUCCESS;
}

