#include "MaterialEditorBase.h"

#include "MaterialEditorRenderCanvas.h"
#include "MaterialEditorApp.h"

/*
	Render Canvas
*/

wxBEGIN_EVENT_TABLE(CMaterialEditorRenderCanvas, wxGLCanvas)
EVT_PAINT(CMaterialEditorRenderCanvas::OnPaint)
wxEND_EVENT_TABLE()

CMaterialEditorRenderContext *mainContext;

CMaterialEditorRenderCanvas::CMaterialEditorRenderCanvas(wxWindow *parent, int *attribList)
	: wxGLCanvas(
	parent,
	wxID_ANY,
	attribList,
	wxDefaultPosition,
	wxDefaultSize,
	wxFULL_REPAINT_ON_RESIZE)
{
	mainContext = new CMaterialEditorRenderContext(this);
}

void CMaterialEditorRenderCanvas::OnPaint(wxPaintEvent &event)
{
	const wxSize clientSize = GetClientSize();
	engine->SetViewportSize(clientSize.GetWidth(), clientSize.GetHeight());

	wxPaintDC dc(this);
	DrawFrame();
}

void CMaterialEditorRenderCanvas::DrawFrame(void)
{
	if (!engine->IsRunning())
		return;

	engine->Video_PreFrame();
	engine->DrawModel();
	engine->ResetCanvas();
	engine->DrawFPS();
	engine->Video_PostFrame();
	
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