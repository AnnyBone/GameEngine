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

wxBEGIN_EVENT_TABLE(BaseViewportPanel, wxPanel)

EVT_TIMER(-1, BaseViewportPanel::OnTimer)

EVT_CHAR_HOOK(BaseViewportPanel::OnKey)

wxEND_EVENT_TABLE()

BaseViewportPanel::BaseViewportPanel(wxWindow *wParent, const wxSize &size)
	: wxPanel(
		wParent,
		wxID_ANY,
		wxDefaultPosition,
		size,
		wxFULL_REPAINT_ON_RESIZE) 
{
	timer = new wxTimer(this);
}

BaseViewportPanel::~BaseViewportPanel()
{
	timer->Stop();
}

void BaseViewportPanel::Initialize()
{
	CreateDrawCanvas(this);

	wxBoxSizer *vertical = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *horizontal = new wxBoxSizer(wxHORIZONTAL);

	bAutoToggle = new wxBitmapToggleButton(
		this,
		wxID_ANY,
		bSmallAuto,
		wxDefaultPosition,
		wxSize(24, 24));
	bAutoToggle->SetValue(true);
	horizontal->Add(bAutoToggle);

	vertical->Add(horizontal, 0, wxTOP | wxLEFT | wxRIGHT);
	vertical->Add(GetDrawCanvas(), 1, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT);

	SetSizer(vertical);
}

void BaseViewportPanel::CreateDrawCanvas(wxWindow *parent)
{
	int attributes[] = {
		WX_GL_DEPTH_SIZE, 24,
		WX_GL_STENCIL_SIZE, 8,
		WX_GL_MIN_RED, 8,
		WX_GL_MIN_GREEN, 8,
		WX_GL_MIN_BLUE, 8,
		WX_GL_MIN_ALPHA, 8,
		WX_GL_MIN_ACCUM_RED, 8,
		WX_GL_MIN_ACCUM_GREEN, 8,
		WX_GL_MIN_ACCUM_BLUE, 8,
		WX_GL_MIN_ACCUM_ALPHA, 8,
#if 0
		WX_GL_DOUBLEBUFFER, 1,
#endif
	};
	viewport_canvas = new BaseDrawCanvas(parent, attributes);
}

void BaseViewportPanel::StartDrawing()
{
	timer->Start(25);
}

void BaseViewportPanel::StopDrawing()
{
	timer->Stop();
}

void BaseViewportPanel::OnTimer(wxTimerEvent &event)
{
	if (!IsShown() || !engine->IsRunning())
		return;

	g_rendercontext->SetCurrent(*viewport_canvas);

	// Ensure we render at the correct size (multiple viewports).
	// TODO: Check if there are actually multiple viewports before doing this?
	viewport_canvas->UpdateViewportSize();

	engine->DrawPreFrame();
	Draw();
	engine->DrawPostFrame();

	// Cleanup
	viewport_canvas->SwapBuffers();
	//viewport_canvas->Refresh();
}