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

#include "engine_base.h"

char loadfilename[PLATFORM_MAX_PATH]; //file scope so that error messages can use it

PLbool image_pngsupport = PL_FALSE;

uint8_t *Image_LoadPNG(FILE *fin, PLuint *width, PLuint *height);

//#define IMAGE_SUPPORT_KTX

/*	Returns a pointer to hunk allocated RGBA data
*/
uint8_t *Image_LoadImage(char *name, unsigned int *width, unsigned int *height)
{
	uint8_t		*bImage;
	FILE		*f;

	// PNG
	if (image_pngsupport)
	{
		sprintf(loadfilename, "%s.png", name);
		COM_FOpenFile(loadfilename, &f);
		if (f)
		{
			bImage = Image_LoadPNG(f, width, height);
			if (bImage)
				return bImage;
		}
	}

	// TGA
	sprintf(loadfilename,"%s.tga",name);
	COM_FOpenFile(loadfilename,&f);
	if(f)
	{
		bImage = Image_LoadTGA(f,width,height);
		if(bImage)
			return bImage;
	}

	// Throw a warning after we've tried all options.
	Con_Warning("Failed to load image! (%s)\n", name);

	return NULL;
}

/*
	TGA
*/

typedef struct targaheader_s {
	unsigned char 	id_length, colormap_type, image_type;
	unsigned short	colormap_index, colormap_length;
	unsigned char	colormap_size;
	unsigned short	x_origin, y_origin, width, height;
	unsigned char	pixel_size, attributes;
} targaheader_t;

#define TARGAHEADERSIZE 18 //size on disk

targaheader_t targa_header;

/*	Writes RGB or RGBA data to a TGA file

	returns TRUE if successful

	TODO: support BGRA and BGR formats (since opengl can return them, and we don't have to swap)
*/
bool Image_WriteTGA(const char *name, uint8_t *data,int width,int height,int bpp,bool upsidedown)
{
	int			handle, i, size, temp, bytes;
	char		pathname[PLATFORM_MAX_PATH];
	uint8_t		header[TARGAHEADERSIZE];

	if (!plCreateDirectory(com_gamedir)) //if we've switched to a nonexistant gamedir, create it now so we don't crash
		Sys_Error("Failed to create directory!\n");

	sprintf (pathname, "%s/%s", com_gamedir, name);
	handle = Sys_FileOpenWrite (pathname);
	if (handle == -1)
		return false;

	memset(&header, 0, TARGAHEADERSIZE);
	header[2]	= 2; // uncompressed type
	header[12]	= width&255;
	header[13]	= width>>8;
	header[14]	= height&255;
	header[15]	= height>>8;
	header[16]	= bpp; // pixel size
	if (upsidedown)
		header[17] = 0x20; //upside-down attribute

	// swap red and blue bytes
	bytes = bpp/8;
	size = width*height*bytes;
	for (i=0; i<size; i+=bytes)
	{
		temp = data[i];
		data[i] = data[i+2];
		data[i+2] = temp;
	}

	Sys_FileWrite (handle, &header, TARGAHEADERSIZE);
	Sys_FileWrite (handle, data, size);
	Sys_FileClose (handle);

	return true;
}

