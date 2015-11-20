/*	Copyright (C) 2011-2015 OldTimes Software

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

#include "EngineBase.h"

#include "video.h"
#include "video_shader.h"
#include "video_framebuffer.h"

/*
	Framebuffer Manager
*/

VideoFrameBufferManager	*g_framebuffermanager;

VideoFrameBufferManager::VideoFrameBufferManager()
{
	//FrameBufferList = new VideoFrameBuffer[5];
}

VideoFrameBufferManager::~VideoFrameBufferManager()
{
	//delete[] FrameBufferList;
}

/*
	Framebuffer Object
*/

VideoFrameBuffer::VideoFrameBuffer(int Width, int Height)
{
	bIsBound = 0;

	uiFrameBuffer = 0;

	gColourBuffer = NULL;

	VideoLayer_GenerateFrameBuffer(&uiFrameBuffer);

	// Create a new texture instance and then bind it.
	gColourBuffer = TexMgr_NewTexture();
	if (!gColourBuffer)
		throw CEngineException("Failed to create framebuffer texture!\n");

	Video_SetTexture(gColourBuffer);

	// Set the texture up.
	VideoLayer_SetupTexture(VIDEO_TEXTURE_FORMAT_RGB, VIDEO_TEXTURE_FORMAT_RGB, Width, Height);
	VideoLayer_SetTextureFilter(VIDEO_TEXTURE_FILTER_LINEAR);

	// Attach the texture to this framebuffer.
	VideoLayer_AttachFrameBufferTexture(gColourBuffer);
}

VideoFrameBuffer::~VideoFrameBuffer()
{
	VideoLayer_DeleteFrameBuffer(&uiFrameBuffer);
}

void VideoFrameBuffer::Bind()
{
	if (bIsBound)
		return;

	VideoLayer_BindFrameBuffer(VIDEO_FBO_DEFAULT, uiFrameBuffer);

	bIsBound = true;
}

void VideoFrameBuffer::Unbind()
{
	if (!bIsBound)
		return;

	VideoLayer_BindFrameBuffer(VIDEO_FBO_DEFAULT, 0);

	bIsBound = false;
}

// Post Processing Object

VideoPostProcess::VideoPostProcess(const char *FragName, const char *VertName)
	: VideoFrameBuffer(Video.iWidth, Video.iHeight)
{
	program = new VideoShaderProgram();
	program->Initialize();

	VideoShader *FragmentShader = new VideoShader(VIDEO_SHADER_FRAGMENT);
	if (!FragmentShader->Load(FragName))
		Sys_Error("Failed to load fragment shader! (%s)\n", FragName);

	VideoShader *VertexShader = new VideoShader(VIDEO_SHADER_VERTEX);
	if (!VertexShader->Load(VertName))
		Sys_Error("Failed to load vertex shader! (%s)\n", VertName);

	program->Attach(FragmentShader);
	program->Attach(VertexShader);
	program->Link();

	iDiffuseUniform = program->GetUniformLocation("SampleTexture");
}

VideoPostProcess::VideoPostProcess(VideoShaderProgram *PostProcessProgram)
	: VideoFrameBuffer(512, 512)	//Video.iWidth, Video.iHeight)
{
	if (!PostProcessProgram)
		throw CEngineException("Invalid shader program!\n");

	this->program = PostProcessProgram;
}

void VideoPostProcess::Draw()
{
	Unbind();

	program->Enable();

	Video_SetTexture(gColourBuffer);

	program->SetVariable(iDiffuseUniform, 0);

	Draw_ResetCanvas();
	GL_SetCanvas(CANVAS_DEFAULT);
	Draw_Fill(0, 0, 512, 512, 1, 1, 1, 1); //Video.iWidth, Video.iHeight, 1, 1, 1, 1);
	Draw_ResetCanvas();

	program->Disable();
}

VideoPostProcess *post_bloom;

void VideoPostProcess_Initialize()
{
	post_bloom = new VideoPostProcess("bloom", "base");
	if (!post_bloom)
		Sys_Error("Failed to create post process effect!\n");
}

void VideoPostProcess_BindFrameBuffer()
{
	post_bloom->Bind();
}

void VideoPostProcess_Draw()
{
	post_bloom->Draw();
}
