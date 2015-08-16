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

	hSizer->Add(new wxBitmapButton(this, wxID_ANY, iconShapeSphere));
	hSizer->Add(new wxBitmapButton(this, wxID_ANY, iconShapeCube));
	hSizer->Add(new wxBitmapButton(this, wxID_ANY, iconShapePlane));

	hSizer->AddSpacer(2);

	hSizer->Add(new wxBitmapButton(this, wxID_ANY, iconMediaPause));
	hSizer->Add(new wxBitmapButton(this, wxID_ANY, iconMediaPlay));

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

	rcRenderCanvas->DrawFrame();
	rcRenderCanvas->Refresh();
}