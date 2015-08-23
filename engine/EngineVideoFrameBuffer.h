#ifndef __ENGINEVIDEOFRAMEBUFFER__
#define	__ENGINEVIDEOFRAMEBUFFER__
#pragma once

class CVideoFrameBuffer
{
public:
	CVideoFrameBuffer();
	~CVideoFrameBuffer();

	void Bind();

protected:

private:
	unsigned int uiFrameBuffer;
};

class CVideoFrameBufferManager
{
public:
	CVideoFrameBufferManager();
	~CVideoFrameBufferManager();

protected:
private:
	CVideoFrameBuffer *FrameBufferList;
};

#endif

