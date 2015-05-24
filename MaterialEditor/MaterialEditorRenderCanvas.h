#ifndef __MATERIALEDITORRENDERCANVAS__
#define	__MATERIALEDITORRENDERCANVAS__

class CMaterialEditorRenderCanvas : public wxGLCanvas
{
public:
	CMaterialEditorRenderCanvas(wxWindow *parent, int *attribList = NULL);

private:
	void OnPaint(wxPaintEvent &event);
	void OnIdle(wxIdleEvent &event);

	void DrawFrame(wxDC &dc);

	wxDECLARE_EVENT_TABLE();
};

class CMaterialEditorRenderContext : public wxGLContext
{
public:
	CMaterialEditorRenderContext(wxGLCanvas *canvas);
};

#endif // !__MATERIALEDITORRENDERCANVAS__

