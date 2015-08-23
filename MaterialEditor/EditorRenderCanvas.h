#ifndef __EDITORRENDERCANVAS__
#define	__EDITORRENDERCANVAS__

class CEditorRenderCanvas : public wxGLCanvas
{
public:
	CEditorRenderCanvas(wxWindow *parent, int *attribList = NULL);

	void DrawFrame(void);
	void UpdateViewportSize();

private:
	void OnPaint(wxPaintEvent &event);

	wxDECLARE_EVENT_TABLE();
};

class CEditorRenderContext : public wxGLContext
{
public:
	CEditorRenderContext(wxGLCanvas *canvas);
};

extern CEditorRenderContext *rcGlobalRenderContext;

#endif // !__EDITORRENDERCANVAS__

