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

class XTexture
{
public:
	PLbool IsValidSize(PLuint width, PLuint height)
	{
		if ((width && !(width & (width - 1))) && (height && !(height & (height - 1))))
			return true;

		return false;
	}

protected:
	
private:
	PLuint _id;
	PLuint _flags;

	PLuint _width, _height;

	PLchar _name[64];
	PLchar _path[PL_MAX_PATH];

	XTextureFormat _format;
};

//////////////////////////////////////////////////////////////////////////

class XTextureManager : public XManager
{
public:
	XTextureManager();
	~XTextureManager();

	XTexture *Load(const PLchar *path);

protected:
private:
	

	std::unordered_map<std::string, XTexture*> _textures;
};

XTextureManager::XTextureManager()
{
	Con_Printf("Initializing Texture Manager...\n");
}

XTextureManager::~XTextureManager()
{}

