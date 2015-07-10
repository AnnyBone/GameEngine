#ifndef __MATERIALEDITORFRAME__
#define	__MATERIALEDITORFRAME__

#include "MaterialEditorRenderCanvas.h"
#include "MaterialEditorMaterialProperties.h"

class CMaterialEditorFrame : public wxFrame
{
public:
	CMaterialEditorFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
	~CMaterialEditorFrame();

	void StartEngineLoop(void);
	void StopEngineLoop(void);

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
	void OnCommand(wxCommandEvent &event);
	void OnReload(wxCommandEvent &event);
	void OnPause(wxCommandEvent &event);
	void OnPlay(wxCommandEvent &event);
	void OnKey(wxKeyEvent &event);
	void OnTimer(wxTimerEvent &event);

	wxBitmap largeNew;
	wxBitmap largeOpen;
	wxBitmap largeScriptEdit;
	wxBitmap iconDocumentSave;
	wxBitmap iconMediaPause;
	wxBitmap iconMediaPlay;
	wxBitmap iconViewRefresh;
	wxBitmap iconShapeCube;
	wxBitmap iconShapeSphere;
	wxBitmap iconShapePlane;

	wxTextCtrl *textConsoleOut;
	wxTextCtrl *textConsoleIn;

	wxTimer* timer;

	wxAuiManager *manager;

	CMaterialEditorRenderCanvas *editorViewport;
	CMaterialEditorMaterialGlobalProperties *editorMaterialProperties;

	wxDECLARE_EVENT_TABLE();
};

#endif // !__MATERIALEDITORFRAME__
