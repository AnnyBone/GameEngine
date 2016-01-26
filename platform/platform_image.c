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

#include "platform.h"

#include "platform_filesystem.h"
#include "platform_image.h"

/*
	BMP Support
*/

typedef struct
{
	unsigned short	type;
	unsigned int	size;
	unsigned short	reserved_a;
	unsigned short	reserved_b;
	unsigned int	offset;
} BMPFileHeader_t;

typedef struct
{
	unsigned char	blue;
	unsigned char	green;
	unsigned char	red;
	unsigned char	reserved;
} BMPRGBQuad_t;

typedef struct
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

typedef struct
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

/*
	DDS Support
*/



/*
	FTX Support
*/

typedef struct
{
	unsigned int width;
	unsigned int height;
	unsigned int alpha;

	uint8_t		*data;
} FTXHeader_t;

uint8_t *plLoadFTXImage(FILE *fin, unsigned int *width, unsigned int *height)
{
	FTXHeader_t		header;
	uint8_t			*image;

	plSetErrorFunction("plLoadPPMImage");

	header.width	= plGetLittleLong(fin);
	header.height	= plGetLittleLong(fin);
	header.alpha	= plGetLittleLong(fin);

	image = (uint8_t*)malloc(header.width * header.height * 4);
	fread(image, sizeof(uint8_t), header.width * header.height * 4, fin);
	fclose(fin);

	*width = header.width;
	*height = header.height;
	return image;
}

/*
	PPM Support
*/

#define	PPM_HEADER_SIZE	70

uint8_t *plLoadPPMImage(FILE *fin, unsigned int *width, unsigned int *height)
{
	int				i, d;
	unsigned int	w, h;
	uint8_t			*image;
	char			header[PPM_HEADER_SIZE];

	plSetErrorFunction("plLoadPPMImage");

	fgets(header, PPM_HEADER_SIZE, fin);
	if (strncmp(header, "P6", 2))
	{
		plSetError("Unsupported PPM type!\n");

		fclose(fin);
		return NULL;
	}

	i = 0;
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

	image = (uint8_t*)malloc(sizeof(uint8_t) * w * h * 3);
	fread(image, sizeof(uint8_t), w * h * 3, fin);
	fclose(fin);

	*width = w;
	*height = h;
	return image;
}