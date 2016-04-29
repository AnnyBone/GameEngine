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

#pragma once

typedef enum
{
	MATERIAL_PREVIEW_CUBE,
	MATERIAL_PREVIEW_SPHERE,
	MATERIAL_PREVIEW_PLANE
} MaterialViewportModel;

class MaterialViewportPanel : public BaseViewportPanel
{
public:
	MaterialViewportPanel(wxWindow *wParent);

	virtual void Initialize();
	virtual void Draw();

	bool SetPreviewMaterial(Material_t *NewMaterial);
	void SetModel(MaterialViewportModel PreviewModel);

	Material_t *GetMaterial();
protected:
private:
	DynamicLight_t	*preview_light;
	ClientEntity_t	*preview_entity;
	Material_t		*preview_material;
	model_t			*CubeModel, *SphereModel;

	void ViewEvent(wxCommandEvent &event);

	wxDECLARE_EVENT_TABLE();
};

class CMaterialFrame : public wxFrame
{
public:

	CMaterialFrame(wxWindow* parent, wxWindowID id = wxID_ANY);
	~CMaterialFrame();

	void LoadMaterial(wxString path);
	void UnloadMaterial();
	void ReloadMaterial();

	virtual bool Destroy();

private:

	time_t tCurrentModified, tLastModified;

	wxAuiManager *mManager;
	wxString sCurrentFilePath, sOldFilePath;
	wxStyledTextCtrl *cMaterialScript;

	MaterialViewportPanel *Viewport;

	// Events
	void FileEvent(wxCommandEvent &event);
	void ViewEvent(wxCommandEvent &event);

	wxDECLARE_EVENT_TABLE();
};

