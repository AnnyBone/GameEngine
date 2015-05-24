#include "MaterialEditorBase.h"

#include "MaterialEditorMaterialProperties.h"

CMaterialEditorMaterialGlobalProperties::CMaterialEditorMaterialGlobalProperties(wxPropertyGrid *propertyParent)
{
	name = new wxStringProperty("Name","name");
	path = new wxStringProperty("Path", "path");
	skins = new wxIntProperty("Skins","skins");
	animationSpeed = new wxFloatProperty("Animation Speed");
	alpha = new wxFloatProperty("Alpha");
	preserve = new wxBoolProperty("Preserve");
	useAlpha = new wxBoolProperty("Alpha-Tested");
	blend = new wxBoolProperty("Blend", "blend");
	animated = new wxBoolProperty("Animated");
	mirror = new wxBoolProperty("Mirror");
	nearest = new wxBoolProperty("Nearest");
	water = new wxBoolProperty("Water");
	notris = new wxBoolProperty("Override Wireframe");

	propertyParent->Append(name);
	propertyParent->Append(path);
	propertyParent->Append(skins);
	propertyParent->Append(animationSpeed);
	propertyParent->Append(alpha);
	propertyParent->Append(preserve);
	propertyParent->Append(useAlpha);
	propertyParent->Append(blend);
	propertyParent->Append(animated);
	propertyParent->Append(mirror);
	propertyParent->Append(nearest);
	propertyParent->Append(water);
	propertyParent->Append(notris);
}

CMaterialEditorMaterialGlobalProperties::~CMaterialEditorMaterialGlobalProperties()
{
}

void CMaterialEditorMaterialGlobalProperties::Update(Material_t *currentMaterial)
{
	name->SetValue(currentMaterial->cName);
	path->SetValue(currentMaterial->cPath);
	skins->SetValue(currentMaterial->iSkins);
	animationSpeed->SetValue(currentMaterial->fAnimationSpeed);
}