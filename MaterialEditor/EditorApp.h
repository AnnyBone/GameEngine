#ifndef __EDITORAPP__
#define	__EDITORAPP__

#include "MaterialEditorFrame.h"

class CEditorApp : public wxApp
{
public:
	virtual bool OnInit();

	virtual int OnExit();

	CMaterialEditorFrame *mainFrame;
};

wxDECLARE_APP(CEditorApp);

#endif // !__EDITORAPP__

