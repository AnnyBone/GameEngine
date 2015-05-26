#ifndef __MATERIALEDITORAPP__
#define	__MATERIALEDITORAPP__

#include "MaterialEditorFrame.h"

class CMaterialEditorApp : public wxApp
{
public:
	virtual bool OnInit();

	virtual int OnExit();

	CMaterialEditorFrame *mainFrame;
};

wxDECLARE_APP(CMaterialEditorApp);

#endif // !__MATERIALEDITORAPP__

