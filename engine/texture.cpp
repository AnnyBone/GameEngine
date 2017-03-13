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
//#include "texture.h"

using namespace xenon::graphics;

ConsoleVariable_t cv_texture_anisotropy = { "texture_anisotropy", "16", true };

namespace xenon {
    namespace graphics {
        TextureManager *texture_manager = nullptr;

        // Base textures...
        PLTexture *nulltexture = nullptr;
    }
}

void _PrintMemoryUsage() {
	texture_manager->PrintMemoryUsage();
}

TextureManager::TextureManager() {
	Con_Printf("Initializing Texture Manager...\n");

	Cvar_RegisterVariable(&cv_texture_anisotropy, NULL);

	Cmd_AddCommand("tm_memoryusage", _PrintMemoryUsage);

    plInitialize(PL_SUBSYSTEM_IMAGE | PL_SUBSYSTEM_GRAPHICS);

	static PLbyte notexture_data[16] = {
		255,  0,255, 255,
		0,  0,  0, 255,
		0,  0,  0, 255,
		255,  0,255, 255
	};
	nulltexture = CreateTexture(
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
	for(auto tex = textures_.begin(); tex != textures_.end(); ++tex) {
		PLTexture *texture = tex->second;
		Con_SafePrintf(" %4i x%4i %s\n", 
			texture->width,
			texture->height,
			texture->path
        );

		// todo, update this crap...
		if (texture->flags & TEXTURE_FLAG_MIPMAP) {
            texels += texture->size / texture->levels;
        } else {
            texels += (texture->width * texture->height);
        }
	}

	unsigned int mb = texels * (Video.bpp / 8) / 0x100000;
	Con_Printf("%i textures %i pixels %1.1f megabytes\n", textures_.size(), texels, mb);
}

PLTexture *TextureManager::CreateTexture(std::string path, PLuint flags) {
	{
		auto texture = textures_.find(path);
		if (texture != textures_.end()) {
            return texture->second;
        }
	}

	FILE *f;
	PLImage image;

    if(plLoadImage(path.c_str(), &image) == PL_RESULT_SUCCESS) {
        textures_[path] = CreateTexture(&image, flags);
        _plFreeImage(&image);
        return textures_[path];
    }

	std::string tgapath = path + PLIMAGE_EXTENSION_TGA;
	COM_FOpenFile(tgapath.c_str(), &f);
	if (f) {
		memset(&image, 0, sizeof(PLImage));

		PLuint width, height;
		PLbyte *data = Image_LoadTGA(f, &width, &height);

		fclose(f);

		if (data) {
			textures_[path] = CreateTexture(path, width, height, PL_IMAGEFORMAT_RGBA8, data, (width * height * 4), flags);
			free(data);
			return textures_[path];
		}
	}

	Con_Warning("Failed to load texture! (%s)\n", path.c_str());
	return nullptr;
}

/*	Texture Management	*/

void TextureManager::DeleteTexture(PLTexture *texture, PLbool force) {
    plDeleteTexture(texture, force);
}

//////////////////////////////////////////////////////////////////////////

#if 0
void Texture::SetImage(PLImage *image)
{
	PLTextureInfo upload;
	memset(&upload, 0, sizeof(PLTextureInfo));
	upload.data			= image->data;
	upload.format		= image->format;
	if (flags & TEXTURE_FLAG_ALPHA)
		upload.pixel_format = PL_COLOURFORMAT_RGBA;
	else
		upload.pixel_format = PL_COLOURFORMAT_RGB;
	upload.width		= image->width;
	upload.height		= image->height;
	upload.size			= image->size;
	upload.initial		= true;
	upload.levels		= info_.levels;

	plUploadTexture(instance_, &upload);
	
	if (flags & TEXTURE_FLAG_MIPMAP) {
        plDisableGraphicsStates(VL_CAPABILITY_GENERATEMIPMAP);
    }

	PLTextureFilter filtermode = PL_TEXTUREFILTER_LINEAR;
	if (flags & TEXTURE_FLAG_MIPMAP)
	{
		if (flags & TEXTURE_FLAG_NEAREST)	filtermode = PL_TEXTUREFILTER_MIPMAP_NEAREST;
		else								filtermode = PL_TEXTUREFILTER_MIPMAP_LINEAR;
	}
	else if(_flags & TEXTURE_FLAG_NEAREST) {
        filtermode = PL_TEXTUREFILTER_NEAREST;
    }

	plSetTextureFilter(instance_, filtermode);
	plSetTextureAnisotropy(instance_, (PLuint)cv_texture_anisotropy.iValue);
}
#endif
