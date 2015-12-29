/*	Copyright (C) 2011-2016 OldTimes Software

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "EditorBase.h"

#include "base_rendercanvas.h"
#include "base_viewportpanel.h"

wxBEGIN_EVENT_TABLE(BaseViewportPanel, wxPanel)
EVT_TIMER(-1, BaseViewportPanel::OnTimer)
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