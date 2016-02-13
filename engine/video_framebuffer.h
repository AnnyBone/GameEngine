/*	Copyright (C) 2011-2016 OldTimes Software

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

#pragma once

#include "video_shader.h"

class VideoRenderBuffer
{
public:
	VideoRenderBuffer(unsigned int w = 512, unsigned int h = 512);
	~VideoRenderBuffer();

	void Attach();
	void Bind();
	void Unbind();
	void Storage(int format, int samples);
	void SetWidth(unsigned int w) { width = w; }
	void SetHeight(unsigned int h) { height = h; }

	unsigned int GetInstance() { return instance; }

protected:
private:
	unsigned int instance;

	unsigned int width, height;
};

class VideoFrameBuffer
{
public:
	VideoFrameBuffer(unsigned int w = 512, unsigned int h = 512);
	~VideoFrameBuffer();

	void GenerateBuffers();
	void Bind();
	void Unbind();

	virtual void Draw();

protected:
	gltexture_t *buf_colour;	// Colour buffer.

	VideoRenderBuffer *buf_depth;

private:
	// GL instance of the fbo.
	unsigned int instance;

	// Width and height of the framebuffer.
	unsigned int width, height;

	bool isbound;
};

class VideoFrameBufferManager
{
public:
	VideoFrameBufferManager();
	~VideoFrameBufferManager();

	void Initialize();

private:
	VideoFrameBuffer *fbo;
};

extern VideoFrameBufferManager *g_framebuffermanager;

class VideoPostProcess : public VideoFrameBuffer
{
public:
	VideoPostProcess(VideoShaderProgram *PostProcessProgram);
	VideoPostProcess(const char *fragpath, const char *vertpath);

	//virtual void Initialize();
	virtual void Draw();

private:
	unsigned int uniform_diffuse;

	VideoShaderProgram *program;
};

