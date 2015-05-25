#ifndef __MATERIALEDITORFRAME__
#define	__MATERIALEDITORFRAME__

#include "MaterialEditorRenderCanvas.h"

class CMaterialEditorFrame : public wxFrame
{
public:
	CMaterialEditorFrame(const wxString &title, const wxPoint &pos, const wxSize &size);

	void StartRendering(void);
	void StopRendering(void);

private:
	void OnOpen(wxCommandEvent &event);
	void OnExit(wxCommandEvent &event);
	void OnAbout(wxCommandEvent &event);
	void OnSave(wxCommandEvent &event);
	void OnConsole(wxCommandEvent &event);
	void OnProperties(wxCommandEvent &event);
	void OnTimer(wxTimerEvent &event);

	wxBitmap largeExit;
	wxBitmap largeOpen;

	wxTimer* timer;

	CMaterialEditorRenderCanvas *engineViewport;
	CMaterialEditorEngineConsoleCanvas *engineConsoleViewport;

	// Material Properties
	wxPropertyGrid* materialProperties;


	wxDECLARE_EVENT_TABLE();
};

#endif // !__MATERIALEDITORFRAME__