#include "MaterialEditorBase.h"

#include "MaterialEditorRenderCanvas.h"
#include "MaterialEditorApp.h"

/*
	Render Canvas
*/

wxBEGIN_EVENT_TABLE(CMaterialEditorRenderCanvas, wxGLCanvas)
EVT_PAINT(CMaterialEditorRenderCanvas::OnPaint)
EVT_IDLE(CMaterialEditorRenderCanvas::OnIdle)
wxEND_EVENT_TABLE()

CMaterialEditorRenderCanvas::CMaterialEditorRenderCanvas(wxWindow *parent, int *attribList)
	: wxGLCanvas(
	parent,
	wxID_ANY,
	attribList,
	wxDefaultPosition,
	wxDefaultSize,
	wxFULL_REPAINT_ON_RESIZE)
{
	new CMaterialEditorRenderContext(this);
}

void CMaterialEditorRenderCanvas::OnPaint(wxPaintEvent &event)
{
	wxPaintDC dc(this);
	DrawFrame(dc);
}

void CMaterialEditorRenderCanvas::OnIdle(wxIdleEvent &event)
{
	wxClientDC dc(this);
	DrawFrame(dc);
}

void CMaterialEditorRenderCanvas::DrawFrame(wxDC &dc)
{
	const wxSize clientSize = GetClientSize();
	engine->SetViewportSize(clientSize.x, clientSize.y);

	SwapBuffers();
}

/*
	Render Context
*/

CMaterialEditorRenderContext::CMaterialEditorRenderContext(wxGLCanvas *canvas)
	: wxGLContext(canvas)
{
	SetCurrent(*canvas);

	// Temporary until we get the engine actually rendering here.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}