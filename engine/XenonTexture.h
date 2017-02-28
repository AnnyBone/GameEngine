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

#include "platform_image.h"

class Texture;

class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	PLbool IsValidSize(PLuint width, PLuint height);

	Texture *CreateTexture(std::string path, PLuint flags = 0);
	Texture *CreateTexture(std::string path, PLuint width, PLuint height, PLImageFormat format, PLbyte *data, PLuint size, PLuint flags = 0);
	Texture *CreateTexture(PLImage *image, PLuint flags = 0);

	void PrintMemoryUsage();

#if 0
	Texture *GetTexture(std::string path);
	Texture *GetTexture(PLushort crc);
#endif

	void DeleteTexture(Texture *texture, PLbool force = false);

protected:
private:

	PLuint max_resolution_; // Maximum supported resolution.

	std::map<std::string, Texture*> _textures;
};

class Texture
{
public:
	Texture();
	~Texture();

	void SetImage(PLImage *image);

	unsigned int GetWidth() const { return _width; }
	unsigned int GetHeight() const { return _height; }
	unsigned int GetSize() const { return _size; }

	unsigned short GetCRC() const { return _crc; }
	void SetCRC(unsigned short crc) { _crc = crc; }

	void Bind();
	void Unbind();

	unsigned int levels;
    unsigned int flags;

	std::string path;

protected:

private:
    PLTexture instance_;

	unsigned int _width, _height;
	unsigned int _size;

	unsigned short _crc;

	PLImageFormat _format;
};

namespace textures
{
	extern Texture *nulltexture;
}

extern TextureManager *g_texturemanager;