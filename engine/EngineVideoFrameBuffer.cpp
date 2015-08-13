#include "EngineBase.h"

#include "EngineVideoFramebuffer.h"
#include "EngineVideo.h"

CVideoFrameBuffer::CVideoFrameBuffer()
{
	VideoLayer_GenerateFrameBuffer(&uiFrameBuffer);
}

CVideoFrameBuffer::~CVideoFrameBuffer()
{
	VideoLayer_DeleteFrameBuffer(&uiFrameBuffer);
}

void CVideoFrameBuffer::Bind()
{
	VideoLayer_BindFrameBuffer(VIDEO_FBO_DEFAULT, uiFrameBuffer);
}
