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
#ifdef VL_MODE_OPENGL
	vlAttachFrameBufferRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, instance);
#endif
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
	//	vlTexImage2D(VL_TEXTUREFORMAT_RGB, VL_TEXTUREFORMAT_RGB, width, height, NULL);
		vlSetTextureFilter(buf_colour->texnum, VL_TEXTUREFILTER_LINEAR);

		// Attach the texture to this framebuffer.
		vlAttachFrameBufferTexture(buf_colour);
	}

	// Depth buffer
#ifdef VL_MODE_OPENGL
	buf_depth = new VideoRenderBuffer(width, height);
	buf_depth->Bind();
	buf_depth->Storage(GL_DEPTH24_STENCIL8, 0);
	buf_depth->Attach();
#endif

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
	: VideoFrameBuffer(0, 0)
{
#if 0
	program = new Core::ShaderProgram();
	program->Initialize();

	Core::Shader *frag = new Core::Shader(VL_SHADER_FRAGMENT);
	if (!frag->Load(fragpath))
		Sys_Error("Failed to load fragment shader! (%s)\n", fragpath);

	Core::Shader *vert = new Core::Shader(VL_SHADER_VERTEX);
	if (!vert->Load(vertpath))
		Sys_Error("Failed to load vertex shader! (%s)\n", vertpath);

	program->Attach(frag);
	program->Attach(vert);
	program->Link();

	uniform_diffuse = program->GetUniformLocation("SampleTexture");
#endif
}

VideoPostProcess::VideoPostProcess(core::ShaderProgram *PostProcessProgram)
	: VideoFrameBuffer(512, 512)	//Video.iWidth, Video.iHeight)
{
	if (!PostProcessProgram)
		throw XException("Invalid shader program!\n");

	this->program = PostProcessProgram;
}

void VideoPostProcess::Draw()
{
	Unbind();

	program->Enable();

	Video_SetTexture(buf_colour);

#if 0
	program->SetUniformVariable(iDiffuseUniform, 0);
#endif

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
