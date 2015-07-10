#pragma once
class CMaterialEditorMaterialGlobalProperties : public wxPropertyGrid
{
public:
	CMaterialEditorMaterialGlobalProperties(wxWindow *parent);

	void Update();
	void SetCurrentMaterial(Material_t *newMaterial);

	Material_t *GetCurrent();

private:
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

