#include "EditorBase.h"

#include "EditorRenderCanvas.h"
#include "EditorApp.h"

/*
	Render Canvas
*/

wxBEGIN_EVENT_TABLE(EditorDrawCanvas, wxGLCanvas)
#if 0
EVT_PAINT(EditorDrawCanvas::OnPaint)
#endif
wxEND_EVENT_TABLE()

EditorDrawContext *g_rendercontext = NULL;

EditorDrawCanvas::EditorDrawCanvas(wxWindow *parent, int *attribList)
	: wxGLCanvas(
	parent,
	wxID_ANY,
	attribList,
	wxDefaultPosition,
	wxDefaultSize)
{
	if (!g_rendercontext)
		g_rendercontext = new EditorDrawContext(this);
}

void EditorDrawCanvas::OnPaint(wxPaintEvent &event)
{
#if 0
	g_rendercontext->SetCurrent(*this);

	UpdateViewportSize();

	wxPaintDC dc(this);

	Draw();

	SwapBuffers();
#endif
}

void EditorDrawCanvas::UpdateViewportSize()
{
	const wxSize clientSize = GetClientSize();
	width = clientSize.GetWidth();
	height = clientSize.GetHeight();
	engine->SetViewportSize(width, height);
}

/*
	Render Context
*/

EditorDrawContext::EditorDrawContext(wxGLCanvas *canvas)
	: wxGLContext(canvas)
{
	SetCurrent(*canvas);

	// Temporary until we get the engine actually rendering here.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
