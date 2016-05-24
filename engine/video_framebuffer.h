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
	VideoPostProcess(Core::ShaderProgram *PostProcessProgram);
	VideoPostProcess(const char *fragpath, const char *vertpath);

	//virtual void Initialize();
	virtual void Draw();

private:
	unsigned int uniform_diffuse;

	Core::ShaderProgram *program;
};

