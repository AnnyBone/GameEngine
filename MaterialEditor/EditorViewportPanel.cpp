/*	Copyright (C) 2011-2015 OldTimes Software

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

#include "EditorViewportPanel.h"

wxBEGIN_EVENT_TABLE(CEditorViewportPanel, wxPanel)
	
EVT_TIMER(-1, CEditorViewportPanel::OnTimer)

wxEND_EVENT_TABLE()

CEditorViewportPanel::CEditorViewportPanel(wxWindow *wParent)
	: wxPanel(wParent, wxID_ANY, wxDefaultPosition, wxSize(512,512))
{
	Show();

	tRenderTimer = new wxTimer(this);
	tRenderTimer->Start(25);

	wxBoxSizer *vSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *hSizer = new wxBoxSizer(wxHORIZONTAL);

	bAutoToggle = new wxBitmapToggleButton(
		this, 
		wxID_ANY, 
		bSmallAuto,
		wxDefaultPosition,
		wxSize(24,24));
	bAutoToggle->SetValue(true);
	hSizer->Add(bAutoToggle);

	vSizer->Add(hSizer, 0, wxTOP | wxLEFT | wxRIGHT);

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

	rcRenderCanvas = new CEditorRenderCanvas(this, attributes);

	vSizer->Add(rcRenderCanvas, 1, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT);

	SetSizer(vSizer);
}

CEditorViewportPanel::~CEditorViewportPanel()
{
	tRenderTimer->Stop();
}

void CEditorViewportPanel::OnTimer(wxTimerEvent &event)
{
	if (!engine->IsRunning())
		return;

	rcGlobalRenderContext->SetCurrent(*rcRenderCanvas);

	// Ensure we render at the correct size (multiple viewports).
	// TODO: Check if there are actually multiple viewports before doing this?
	rcRenderCanvas->UpdateViewportSize();

	engine->Video_PreFrame();
	Draw();
	engine->Video_PostFrame();

	// Cleanup
	rcRenderCanvas->SwapBuffers();
	rcRenderCanvas->Refresh();
}

void CEditorViewportPanel::Draw() {}