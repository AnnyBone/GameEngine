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
	CEditorRenderCanvas *rcRenderCanvas;
};

#endif

