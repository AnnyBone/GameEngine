/*	Copyright (C) 2011-2016 OldTimes Software

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