PLbyte *Image_LoadTGA (FILE *fin, unsigned int *width, unsigned int *height)
{
	int	columns,rows,numPixels,row,column,realrow;
	uint8_t	*pixbuf,*targa_rgba;
	bool upside_down;                                //johnfitz -- fix for upside-down targas

	targa_header.id_length			= fgetc(fin);
	targa_header.colormap_type		= fgetc(fin);
	targa_header.image_type			= fgetc(fin);
	targa_header.colormap_index		= plGetLittleShort(fin);
	targa_header.colormap_length	= plGetLittleShort(fin);
	targa_header.colormap_size		= fgetc(fin);
	targa_header.x_origin			= plGetLittleShort(fin);
	targa_header.y_origin			= plGetLittleShort(fin);
	targa_header.width				= plGetLittleShort(fin);
	targa_header.height				= plGetLittleShort(fin);
	targa_header.pixel_size			= fgetc(fin);
	targa_header.attributes			= fgetc(fin);

	if(targa_header.image_type != 2 && targa_header.image_type != 10)
		Sys_Error("Image_LoadTGA: %s is not a type 2 or type 10 targa\n",loadfilename);
	else if(targa_header.colormap_type != 0 || (targa_header.pixel_size != 32 && targa_header.pixel_size != 24))
		Sys_Error("Image_LoadTGA: %s is not a 24bit or 32bit targa\n",loadfilename);

	columns     = targa_header.width;
	rows        = targa_header.height;
	numPixels   = columns * rows;
	upside_down = !(targa_header.attributes & 0x20); //johnfitz -- fix for upside-down targas

	targa_rgba = (PLbyte*)malloc (numPixels*4);

	if (targa_header.id_length != 0)
		fseek(fin, targa_header.id_length, SEEK_CUR);  // skip TARGA image comment

	if (targa_header.image_type==2) // Uncompressed, RGB images
	{
		for(row=rows-1; row>=0; row--)
		{
			//johnfitz -- fix for upside-down targas
			realrow = upside_down ? row : rows - 1 - row;
			pixbuf = targa_rgba + realrow*columns*4;
			//johnfitz
			for(column=0; column<columns; column++)
			{
				unsigned char red,green,blue,alphabyte;
				switch (targa_header.pixel_size)
				{
				case 24:
						blue = getc(fin);
						green = getc(fin);
						red = getc(fin);
						*pixbuf++ = red;
						*pixbuf++ = green;
						*pixbuf++ = blue;
						*pixbuf++ = 255;
						break;
				default:
						blue = getc(fin);
						green = getc(fin);
						red = getc(fin);
						alphabyte = getc(fin);
						*pixbuf++ = red;
						*pixbuf++ = green;
						*pixbuf++ = blue;
						*pixbuf++ = alphabyte;
						break;
				}
			}
		}
	}
	else if(targa_header.image_type == 10) // Runlength encoded RGB images
	{
		unsigned char red,green,blue,alphabyte,packetHeader,packetSize,j;
		for(row=rows-1; row>=0; row--)
		{
			//johnfitz -- fix for upside-down targas
			realrow = upside_down ? row : rows - 1 - row;
			pixbuf = targa_rgba + realrow*columns*4;
			//johnfitz
			for(column = 0; column < columns;)
			{
				packetHeader    =getc(fin);
				packetSize      = 1+(packetHeader & 0x7f);
				if (packetHeader & 0x80) // run-length packet
				{
					switch (targa_header.pixel_size)
					{
					case 24:
							blue        = getc(fin);
							green       = getc(fin);
							red         = getc(fin);
							alphabyte   = 255;
							break;
					default:
							blue        = getc(fin);
							green       = getc(fin);
							red         = getc(fin);
							alphabyte   = getc(fin);
							break;
					}

					for(j=0;j<packetSize;j++)
					{
						*pixbuf++   = red;
						*pixbuf++   = green;
						*pixbuf++   = blue;
						*pixbuf++   = alphabyte;

						column++;
						if (column==columns) // run spans across rows
						{
							column=0;
							if (row>0)
								row--;
							else
								goto breakOut;
							//johnfitz -- fix for upside-down targas
							realrow = upside_down ? row : rows - 1 - row;
							pixbuf = targa_rgba + realrow*columns*4;
							//johnfitz
						}
					}
				}
				else // non run-length packet
				{
					for(j=0;j<packetSize;j++)
					{
						switch (targa_header.pixel_size)
						{
						case 24:
								blue = getc(fin);
								green = getc(fin);
								red = getc(fin);
								*pixbuf++ = red;
								*pixbuf++ = green;
								*pixbuf++ = blue;
								*pixbuf++ = 255;
								break;
						default:
								blue = getc(fin);
								green = getc(fin);
								red = getc(fin);
								alphabyte = getc(fin);
								*pixbuf++ = red;
								*pixbuf++ = green;
								*pixbuf++ = blue;
								*pixbuf++ = alphabyte;
								break;
						}
						column++;
						if (column==columns) // pixel packet run spans across rows
						{
							column=0;
							if (row>0)
								row--;
							else
								goto breakOut;
							//johnfitz -- fix for upside-down targas
							realrow = upside_down ? row : rows - 1 - row;
							pixbuf = targa_rgba + realrow*columns*4;
							//johnfitz
						}
					}
				}
			}
			breakOut:;
		}
	}

	*width = (int)(targa_header.width);
	*height = (int)(targa_header.height);
	return targa_rgba;
}

/*	PNG Support	*/

// Use a direct path, mainly for Linux's sake.
#include "../external/lpng1618/png.h"

PL_INSTANCE iPNGLibraryInstance;

static png_structp(*PNG_CreateReadStruct)(png_const_charp user_png_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn);

static png_infop(*PNG_CreateInfoStruct)(png_const_structrp png_ptr);

static png_uint_32(*PNG_AccessVersionNumber)(void);
static png_uint_32(*PNG_GetImageWidth)(png_const_structrp png_ptr, png_const_inforp info_ptr);
static png_uint_32(*PNG_GetImageHeight)(png_const_structrp png_ptr, png_const_inforp info_ptr);

