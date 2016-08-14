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

#include "platform.h"

#include "platform_filesystem.h"
#include "platform_video_layer.h"
#include "platform_image.h"

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

/*	DDS Support	*/

/*	Ritual's FTX Format	*/

typedef struct
{
	PLuint32 width;
	PLuint32 height;
	PLuint32 alpha;

	PLbyte	*data;
} FTXHeader_t;

PLresult plLoadFTXImage(FILE *fin, PLImage *out)
{
	plSetErrorFunction("plLoadPPMImage");

	FTXHeader_t	header;
	memset(&header, 0, sizeof(FTXHeader_t));
	header.width	= plGetLittleLong(fin);
	header.height	= plGetLittleLong(fin);
	header.alpha	= plGetLittleLong(fin);

	if (out->data) free(out->data);

	memset(out, 0, sizeof(PLImage));

	out->size = header.width * header.height * 4;
	out->data = (uint8_t*)malloc(out->size);
	if (fread(out->data, sizeof(uint8_t), out->size, fin) != out->size)
		return PL_RESULT_FILEREAD;

	out->format = VL_TEXTUREFORMAT_RGBA;

	out->width = header.width;
	out->height = header.height;
	return PL_RESULT_SUCCESS;
}

/*	PPM Format	*/

#define	PPM_HEADER_SIZE	70

uint8_t *plLoadPPMImage(FILE *fin, unsigned int *width, unsigned int *height)
{
	plSetErrorFunction("plLoadPPMImage");

	char header[PPM_HEADER_SIZE];
	memset(&header, 0, sizeof(header));

	fgets(header, PPM_HEADER_SIZE, fin);
	if (strncmp(header, "P6", 2))
	{
		plSetError("Unsupported PPM type!\n");

		fclose(fin);
		return NULL;
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

	uint8_t	*image = (uint8_t*)malloc(sizeof(uint8_t) * w * h * 3);
	fread(image, sizeof(uint8_t), w * h * 3, fin);
	fclose(fin);

	*width = w;
	*height = h;
	return image;
}

/*	Monolith's DTX Format (http://www.cnblogs.com/crsky/p/4702916.html)	*/

typedef struct
{
	PLuint32	type;			// This is always the same (resource type).
	PLint32		version;		// Version of the format, Lithtech used negative numbers.

	PLuint16	width, height;	// Width and height of the texture.
	PLuint16	mipmaps;		// Number of mipmaps included.
	PLuint16	sections;

	PLint32		flags, userflags;
	PLbyte		extra[12];
	PLchar		commandstring[128];
} DTXHeader;

typedef struct
{
	// no idea.
} DTXSection;

#define DTX_VERSION_2		-2				// Lithtech 1.0 (Shogo)
											// Lithtech 1.5
											// Lithtech 2.0
											// Lithtech 2.2
											// Lithtech 2.4
#define DTX_VERSION_5		-5				// Lithtech Talon (Purge)
											// Lithtech Jupiter
											// Lithtech Jupiter EX
#define DTX_VERSION_MIN		DTX_VERSION_2
#define DTX_VERSION_MAX		DTX_VERSION_5

#define DTX_GROUP(a)	a.extra[0]
#define DTX_MIPMAP(a)	a.extra[1]
#define DTX_FORMAT(a)	a.extra[2]
#define DTX_OFFSET(a)	a.extra[3]

enum DTXFlag
{
	DTX_FLAG_FULLBRIGHT		= (1 << 0),
	DTX_FLAG_16BIT			= (1 << 1),
	DTX_FLAG_4444			= (1 << 7),
	DTX_FLAG_5551			= (1 << 8),
	DTX_FLAG_CUBEMAP		= (1 << 10),
	DTX_FLAG_NORMALMAP		= (1 << 11),
} DTXFlag;

enum DTXFormat
{
	DTX_FORMAT_8PALLETTE,
	DTX_FORMAT_8,
	DTX_FORMAT_16,
	DTX_FORMAT_32,

	// Compressed Formats
	DTX_FORMAT_S3TC_DXT1,
	DTX_FORMAT_S3TC_DXT3,
	DTX_FORMAT_S3TC_DXT5,

	DTX_FORMAT_END
} DTXFormat;

PLbyte _plGetDTXFormat(DTXHeader *dtx)
{
	// This is a little hacky, DTX version 2 images don't seem to use
	// the extra[2] slot the same way as later versions. So we need to
	// basically switch it to 0 since 99.9% of textures from that period
	// use a pallette anyway.
	//
	// tl;dr this is a hack because I'm lazy!
	if (dtx->version <= DTX_VERSION_2)
		return DTX_FORMAT_8PALLETTE;
	 
	return dtx->extra[2];
}

PLresult plLoadDTXImage(FILE *fin, PLImage *out)
{
	plSetErrorFunction("plLoadDTXImage");

	DTXHeader header;
	memset(&header, 0, sizeof(header));

	if (fread(&header, sizeof(DTXHeader), 1, fin) != 1)
		return PL_RESULT_FILEREAD;
	
	if (header.type != 0)
		return PL_RESULT_FILETYPE;
	else if ((header.version < DTX_VERSION_MAX) || (header.version > DTX_VERSION_MIN))
		return PL_RESULT_FILEVERSION;
	else if ((header.width < 8) || (header.height < 8))
		return PL_RESULT_IMAGERESOLUTION;

	if (out->data) free(out->data);

	memset(out, 0, sizeof(PLImage));

	out->width = header.width;
	out->height = header.height;
	
	switch (_plGetDTXFormat(&header))
	{
	case DTX_FORMAT_8PALLETTE:
		out->size = header.width * header.height;
		out->format = VL_TEXTUREFORMAT_RGB;
		break;

	case DTX_FORMAT_S3TC_DXT1:
		out->size = (header.width * header.height) >> 1;
		out->format = VL_TEXTUREFORMAT_RGB_DXT1;
		break;
	case DTX_FORMAT_S3TC_DXT3:
		out->size = header.width * header.height;
		out->format = VL_TEXTUREFORMAT_RGBA_DXT3;
		break;
	case DTX_FORMAT_S3TC_DXT5:
		out->size = header.width * header.height;
		out->format = VL_TEXTUREFORMAT_RGBA_DXT5;
		break;

	case DTX_FORMAT_8:
		out->size = header.width * header.height;
		out->format = VL_TEXTUREFORMAT_RGB;
		break;
	case DTX_FORMAT_16:
	case DTX_FORMAT_32:
		out->size = header.width * header.height * 4;
		out->format = VL_TEXTUREFORMAT_RGBA;
		break;
	}

	if (!out->size)
		return PL_RESULT_FILESIZE;

	for(int i = 0; i < header.mipmaps; i++)
	{ }

	out->data = (PLbyte*)calloc(out->size, sizeof(PLbyte));
	if (!out->data) return PL_RESULT_MEMORYALLOC;

	fread(out->data, sizeof(PLbyte), out->size, fin);

/*	for (PLuint i = 0; i < (PLuint)size; i += 4)
	{
		image[i + 0] ^= image[i + 2];
		image[i + 2] ^= image[i + 0];
		image[i + 0] ^= image[i + 2];
	}*/

	return PL_RESULT_SUCCESS;
}