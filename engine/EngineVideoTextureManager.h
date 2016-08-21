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

//gl_texmgr.h -- fitzquake's texture manager. manages opengl texture images

#define TEXPREF_NONE			0x0000
#define TEXPREF_MIPMAP			0x0001	// generate mipmaps
#define TEXPREF_LINEAR			0x0002	// force linear
#define TEXPREF_NEAREST			0x0004	// force nearest
#define TEXPREF_ALPHA			0x0008	// allow alpha
#define TEXPREF_PAD				0x0010	// allow padding
#define TEXPREF_PERSIST			0x0020	// never free
#define TEXPREF_OVERWRITE		0x0040	// overwrite existing same-name texture
#define TEXPREF_NOPICMIP		0x0080	// always load full-sized
#define TEXPREF_FULLBRIGHT		0x0100	// use fullbright mask palette
#define TEXPREF_NOBRIGHT		0x0200	// use nobright mask palette
#define TEXPREF_CONCHARS		0x0400	// use conchars palette
#define TEXPREF_WARPIMAGE		0x0800	// resize this texture when warpimagesize changes

#define	LIGHTMAP_BYTES	4

#include "shared_texture.h"

#ifdef __cplusplus
extern "C" {
#endif

	extern gltexture_t *notexture;

	extern unsigned int d_8to24table[256];
	extern unsigned int d_8to24table_fbright[256];
	extern unsigned int d_8to24table_nobright[256];
	extern unsigned int d_8to24table_conchars[256];
	extern unsigned int d_8to24table_shirt[256];
	extern unsigned int d_8to24table_pants[256];

	// TEXTURE MANAGER
	float TexMgr_FrameUsage(void);
	gltexture_t *TexMgr_FindTexture(model_t *owner, char *name);
	gltexture_t *TexMgr_GetTexture(const char *ccPath);
	void TexMgr_SetFilterModes(gltexture_t *glt);
	void TexMgr_FreeTexture(gltexture_t *kill);
	void TextureManager_FreeTextures(unsigned int flags, unsigned int mask);
	void TexMgr_FreeTexturesForOwner(model_t *owner);

	// IMAGE LOADING
	gltexture_t *TexMgr_LoadImage(model_t *owner, char *name, int width, int height, enum XTextureFormat format,
		uint8_t *data, char *source_file, unsigned int source_offset, unsigned int flags);
	void TexMgr_ReloadImage(gltexture_t *glt, int shirt, int pants);
	void TexMgr_ReloadImages(void);

	int TexMgr_Pad(int s);
	unsigned int TexMgr_SafeTextureSize(int s);
	unsigned int TexMgr_PadConditional(unsigned int s);

#ifdef __cplusplus
};
#endif
