/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others
Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

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
#include "video.h"
#include "XenonTexture.h"

XTextureManager *g_texturemanager = nullptr;

ConsoleVariable_t cv_texture_anisotropy = { "texture_anisotropy", "16", true };

XTextureManager::XTextureManager()
{
	Con_Printf("Initializing Texture Manager...\n");

	Cvar_RegisterVariable(&cv_texture_anisotropy, NULL);

	Image_InitializePNG();
}

XTextureManager::~XTextureManager()
{
	_textures.clear();
}

/*	Utility	*/

PLbool XTextureManager::IsValidSize(PLuint width, PLuint height)
{
	// Check it's not a null size.
	if ((width == 0) || (height == 0))
	{
		Con_Warning("Null width or height! (%i x %i)\n", width, height);
		return false;
	}
	// Check that it's a multiple of two.
	else if ((width && !(width & (width - 1))) && (height && !(height & (height - 1))))
	{
		Con_Warning("Resolution isn't a multiple of 2! (%i x %i)\n", width, height);
		return false;
	}

	return true;
}

XTexture* XTextureManager::CreateTexture(std::string path)
{
	XTexture *texture = GetTexture(path);
	if (texture) 
		return texture;

	FILE *f;
	PLImage image;

	std::string dtxpath = path + PLIMAGE_EXTENSION_DTX;
	COM_FOpenFile(dtxpath.c_str(), &f);
	if (f)
	{
		PLresult result = plLoadDTXImage(f, &image);

		fclose(f);

		if (result == PL_RESULT_SUCCESS)
		{
			texture = CreateTexture(&image);
			free(image.data);
			return texture;
		}
	}

	std::string ftxpath = path + PLIMAGE_EXTENSION_FTX;
	COM_FOpenFile(ftxpath.c_str(), &f);
	if (f)
	{
		PLresult result = plLoadFTXImage(f, &image);

		fclose(f);

		if (result == PL_RESULT_SUCCESS)
		{
			texture = CreateTexture(&image);
			free(image.data);
			return texture;
		}
	}

	std::string tgapath = path + PLIMAGE_EXTENSION_TGA;
	COM_FOpenFile(tgapath.c_str(), &f);
	if (f)
	{
		memset(&image, 0, sizeof(PLImage));

		PLuint width, height;
		PLbyte *data = Image_LoadTGA(f, &width, &height);

		fclose(f);

		if (data)
		{
			texture = CreateTexture(path, width, height, VL_TEXTUREFORMAT_RGBA, data, (width * height * 4));
			free(data);
			return texture;
		}
	}

	Con_Warning("Failed to load texture! (%s)\n", path.c_str());

	return nullptr;
}

XTexture* XTextureManager::CreateTexture(std::string name, PLuint width, PLuint height, VLTextureFormat format, PLbyte *data, PLuint size, PLuint flags)
{
	PLImage image;
	memset(&image, 0, sizeof(PLImage));
	image.data		= data;
	image.flags		= flags;
	image.format	= format;
	image.width		= width;
	image.height	= height;
	image.size		= size;

	return CreateTexture(&image);
}

XTexture *XTextureManager::CreateTexture(PLImage *image)
{
	if (!image || !image->data) throw XException("Invalid image data!\n");

	// Check to see if we already have this texture data.
	PLushort crc = CRC_Block(image->data, image->size);
	XTexture *texture = GetTexture(crc);
	if (texture) return texture;

	// Ensure it's a valid size, do we always want to do this?
	if (!g_texturemanager->IsValidSize(image->width, image->height))
	{
	}

	// Add the new texture to our manager.
	XTexture tex;
	tex.SetImage(image);
	tex.SetCRC(crc);

	return &_textures.emplace(crc, tex).first->second;
}

/*	Texture Management	*/

XTexture *XTextureManager::GetTexture(std::string path)
{
	auto texture = _textures.begin();
	while (texture != _textures.end())
		if (path == texture->second.path)
			return &texture->second;

	return nullptr;
}

XTexture *XTextureManager::GetTexture(PLushort crc)
{
	auto texture = _textures.find(crc);
	if (texture != _textures.end())
		return &texture->second;

	return nullptr;
}

void XTextureManager::DeleteTexture(XTexture *texture, PLbool force)
{
	if (!texture || ((texture->GetFlags() & XTEXTURE_FLAG_PRESERVE) && !force))
		return;

	// Remove it from the list.
	auto tex = _textures.begin();
	while(tex != _textures.end())
		if (&tex->second == texture)
		{
			_textures.erase(tex);
			return;
		}
}

//////////////////////////////////////////////////////////////////////////

XTexture::XTexture() :
	_flags(0),
	_width(8), _height(8),
	_format(VL_TEXTUREFORMAT_RGBA),
	_size(0),
	path(""),
	_crc(0)
{
	_id = vlGenerateTexture();
}

XTexture::~XTexture()
{
	vlDeleteTexture(&_id);
}

void XTexture::SetImage(PLImage *image)
{
	PLushort crc = CRC_Block(image->data, image->size);

	_width		= image->width;
	_height		= image->height;
	_format		= image->format;
	_size		= image->size;
	path		= image->path;

	if (_flags & XTEXTURE_FLAG_MIPMAP)
		vlEnable(VL_CAPABILITY_GENERATEMIPMAP);

	VLTextureInfo upload;
	memset(&upload, 0, sizeof(VLTextureInfo));
	upload.data			= image->data;
	upload.format		= image->format;
	if (_flags & XTEXTURE_FLAG_ALPHA)
		upload.pixel_format = VL_COLOURFORMAT_BGRA;
	else
		upload.pixel_format = VL_COLOURFORMAT_BGR;
	upload.width		= image->width;
	upload.height		= image->height;
	upload.size			= image->size;

	vlUploadTexture(_id, &upload);
	
	if (_flags & XTEXTURE_FLAG_MIPMAP)
		vlDisable(VL_CAPABILITY_GENERATEMIPMAP);

	VLTextureFilter filtermode = VL_TEXTUREFILTER_LINEAR;
	if (_flags & XTEXTURE_FLAG_MIPMAP)
	{
		if (_flags & XTEXTURE_FLAG_NEAREST)	filtermode = VL_TEXTUREFILTER_MIPMAP_NEAREST;
		else								filtermode = VL_TEXTUREFILTER_MIPMAP_LINEAR;
	}
	else if(_flags & XTEXTURE_FLAG_NEAREST)
		filtermode = VL_TEXTUREFILTER_NEAREST;

	vlSetTextureFilter(_id, filtermode);
	vlSetTextureAnisotropy(_id, cv_texture_anisotropy.value);
}

void XTexture::Bind()
{
	vlBindTexture(VL_TEXTURE_2D, _id);
}

/*	Flags	*/

void XTexture::AddFlags(PLuint flags)
{
	_flags |= flags;
}

void XTexture::RemoveFlags(PLuint flags)
{
	_flags &= ~flags;
}
