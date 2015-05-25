#include "MaterialEditorBase.h"

#include "MaterialEditorRenderCanvas.h"
#include "MaterialEditorApp.h"

/*
	Render Canvas
*/

wxBEGIN_EVENT_TABLE(CMaterialEditorRenderCanvas, wxGLCanvas)
EVT_PAINT(CMaterialEditorRenderCanvas::OnPaint)
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
	const wxSize clientSize = GetClientSize();
	engine->SetViewportSize(clientSize.x, clientSize.y);

	wxPaintDC dc(this);
	DrawFrame();
}

void CMaterialEditorRenderCanvas::DrawFrame(void)
{
	engine->Video_PreFrame();
	engine->DrawModel();
	engine->ResetCanvas();
	engine->DrawFPS();
	engine->Video_PostFrame();
	
	SwapBuffers();
}

wxBEGIN_EVENT_TABLE(CMaterialEditorEngineConsoleCanvas, wxGLCanvas)
EVT_PAINT(CMaterialEditorEngineConsoleCanvas::OnPaint)
wxEND_EVENT_TABLE()

CMaterialEditorEngineConsoleCanvas::CMaterialEditorEngineConsoleCanvas(wxWindow *parent, int *attribList)
	: wxGLCanvas(
	parent,
	wxID_ANY,
	attribList,
	wxDefaultPosition,
	wxDefaultSize,
	wxFULL_REPAINT_ON_RESIZE)
{}

void CMaterialEditorEngineConsoleCanvas::OnPaint(wxPaintEvent &event)
{
	const wxSize clientSize = GetClientSize();
	engine->SetViewportSize(clientSize.x, clientSize.y);
}

void CMaterialEditorEngineConsoleCanvas::DrawFrame(void)
{
	engine->Video_PreFrame();
	engine->DrawConsole();
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