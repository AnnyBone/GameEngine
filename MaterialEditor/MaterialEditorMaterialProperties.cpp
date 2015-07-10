#include "MaterialEditorBase.h"

#include "MaterialEditorMaterialProperties.h"

CMaterialEditorMaterialGlobalProperties::CMaterialEditorMaterialGlobalProperties(wxWindow *parent)
	: wxPropertyGrid(parent)
{
	currentMaterial = NULL;

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

	Append(new wxPropertyCategory("Global"));
	Append(name);
	Append(path);
	Append(skins);
	Append(animationSpeed);
	Append(alpha);
	Append(preserve);
	Append(useAlpha);
	Append(blend);
	Append(animated);
	Append(mirror);
	Append(nearest);
	Append(water);
	Append(notris);

	SetSize(wxSize(300, wxDefaultSize.y));

	CenterSplitter(true);
}

void CMaterialEditorMaterialGlobalProperties::Update()
{
	if (!currentMaterial)
		return;

	name->SetValue(currentMaterial->cName);
	path->SetValue(currentMaterial->cPath);
	skins->SetValue(currentMaterial->iSkins);
	animationSpeed->SetValue(currentMaterial->fAnimationSpeed);

	if (currentMaterial->iFlags & MATERIAL_FLAG_PRESERVE)
		preserve->SetValue(true);
	else
		preserve->SetValue(false);

	if (currentMaterial->iFlags & MATERIAL_FLAG_ANIMATED)
		animated->SetValue(true);
	else
		animated->SetValue(false);

	if (currentMaterial->iFlags & MATERIAL_FLAG_NOTRIS)
		notris->SetValue(true);
	else
		notris->SetValue(false);

	int i; 
	for (i = 0; i < currentMaterial->iSkins; i++)
	{

	}
}

void CMaterialEditorMaterialGlobalProperties::SetCurrentMaterial(Material_t *newMaterial)
{
	currentMaterial = newMaterial;
}

Material_t *CMaterialEditorMaterialGlobalProperties::GetCurrent()
{
	return currentMaterial;
}