#include "EditorBase.h"

#include "EditorRenderCanvas.h"
#include "EditorApp.h"

/*
	Render Canvas
*/

wxBEGIN_EVENT_TABLE(CEditorRenderCanvas, wxGLCanvas)
EVT_PAINT(CEditorRenderCanvas::OnPaint)
wxEND_EVENT_TABLE()

CEditorRenderContext *rcGlobalRenderContext;

CEditorRenderCanvas::CEditorRenderCanvas(wxWindow *parent, int *attribList)
	: wxGLCanvas(
	parent,
	wxID_ANY,
	attribList,
	wxDefaultPosition,
	wxDefaultSize,
	wxFULL_REPAINT_ON_RESIZE)
{
	if(!rcGlobalRenderContext) 
		rcGlobalRenderContext = new CEditorRenderContext(this);
}

void CEditorRenderCanvas::OnPaint(wxPaintEvent &event)
{
	SetCurrent();

	const wxSize clientSize = GetClientSize();
	engine->SetViewportSize(clientSize.GetWidth(), clientSize.GetHeight());

	wxPaintDC dc(this);
	DrawFrame();
}

void CEditorRenderCanvas::DrawFrame(void)
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

CEditorRenderContext::CEditorRenderContext(wxGLCanvas *canvas)
	: wxGLContext(canvas)
{
	SetCurrent(*canvas);

	// Temporary until we get the engine actually rendering here.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}