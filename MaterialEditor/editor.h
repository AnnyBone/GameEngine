#ifndef EDITOR_H
#define	EDITOR_H

#include "EditorFrame.h"

class CEditorApp : public wxApp
{
public:
	virtual bool OnInit();

	virtual int OnExit();

	CEditorFrame *efMainFrame;
};

wxDECLARE_APP(CEditorApp);

#endif // !EDITOR_H

