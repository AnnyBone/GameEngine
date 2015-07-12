#ifndef __MATERIALEDITORFRAME__
#define	__MATERIALEDITORFRAME__

#include "MaterialEditorRenderCanvas.h"
#include "MaterialEditorMaterialProperties.h"
#include "MaterialEditorConsolePanel.h"

class CMaterialEditorFrame : public wxFrame
{
public:
	CMaterialEditorFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
	~CMaterialEditorFrame();

	void StartEngineLoop();
	void StopEngineLoop();

	void InitializeConsoleVariables();

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

	wxBitmap 
		largeOpen,
		largeScriptEdit;
	wxBitmap iconMediaPause;
	wxBitmap iconMediaPlay;
	wxBitmap
		iconDocumentRefresh,
		iconDocumentNew,
		iconDocumentSave,
		iconDocumentUndo,
		iconDocumentRedo,
		iconShapeCube;
	wxBitmap iconShapeSphere;
	wxBitmap iconShapePlane;

	wxTimer* timer;

	wxAuiManager *manager;

	CMaterialEditorRenderCanvas *editorViewport;
	CMaterialEditorMaterialGlobalProperties *editorMaterialProperties;
	CMaterialEditorConsolePanel *editorConsolePanel;

	wxDECLARE_EVENT_TABLE();
};

#endif // !__MATERIALEDITORFRAME__