static void(*PNG_InitIO)(png_structrp png_ptr, png_FILE_p fp);
static void(*PNG_SetSigBytes)(png_structrp png_ptr, int num_bytes);
static void(*PNG_ReadPNG)(png_structrp png_ptr, png_inforp info_ptr, int transforms, png_voidp params);
static void(*PNG_ReadInfo)(png_structrp png_ptr, png_inforp info_ptr);
static void(*PNG_SetKeepUnknownChunks)(png_structrp png_ptr, int keep, png_const_bytep chunk_list, int num_chunks);
static void(*PNG_DestroyReadStruct)(png_structpp png_ptr_ptr, png_infopp info_ptr_ptr, png_infopp end_info_ptr_ptr);

static PLModuleFunction PNGFunctions[]=
{
	{ "png_create_read_struct", (void**)&PNG_CreateReadStruct },
	{ "png_create_info_struct", (void**)&PNG_CreateInfoStruct },
	{ "png_access_version_number", (void**)&PNG_AccessVersionNumber },
	{ "png_get_image_width", (void**)&PNG_GetImageWidth },
	{ "png_get_image_height", (void**)&PNG_GetImageHeight },
	{ "png_init_io", (void**)&PNG_InitIO },
	{ "png_set_sig_bytes", (void**)&PNG_SetSigBytes },
	{ "png_read_png", (void**)&PNG_ReadPNG },
	{ "png_read_info", (void**)&PNG_ReadInfo },
	{ "png_set_keep_unknown_chunks", (void**)&PNG_SetKeepUnknownChunks },
	{ "png_destroy_read_struct", (void**)&PNG_DestroyReadStruct }
};

void Image_InitializePNG()
{
	iPNGLibraryInstance = plLoadLibrary("libpng16");
	if (!iPNGLibraryInstance)
	{
		Con_Warning("Failed to load libpng!\n");
		return;
	}

	// TODO: Introduce new Platform functionality to do this itself?
	int i;
	for (i = 0; i < plArrayElements(PNGFunctions); i++)
	{
		*(PNGFunctions[i].Function) = plFindLibraryFunction(iPNGLibraryInstance, PNGFunctions[i].name);
		if (!PNGFunctions[i].Function)
		{
			Con_Warning("Failed to find libpng function! (%s)\n", PNGFunctions[i].name);
			return;
		}
	}

	image_pngsupport = true;
}

void Image_PNGError(png_structp pPNG, const char *ccString)
{
	Con_Warning("Load error! (%s)\n", ccString);
}

uint8_t *Image_LoadPNG(FILE *fin, unsigned int *width, unsigned int *height)
{
	png_structp pPNG = NULL;
	png_infop pInfo;
	uint8_t *iImageBuffer;
	int iWidth, iHeight;
	//int iBitDepth, iColourType, iInterlaceType;

	// TEMP: Temporarily commented out to force this to fail!
//	pPNG = PNG_CreateReadStruct(PNG_LIBPNG_VER_STRING, NULL, Image_PNGError, Image_PNGError);
	if (!pPNG)
	{
		Con_Warning("Failed to create PNG read struct!\n");

		fclose(fin);
		return NULL;
	}

	pInfo = PNG_CreateInfoStruct(pPNG);
	if (!pInfo)
	{
		Con_Warning("Failed to create info struct!\n");

		PNG_DestroyReadStruct(&pPNG, &pInfo, NULL);

		fclose(fin);
		return NULL;
	}

	PNG_InitIO(pPNG, fin);

	// Ignore unknown chunks.
	PNG_SetKeepUnknownChunks(pPNG, 0, NULL, 0);

	// Last parameter isn't used, so just passed null.
	// No transforms are needed here either (though PNG_TRANSFORM_BGR in future????)
	PNG_ReadPNG(pPNG, pInfo, PNG_TRANSFORM_IDENTITY, NULL);

	// Get the width and height.
	iWidth = PNG_GetImageWidth(pPNG, pInfo);
	iHeight = PNG_GetImageHeight(pPNG, pInfo);

	/* TODO: Is this a memory leak, or one of the many fragile things upon
	 * which everything barely balances?
	 */
	iImageBuffer = (uint8_t*)Hunk_Alloc((iHeight + iWidth) * 4);

	PNG_DestroyReadStruct(&pPNG, &pInfo, NULL);

	fclose(fin);

	return NULL;
}

/**/

void Image_Shutdown()
{
	if (iPNGLibraryInstance)
		plUnloadLibrary(iPNGLibraryInstance);
}
