#pragma once
class CMaterialEditorMaterialGlobalProperties : public wxPropertyGrid
{
public:
	CMaterialEditorMaterialGlobalProperties(wxWindow *parent);
	~CMaterialEditorMaterialGlobalProperties();

	void Update();
	void SetCurrentMaterial(Material_t *newMaterial);

	Material_t *GetCurrent();

private:
	void OnChanged(wxPropertyGridEvent &event);

	wxPGProperty 
		*materialCategory,
		*skinCategory;

	wxStringProperty* name;
	wxStringProperty* path;

	wxBoolProperty* preserve;
	wxBoolProperty* useAlpha;
	wxBoolProperty* blend;
	wxBoolProperty* animated;
	wxBoolProperty* mirror;
	wxBoolProperty* nearest;
	wxBoolProperty* water;
	wxBoolProperty* notris;

	wxIntProperty* skins;

	wxFloatProperty* animationSpeed;
	wxFloatProperty* alpha;

	Material_t *currentMaterial;

};

