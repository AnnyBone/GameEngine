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

class XTexture;

class XTextureManager
{
public:
	XTextureManager();
	~XTextureManager();

	PLbool IsValidSize(PLuint width, PLuint height);

	XTexture *CreateTexture(std::string path, PLuint flags = 0);
	XTexture *CreateTexture(std::string path, PLuint width, PLuint height, VLTextureFormat format, PLbyte *data, PLuint size, PLuint flags = 0);
	XTexture *CreateTexture(PLImage *image, PLuint flags = 0);

	void PrintMemoryUsage();

	XTexture *GetTexture(std::string path);
	XTexture *GetTexture(PLushort crc);

	void DeleteTexture(XTexture *texture, PLbool force = false);

protected:
private:

	PLuint _max_resolution; // Maximum supported resolution.

	std::map<std::string, XTexture*> _textures;
};

class XTexture
{
public:
	XTexture();
	~XTexture();

	void SetImage(PLImage *image);

	PLuint GetWidth() const { return _width; }
	PLuint GetHeight() const { return _height; }
	PLuint GetSize() const { return _size; }

	PLuint GetFlags() const { return _flags; }
	void AddFlags(PLuint flags);
	void RemoveFlags(PLuint flags);
	void SetFlags(PLuint flags) { _flags = flags; }
	void ClearFlags() { _flags = 0; }

	PLushort GetCRC() const { return _crc; }
	void SetCRC(PLushort crc) { _crc = crc; }

	void Bind();

	PLint levels;

	std::string path;

protected:

private:
	VLTexture _id;

	PLuint _flags;
	PLuint _width, _height;

	PLuint _size;

	PLushort _crc;

	VLTextureFormat _format;
};

namespace textures
{
	extern XTexture *nulltexture;
}

extern XTextureManager *g_texturemanager;