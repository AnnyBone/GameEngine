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

#include "engine_base.h"

#include "video.h"
#include "video_framebuffer.h"

VideoRenderBuffer::VideoRenderBuffer(unsigned int w, unsigned int h)
{
	width	= w;
	height	= h;

	vlGenerateRenderBuffer(&instance);
}

VideoRenderBuffer::~VideoRenderBuffer()
{
	vlDeleteRenderBuffer(&instance);
}

void VideoRenderBuffer::Attach()
{
	vlAttachFrameBufferRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, instance);
}

void VideoRenderBuffer::Bind()
{
	vlBindRenderBuffer(instance);
}

void VideoRenderBuffer::Unbind()
{
	vlBindRenderBuffer(0);
}

void VideoRenderBuffer::Storage(int format, int samples)
{
	vlRenderBufferStorage(format, samples, width, height);
}

/*
	Framebuffer Manager
*/

VideoFrameBufferManager	*g_framebuffermanager;

VideoFrameBufferManager::VideoFrameBufferManager()
{
	fbo = NULL;
}

VideoFrameBufferManager::~VideoFrameBufferManager()
{
	delete[] fbo;
}

void VideoFrameBufferManager::Initialize()
{
	//fbo = new VideoFrameBuffer[1];
}

/*
	Framebuffer Object
*/

VideoFrameBuffer::VideoFrameBuffer(unsigned int w, unsigned int h)
{
	isbound		= false;
	buf_colour	= NULL;
	buf_depth	= NULL;
	width		= w;
	height		= h;

	// Generate an fbo for this object.
	vlGenerateFrameBuffer(&instance);
}

VideoFrameBuffer::~VideoFrameBuffer()
{
	delete buf_depth;

	vlDeleteFrameBuffer(&instance);
}

/*	TODO: Make this more abstract; need more control over what this is doing.
*/
void VideoFrameBuffer::GenerateBuffers()
{
	// Colour buffer
	{
		// Create a new texture instance and then bind it.
		buf_colour = TexMgr_NewTexture();
		if (!buf_colour)
			Sys_Error("Failed to create framebuffer texture!\n");

		Video_SetTexture(buf_colour);

		// Set the texture up.
		vlTexImage2D(VL_TEXTURE_2D, VL_TEXTURE_FORMAT_RGB, VL_TEXTURE_FORMAT_RGB, width, height, NULL);
		vlSetTextureFilter(VL_TEXTURE_FILTER_LINEAR);

		// Attach the texture to this framebuffer.
		vlAttachFrameBufferTexture(buf_colour);
	}

	// Depth buffer
	buf_depth = new VideoRenderBuffer(width, height);
	buf_depth->Bind();
	buf_depth->Storage(GL_DEPTH24_STENCIL8, 0);
	buf_depth->Attach();

	vlCheckFrameBufferStatus();
}

void VideoFrameBuffer::Bind()
{
	if (isbound)
		return;

	vlBindFrameBuffer(VL_FRAMEBUFFER_DEFAULT, instance);

	isbound = true;
}

void VideoFrameBuffer::Unbind()
{
	if (!isbound)
		return;

	vlBindFrameBuffer(VL_FRAMEBUFFER_DEFAULT, 0);

	isbound = false;
}

void VideoFrameBuffer::Draw()
{
	Unbind();

	GL_SetCanvas(CANVAS_DEFAULT);

	Video_SetTexture(buf_colour);

	Draw_Rectangle(0, 0, 512, 512, pl_white); //Video.iWidth, Video.iHeight,
}

VideoFrameBuffer *debug_fbo;

void DEBUG_FrameBufferInitialization()
{
	debug_fbo = new VideoFrameBuffer(512, 512);	// Generate the FBO.
	debug_fbo->Bind();							// Bind it, so it's active.
	debug_fbo->GenerateBuffers();				// TODO: Abstract this stage out.
	debug_fbo->Unbind();
}

void DEBUG_FrameBufferBind()
{
	debug_fbo->Bind();
}

void DEBUG_FrameBufferDraw()
{
	debug_fbo->Draw();
}

// Post Processing Object

VideoPostProcess::VideoPostProcess(const char *fragpath, const char *vertpath)
	: VideoFrameBuffer(Video.iWidth, Video.iHeight)
{
	program = new VideoShaderProgram();
	program->Initialize();

	VideoShader *frag = new VideoShader(VIDEO_SHADER_FRAGMENT);
	if (!frag->Load(fragpath))
		Sys_Error("Failed to load fragment shader! (%s)\n", fragpath);

	VideoShader *vert = new VideoShader(VIDEO_SHADER_VERTEX);
	if (!vert->Load(vertpath))
		Sys_Error("Failed to load vertex shader! (%s)\n", vertpath);

	program->Attach(frag);
	program->Attach(vert);
	program->Link();

	uniform_diffuse = program->GetUniformLocation("SampleTexture");
}

VideoPostProcess::VideoPostProcess(VideoShaderProgram *PostProcessProgram)
	: VideoFrameBuffer(512, 512)	//Video.iWidth, Video.iHeight)
{
	if (!PostProcessProgram)
		throw EngineException("Invalid shader program!\n");

	this->program = PostProcessProgram;
}

void VideoPostProcess::Draw()
{
	Unbind();

	program->Enable();

	Video_SetTexture(buf_colour);

	program->SetVariable(iDiffuseUniform, 0);

	GL_SetCanvas(CANVAS_DEFAULT);

	Draw_Rectangle(0, 0, 512, 512, pl_white); //Video.iWidth, Video.iHeight,

	program->Disable();
}

VideoPostProcess *post_bloom;

void DEBUG_PostProcessInitialization()
{
	post_bloom = new VideoPostProcess("bloom", "base");
}

void DEBUG_PostProcessDraw()
{
	post_bloom->Draw();
}

void DEBUG_PostProcessBind()
{
	post_bloom->Bind();
}

void VideoPostProcess_Initialize() {}
void VideoPostProcess_BindFrameBuffer() {}
void VideoPostProcess_Draw() {}
