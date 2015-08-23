/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
	Copyright (C) 2011-2015 OldTimes Software

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

#ifndef __ENGINEVIDEOLAYER__
#define	__ENGINEVIDEOLAYER__

typedef enum
{
	VIDEO_FBO_DEFAULT,

	VIDEO_FBO_DRAW,
	VIDEO_FBO_READ
} VideoFBOTarget_t;

#if __cplusplus
extern "C" {
#endif

	char *VideoLayer_GetErrorMessage(void);

	void VideoLayer_Enable(unsigned int uiCapabilities);
	void VideoLayer_Disable(unsigned int uiCapabilities);
	void VideoLayer_GenerateVertexBuffer(unsigned int *uiBuffer);
	void VideoLayer_GenerateFrameBuffer(unsigned int *uiBuffer);
	void VideoLayer_BindFrameBuffer(VideoFBOTarget_t vtTarget, unsigned int uiBuffer);
	void VideoLayer_DeleteVertexBuffer(unsigned int *uiBuffer);
	void VideoLayer_DeleteFrameBuffer(unsigned int *uiBuffer);

#if __cplusplus
};
#endif

#endif