/*	DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
	Version 2, December 2004

	Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

	Everyone is permitted to copy and distribute verbatim or modified
	copies of this license document, and changing it is allowed as long
	as the name is changed.

	DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
	TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

	0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#include "EditorBase.h"

#include "base_rendercanvas.h"
#include "base_viewportpanel.h"
#include "editor.h"

/*
	Render Canvas
*/

wxBEGIN_EVENT_TABLE(BaseDrawCanvas, wxGLCanvas)
#if 0
EVT_PAINT(BaseDrawCanvas::OnPaint)
#endif
wxEND_EVENT_TABLE()

EditorDrawContext *g_rendercontext = NULL;

BaseDrawCanvas::BaseDrawCanvas(wxWindow *parent, int *attribList)
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

void BaseDrawCanvas::OnPaint(wxPaintEvent &event)
{
#if 0	// TODO: I want this back in at some point...
	g_rendercontext->SetCurrent(*this);

	UpdateViewportSize();

	wxPaintDC dc(this);

	Draw();

	SwapBuffers();
#endif
}

void BaseDrawCanvas::UpdateViewportSize()
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
