#ifndef __MATERIALEDITORRENDERCANVAS__
#define	__MATERIALEDITORRENDERCANVAS__

class CMaterialEditorRenderCanvas : public wxGLCanvas
{
public:
	CMaterialEditorRenderCanvas(wxWindow *parent, int *attribList = NULL);

	void DrawFrame(void);

private:
	void OnPaint(wxPaintEvent &event);

	wxDECLARE_EVENT_TABLE();
};

class CMaterialEditorEngineConsoleCanvas : public wxGLCanvas
{
public:
	CMaterialEditorEngineConsoleCanvas(wxWindow *parent, int *attribList = NULL);

	void DrawFrame(void);

private:
	void OnPaint(wxPaintEvent &event);

	wxDECLARE_EVENT_TABLE();
};

class CMaterialEditorRenderContext : public wxGLContext
{
public:
	CMaterialEditorRenderContext(wxGLCanvas *canvas);
};

#endif // !__MATERIALEDITORRENDERCANVAS__

