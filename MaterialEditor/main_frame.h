/*	DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
	Version 2, December 2004

	Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

	Everyone is permitted to copy and distribute verbatim or modified
	copies of this license document, and changing it is allowed as long
	as the name is changed.

	DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
	TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

	0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#ifndef EDITOR_FRAME_H
#define	EDITOR_FRAME_H

#include "base_rendercanvas.h"
#include "base_viewportpanel.h"
#include "EditorConsolePanel.h"

// Tools
#include "wad_frame.h"
#include "material_frame.h"
#include "model_frame.h"

// Dialog
#include "preferences/PreferencesDialog.h"

class MainViewportPanel : public BaseViewportPanel
{
public:
	MainViewportPanel(wxWindow *parent) : BaseViewportPanel(parent) {}

	virtual void Draw();

	virtual void OnKey(wxKeyEvent &event);
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
	void Initialize();
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
	void OnMouseEvent(wxMouseEvent &event);

	void ReloadCurrentDocument();

	void OpenWADTool(wxString sPath);
	void OpenMaterial(wxString path);

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

	wxTimer			*timer;
	wxAuiManager	*manager;

	time_t currentTimeModified;
	time_t lastTimeModified;

	wxString currentFilePath;

	BaseDrawCanvas *editorViewport;

	CWADFrame		*tool_wad;
	CMaterialFrame	*tool_material;
	ModelFrame		*frame_model;

	CPreferencesDialog *preferences;

	MainViewportPanel	*viewport;
	CEditorConsolePanel *pConsole;

	wxDECLARE_EVENT_TABLE();
};

#endif // !EDITOR_FRAME_H
