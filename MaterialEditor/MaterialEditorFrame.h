#ifndef __MATERIALEDITORFRAME__
#define	__MATERIALEDITORFRAME__

#include "MaterialEditorRenderCanvas.h"

class CMaterialEditorFrame : public wxFrame
{
public:
	CMaterialEditorFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
	~CMaterialEditorFrame();

	void StartRendering(void);
	void StopRendering(void);

	void PrintMessage(char *text);
	void PrintWarning(char *text);
	void PrintError(char *text);

private:
	void OnOpen(wxCommandEvent &event);
	void OnExit(wxCommandEvent &event);
	void OnAbout(wxCommandEvent &event);
	void OnSave(wxCommandEvent &event);
	void OnConsole(wxCommandEvent &event);
	void OnProperties(wxCommandEvent &event);
	void OnTimer(wxTimerEvent &event);

	wxBitmap largeExit;
	wxBitmap largeNew;
	wxBitmap largeOpen;
	wxBitmap largeScriptEdit;

	wxTextCtrl *textConsoleOut;

	wxTimer* timer;

	CMaterialEditorRenderCanvas* engineViewport;

	// Material Properties
	wxPropertyGrid* materialProperties;

	wxAuiManager *manager;

	wxDECLARE_EVENT_TABLE();
};

#endif // !__MATERIALEDITORFRAME__
