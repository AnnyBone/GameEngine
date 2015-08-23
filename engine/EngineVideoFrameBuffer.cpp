#include "EngineBase.h"

#include "EngineVideo.h"
#include "EngineVideoShader.h"
#include "EngineVideoFramebuffer.hpp"

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
	VideoLayer_GenerateFrameBuffer(&uiFrameBuffer);

	// Create a new texture instance and then bind it.
	gColourBuffer = TexMgr_NewTexture();
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
	VideoLayer_BindFrameBuffer(VIDEO_FBO_DEFAULT, uiFrameBuffer);
}

void CVideoFrameBuffer::Unbind()
{
	VideoLayer_BindFrameBuffer(VIDEO_FBO_DEFAULT, 0);
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
}

CVideoPostProcess::CVideoPostProcess(CVideoShaderProgram *PostProcessProgram)
	: CVideoFrameBuffer(800, 600)	//Video.iWidth, Video.iHeight)
{
	if (!PostProcessProgram)
	{ 
		// TODO: Error handling...
	}

	this->PostProcessProgram = PostProcessProgram;
}

void CVideoPostProcess::Draw()
{
	Unbind();

	Video_ResetCapabilities(false);

	PostProcessProgram->Enable();

	Video_SetTexture(gColourBuffer);

	PostProcessProgram->SetVariable("SampleTexture", 0);

	Draw_ResetCanvas();
	GL_SetCanvas(CANVAS_DEFAULT);
	Draw_Fill(0, 0, 512, 512, 1, 1, 1, 1); //Video.iWidth, Video.iHeight, 1, 1, 1, 1);
	Draw_ResetCanvas();

	PostProcessProgram->Disable();

	Video_ResetCapabilities(true);
}

CVideoPostProcess *PostEffects;

void VideoPostProcess_Initialize()
{
	PostEffects = new CVideoPostProcess("bloom", "base");
}

void VideoPostProcess_BindFrameBuffer()
{
	PostEffects->Bind();
}

void VideoPostProcess_Draw()
{
	PostEffects->Draw();
}