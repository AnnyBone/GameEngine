/*	Copyright (C) 2011-2015 OldTimes Software

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

#pragma once

typedef enum
{
	MATERIAL_PREVIEW_CUBE,
	MATERIAL_PREVIEW_SPHERE,
	MATERIAL_PREVIEW_PLANE
} MaterialViewportModel;

class CMaterialViewportPanel : public CEditorViewportPanel
{
public:
	CMaterialViewportPanel(wxWindow *wParent);

	virtual void Draw();

	bool SetMaterial(Material_t *NewMaterial);
	void SetModel(MaterialViewportModel PreviewModel);

	Material_t *GetMaterial();
protected:
private:
	ClientEntity_t *PreviewEntity;
	Material_t *PreviewMaterial;
	model_t *CubeModel, *PlaneModel, *SphereModel;
};

class CMaterialFrame : public wxFrame
{
public:

	CMaterialFrame(wxWindow* parent, wxWindowID id = wxID_ANY);
	~CMaterialFrame();

	void LoadMaterial(wxString sFileName);

	void ReloadCurrentFile();

private:

	time_t tCurrentModified, tLastModified;

	wxAuiManager *mManager;
	wxString sCurrentFilePath;
	wxStyledTextCtrl *cMaterialScript;

	CMaterialViewportPanel *Viewport;

	// Events
	void FileEvent(wxCommandEvent &event);

	wxDECLARE_EVENT_TABLE();
};

