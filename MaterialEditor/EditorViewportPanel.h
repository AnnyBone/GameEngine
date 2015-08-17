#ifndef __EDITORVIEWPORTPANEL__
#define	__EDITORVIEWPORTPANEL__
#pragma once

#include "EditorRenderCanvas.h"

class CEditorViewportPanel :
	public wxPanel
{
public:
	CEditorViewportPanel(wxWindow *wParent);
	~CEditorViewportPanel();

private:

	// Events
	void OnTimer(wxTimerEvent &event);

	CEditorRenderCanvas *rcRenderCanvas;

	wxBitmapToggleButton *bAutoToggle;

	wxTimer *tRenderTimer;

	wxDECLARE_EVENT_TABLE();
};

#endif

