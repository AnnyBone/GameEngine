#pragma once
class CMaterialEditorMaterialGlobalProperties
{
public:
	CMaterialEditorMaterialGlobalProperties(wxPropertyGrid *propertyParent);
	~CMaterialEditorMaterialGlobalProperties();

	void Update(Material_t *currentMaterial);

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
};

