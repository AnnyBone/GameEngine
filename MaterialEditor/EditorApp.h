#ifndef __EDITORAPP__
#define	__EDITORAPP__

#include "EditorFrame.h"

class CEditorApp : public wxApp
{
public:
	virtual bool OnInit();

	virtual int OnExit();

	CEditorFrame *efMainFrame;
};

wxDECLARE_APP(CEditorApp);

#endif // !__EDITORAPP__

