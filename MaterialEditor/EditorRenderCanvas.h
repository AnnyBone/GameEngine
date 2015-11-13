#ifndef VIEWPORT_CANVAS_H
#define	VIEWPORT_CANVAS_H

class EditorDrawCanvas : public wxGLCanvas
{
public:
	EditorDrawCanvas(wxWindow *parent, int *attribList = NULL);

	void UpdateViewportSize();

	int GetWidth() 
	{
		return width;
	}
	int GetHeight()
	{
		return height;
	}

private:
	void OnPaint(wxPaintEvent &event);

	int width, height;

	wxDECLARE_EVENT_TABLE();
};

class EditorDrawContext : public wxGLContext
{
public:
	EditorDrawContext(wxGLCanvas *canvas);
};

extern EditorDrawContext *g_rendercontext;

#endif // !VIEWPORT_CANVAS_H

