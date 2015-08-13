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

