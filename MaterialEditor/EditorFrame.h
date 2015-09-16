#ifndef __EDITORFRAME__
#define	__EDITORFRAME__

#include "EditorRenderCanvas.h"
#include "EditorViewportPanel.h"
#include "EditorConsolePanel.h"

// Tools
#include "WADFrame.h"
#include "MaterialFrame.h"

// Dialog
#include "preferences\PreferencesDialog.h"

class CMainViewportPanel : public CEditorViewportPanel
{
public:
	CMainViewportPanel(wxWindow *wParent) : CEditorViewportPanel(wParent) {}

	virtual void Draw()
	{
		engine->Video_PreFrame();

		engine->DrawGradientBackground();
		engine->DrawResetCanvas();
		engine->DrawFPS();

		engine->Video_PostFrame();
	}
protected:
private:
};

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
	void OnPreferences(wxCommandEvent &event);
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
	CEditorConsolePanel *pConsole;

	CWADFrame *WADTool;
	CMaterialFrame *MaterialTool;

	CPreferencesDialog *Preferences;

	CMainViewportPanel *pViewport;

	wxDECLARE_EVENT_TABLE();
};

#endif // !__EDITORFRAME__
