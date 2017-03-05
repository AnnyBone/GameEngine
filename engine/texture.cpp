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
#include "texture.h"

TextureManager *g_texturemanager = nullptr;

ConsoleVariable_t cv_texture_anisotropy = { "texture_anisotropy", "16", true };

// Base textures...
namespace textures
{
	Texture *nulltexture = nullptr;
}

void _PrintMemoryUsage()
{
	g_texturemanager->PrintMemoryUsage();
}

TextureManager::TextureManager() {
	Con_Printf("Initializing Texture Manager...\n");

	Cvar_RegisterVariable(&cv_texture_anisotropy, NULL);

	Cmd_AddCommand("tm_memoryusage", _PrintMemoryUsage);

    plInitialize(PL_SUBSYSTEM_IMAGE | PL_SUBSYSTEM_GRAPHICS);

	max_resolution_ = plGetMaxTextureSize();

	static PLbyte notexture_data[16] =
	{
		255,  0,255, 255,
		0,  0,  0, 255,
		0,  0,  0, 255,
		255,  0,255, 255
	};
	textures::nulltexture = CreateTexture(
		"nulltexture",
		2, 2, 
		PL_IMAGEFORMAT_RGB8,
		notexture_data, sizeof(notexture_data), 
		TEXTURE_FLAG_PRESERVE | TEXTURE_FLAG_NEAREST
	);
}

TextureManager::~TextureManager() {

}

/*	Utility	*/

void TextureManager::PrintMemoryUsage() {
	unsigned int texels = 0;
	for(auto tex = _textures.begin(); tex != _textures.end(); ++tex) {
		Texture *texture = tex->second;
		Con_SafePrintf(" %4i x%4i %s\n", 
			texture->GetWidth(), 
			texture->GetHeight(), 
			texture->path.c_str()
        );

		// todo, update this crap...
		if (texture->flags & TEXTURE_FLAG_MIPMAP) {
            texels += texture->GetSize() / texture->levels;
        } else {
            texels += (texture->GetWidth() * texture->GetHeight());
        }
	}

	unsigned int mb = texels * (Video.bpp / 8) / 0x100000;
	Con_Printf("%i textures %i pixels %1.1f megabytes\n", _textures.size(), texels, mb);
}

Texture *TextureManager::CreateTexture(std::string path, PLuint flags) {
	{
		auto texture = _textures.find(path);
		if (texture != _textures.end())
			return texture->second;
	}

	FILE *f;
	PLImage image;

    if(plLoadImage(path.c_str(), &image) == PL_RESULT_SUCCESS) {
        _textures[path] = CreateTexture(&image, flags);
        _plFreeImage(&image);
        return _textures[path];
    }

	std::string tgapath = path + PLIMAGE_EXTENSION_TGA;
	COM_FOpenFile(tgapath.c_str(), &f);
	if (f) {
		memset(&image, 0, sizeof(PLImage));

		PLuint width, height;
		PLbyte *data = Image_LoadTGA(f, &width, &height);

		fclose(f);

		if (data)
		{
			_textures[path] = CreateTexture(path, width, height, PL_IMAGEFORMAT_RGBA8, data, (width * height * 4), flags);
			free(data);
			return _textures[path];
		}
	}

	Con_Warning("Failed to load texture! (%s)\n", path.c_str());
	return nullptr;
}

Texture* TextureManager::CreateTexture(std::string path, PLuint width, PLuint height, PLImageFormat format,
									   PLbyte *data, PLuint size, PLuint flags)
{
	PLImage image;
	memset(&image, 0, sizeof(PLImage));
	image.data		= data;
	image.format	= format;
	image.width		= width;
	image.height	= height;
	image.size		= size;
	strncpy(image.path, path.c_str(), sizeof(image.path));

	return CreateTexture(&image, flags);
}

Texture *TextureManager::CreateTexture(PLImage *image, PLuint flags)
{
	if (!image || !image->data) throw XException("Invalid image data!\n");

	// Ensure it's a valid size, do we always want to do this?
	if (!plIsValidImageSize(image->width, image->height)) {
	}

	// Add the new texture to our manager.
	Texture *tex = new Texture;
	tex->AddFlags(flags);
	tex->SetImage(image);
	tex->SetCRC(CRC_Block(image->data, image->size));

	return tex;
}

/*	Texture Management	*/

void TextureManager::DeleteTexture(Texture *texture, PLbool force) {
	if (!texture || ((texture->flags & TEXTURE_FLAG_PRESERVE) && !force))
		return;

	// Remove it from the list.
	auto tex = _textures.begin();
	while (tex != _textures.end())
	{
		if (tex->second == texture)
		{
			delete tex->second;
			_textures.erase(tex);
			return;
		}
		++tex;
	}
}

//////////////////////////////////////////////////////////////////////////

Texture::Texture() :
	flags(0),
	_width(8), _height(8),
	_format(PL_IMAGEFORMAT_RGBA8),
	_size(0),
	path(""),
	levels(0),
	_crc(0) {
	instance_ = plCreateTexture();
}

Texture::~Texture() {
	plDeleteTexture(instance_, true);
}

void Texture::SetImage(PLImage *image)
{
	_width		= image->width;
	_height		= image->height;
	_format		= image->format;
	_size		= image->size;
	path		= image->path;

	levels = 1;
	if (flags & TEXTURE_FLAG_MIPMAP)
	{
		plEnableGraphicsStates(VL_CAPABILITY_GENERATEMIPMAP);
		levels = 4;
	}
	
	PLTextureInfo upload;
	memset(&upload, 0, sizeof(PLTextureInfo));
	upload.data			= image->data;
	upload.format		= image->format;
	if (_flags & TEXTURE_FLAG_ALPHA)
		upload.pixel_format = VL_COLOURFORMAT_RGBA;
	else
		upload.pixel_format = VL_COLOURFORMAT_RGB;
	upload.width		= image->width;
	upload.height		= image->height;
	upload.size			= image->size;
	upload.initial		= true;
	upload.levels		= levels;

	plUploadTexture(instance_, &upload);
	
	if (_flags & TEXTURE_FLAG_MIPMAP)
		plDisableGraphicsStates(VL_CAPABILITY_GENERATEMIPMAP);

	PLTextureFilter filtermode = PL_TEXTUREFILTER_LINEAR;
	if (_flags & TEXTURE_FLAG_MIPMAP)
	{
		if (_flags & TEXTURE_FLAG_NEAREST)	filtermode = PL_TEXTUREFILTER_MIPMAP_NEAREST;
		else								filtermode = PL_TEXTUREFILTER_MIPMAP_LINEAR;
	}
	else if(_flags & TEXTURE_FLAG_NEAREST)
		filtermode = PL_TEXTUREFILTER_NEAREST;

	plSetTextureFilter(instance_, filtermode);
	plSetTextureAnisotropy(instance_, (PLuint)cv_texture_anisotropy.iValue);
}
