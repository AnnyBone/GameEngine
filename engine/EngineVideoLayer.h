#ifndef __ENGINEVIDEOLAYER__
#define	__ENGINEVIDEOLAYER__

typedef enum
{
	VIDEO_FBO_DEFAULT,

	VIDEO_FBO_DRAW,
	VIDEO_FBO_READ
} VideoFBOTarget_t;

#if __cplusplus
extern "C" {
#endif

	unsigned int VideoLayer_GenerateVertexBuffer(void);

	void VideoLayer_Enable(unsigned int uiCapabilities);
	void VideoLayer_Disable(unsigned int uiCapabilities);
	void VideoLayer_GenerateFrameBuffer(unsigned int *uiBuffer);
	void VideoLayer_BindFrameBuffer(VideoFBOTarget_t vtTarget, unsigned int uiBuffer);
	void VideoLayer_DeleteVertexBuffer(unsigned int *uiBuffer);
	void VideoLayer_DeleteFrameBuffer(unsigned int *uiBuffer);

#if __cplusplus
};
#endif

#endif