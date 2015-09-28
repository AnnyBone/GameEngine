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

#include "EngineBase.h"

#include "EngineVideo.h"
#include "EngineVideoShader.h"
#include "EngineVideoFrameBuffer.hpp"

// FrameBuffer Manager

CVideoFrameBufferManager::CVideoFrameBufferManager()
{
	//FrameBufferList = new CVideoFrameBuffer[5];
}

CVideoFrameBufferManager::~CVideoFrameBufferManager()
{
	//delete[] FrameBufferList;
}

// FrameBuffer Object

CVideoFrameBuffer::CVideoFrameBuffer(int Width, int Height)
{
	bIsBound = 0;

	uiFrameBuffer = 0;

	gColourBuffer = nullptr;

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

CVideoFrameBuffer::~CVideoFrameBuffer()
{
	VideoLayer_DeleteFrameBuffer(&uiFrameBuffer);
}

void CVideoFrameBuffer::Bind()
{
	if (bIsBound)
		return;

	VideoLayer_BindFrameBuffer(VIDEO_FBO_DEFAULT, uiFrameBuffer);

	bIsBound = true;
}

void CVideoFrameBuffer::Unbind()
{
	if (!bIsBound)
		return;

	VideoLayer_BindFrameBuffer(VIDEO_FBO_DEFAULT, 0);

	bIsBound = false;
}

// Post Processing Object

CVideoPostProcess::CVideoPostProcess(const char *FragName, const char *VertName)
	: CVideoFrameBuffer(Video.iWidth, Video.iHeight)
{
	PostProcessProgram = new CVideoShaderProgram();

	CVideoShader *FragmentShader = new CVideoShader(FragName, VIDEO_SHADER_FRAGMENT);
	CVideoShader *VertexShader = new CVideoShader(VertName, VIDEO_SHADER_VERTEX);

	PostProcessProgram->Attach(FragmentShader);
	PostProcessProgram->Attach(VertexShader);
	PostProcessProgram->Link();

	iDiffuseUniform = PostProcessProgram->GetUniformLocation("SampleTexture");
}

CVideoPostProcess::CVideoPostProcess(CVideoShaderProgram *PostProcessProgram)
	: CVideoFrameBuffer(512, 512)	//Video.iWidth, Video.iHeight)
{
	if (!PostProcessProgram)
		throw CEngineException("Invalid shader program!\n");

	this->PostProcessProgram = PostProcessProgram;
}

void CVideoPostProcess::Draw()
{
	Unbind();

	PostProcessProgram->Enable();

	Video_SetTexture(gColourBuffer);

	PostProcessProgram->SetVariable(iDiffuseUniform, 0);

	Draw_ResetCanvas();
	GL_SetCanvas(CANVAS_DEFAULT);
	Draw_Fill(0, 0, 512, 512, 1, 1, 1, 1); //Video.iWidth, Video.iHeight, 1, 1, 1, 1);
	Draw_ResetCanvas();

	PostProcessProgram->Disable();
}

CVideoPostProcess *PostEffects;

void VideoPostProcess_Initialize()
{
	PostEffects = new CVideoPostProcess("bloom", "base");
	if (!PostEffects)
		Sys_Error("Failed to create post process effect!\n");
}

void VideoPostProcess_BindFrameBuffer()
{
	PostEffects->Bind();
}

void VideoPostProcess_Draw()
{
	PostEffects->Draw();
}
