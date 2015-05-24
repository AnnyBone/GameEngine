#ifndef __MATERIALEDITORAPP__
#define	__MATERIALEDITORAPP__

class CMaterialEditorApp : public wxApp
{
public:
	virtual bool OnInit();

	virtual int OnExit();
};

wxDECLARE_APP(CMaterialEditorApp);

#endif // !__MATERIALEDITORAPP__

