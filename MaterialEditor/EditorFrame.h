#ifndef __EDITORFRAME__
#define	__EDITORFRAME__

#include "EditorRenderCanvas.h"
#include "EditorMaterialProperties.h"
#include "EditorConsolePanel.h"

class CEditorFrame : public wxFrame
{
public:
	CEditorFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
	~CEditorFrame();

	void StartEngineLoop();
	void StopEngineLoop();

	void InitializeConsoleVariables();

	void PrintMessage(char *text);
	void PrintWarning(char *text);
	void PrintError(char *text);

private:
	// Events
	void OnOpen(wxCommandEvent &event);
	void OnTool(wxCommandEvent &event);
	void OnView(wxCommandEvent &event);
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

	void ReloadCurrentDocument();

	void OpenWADTool(wxString sPath);
	void OpenMaterialTool(wxString sPath);

	double 
		dAutoReloadDelay,
		dClientTime;

	wxMenuItem

		*viewWireframe,
		*viewTextured,
		*viewFlat,
		*viewLit,

		*windowShowConsole,
		*windowShowProperties;

	wxTimer* timer;

	wxAuiManager *manager;

	time_t currentTimeModified;
	time_t lastTimeModified;

	wxString currentFilePath;

	CEditorRenderCanvas *editorViewport;
	CEditorMaterialGlobalProperties *editorMaterialProperties;
	CEditorConsolePanel *editorConsolePanel;

	wxDECLARE_EVENT_TABLE();
};

#endif // !__EDITORFRAME__
