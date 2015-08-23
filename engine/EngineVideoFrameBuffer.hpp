#ifndef __ENGINEVIDEOFRAMEBUFFER__
#define	__ENGINEVIDEOFRAMEBUFFER__
#pragma once

class CVideoFrameBuffer
{
public:
	CVideoFrameBuffer(int Width, int Height);
	~CVideoFrameBuffer();

	void Bind();
	void Unbind();

protected:
	gltexture_t *gColourBuffer;

private:
	unsigned int uiFrameBuffer;
};

class CVideoFrameBufferManager
{
public:
	CVideoFrameBufferManager();
	~CVideoFrameBufferManager();

private:
	CVideoFrameBuffer *FrameBufferList;
};

class CVideoPostProcess : public CVideoFrameBuffer
{
public:
	CVideoPostProcess(CVideoShaderProgram *PostProcessProgram);
	CVideoPostProcess(const char *FragName, const char *VertName);

	void Draw();

private:
	CVideoShaderProgram *PostProcessProgram;
};

#endif

