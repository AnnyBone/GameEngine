#include "EditorBase.h"

#include "EditorRenderCanvas.h"
#include "EditorApp.h"

/*
	Render Canvas
*/

wxBEGIN_EVENT_TABLE(CEditorRenderCanvas, wxGLCanvas)
EVT_PAINT(CEditorRenderCanvas::OnPaint)
wxEND_EVENT_TABLE()

CEditorRenderContext *rcGlobalRenderContext = NULL;

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
	rcGlobalRenderContext->SetCurrent(*this);

	UpdateViewportSize();

	wxPaintDC dc(this);

	DrawFrame();
}

void CEditorRenderCanvas::UpdateViewportSize()
{
	const wxSize clientSize = GetClientSize();
	engine->SetViewportSize(clientSize.GetWidth(), clientSize.GetHeight());
}

void CEditorRenderCanvas::DrawFrame(void)
{
#if 0
	engine->Video_PreFrame();

	engine->DrawGradientBackground();
	engine->DrawResetCanvas();
	
	engine->DrawModel();

	engine->DrawFPS();

	engine->Video_PostFrame();
#endif

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
