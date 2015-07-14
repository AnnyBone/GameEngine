#include "MaterialEditorBase.h"

#include "MaterialEditorMaterialProperties.h"

enum
{
	MATERIAL_EVENT_GLOBAL
};

CMaterialEditorMaterialGlobalProperties::CMaterialEditorMaterialGlobalProperties(wxWindow *parent)
: wxPropertyGrid(parent)
{
	currentMaterial = NULL;

	name = new wxStringProperty("Name","name");
	path = new wxStringProperty("Path", "path");
	skins = new wxIntProperty("Skins","skins");
	animationSpeed = new wxFloatProperty("Animation Speed", "animspeed");
	alpha = new wxFloatProperty("Alpha", "alpha");
	preserve = new wxBoolProperty("Preserve", "preserve");
	useAlpha = new wxBoolProperty("Alpha-Tested", "alphatest");
	blend = new wxBoolProperty("Blend", "blend");
	animated = new wxBoolProperty("Animated", "animated");
	mirror = new wxBoolProperty("Mirror", "mirror");
	nearest = new wxBoolProperty("Nearest", "nearest");
	water = new wxBoolProperty("Water", "water");
	notris = new wxBoolProperty("Override Wireframe", "wireframe");

	materialCategory = Append(new wxPropertyCategory("Material"));
	AppendIn(materialCategory, name);
	AppendIn(materialCategory, path);
	AppendIn(materialCategory, animationSpeed);
	AppendIn(materialCategory, alpha);
	AppendIn(materialCategory, preserve);
	AppendIn(materialCategory, useAlpha);
	AppendIn(materialCategory, blend);
	AppendIn(materialCategory, animated);
	AppendIn(materialCategory, mirror);
	AppendIn(materialCategory, nearest);
	AppendIn(materialCategory, water);
	AppendIn(materialCategory, notris);

	//skinCategory = AppendIn(materialCategory, skins);

	// TODO: skin category class...

	SetSize(wxSize(300, wxDefaultSize.y));

	CenterSplitter(true);
}

CMaterialEditorMaterialGlobalProperties::~CMaterialEditorMaterialGlobalProperties()
{
}

void CMaterialEditorMaterialGlobalProperties::OnChanged(wxPropertyGridEvent &event)
{
	// Ignore if we don't have anything loaded.
	if (!currentMaterial)
		return;

	wxPGProperty *property = event.GetProperty();
	if (!strcmp(event.GetPropertyName(), "animspeed"))
		currentMaterial->fAnimationSpeed = event.GetPropertyValue().GetDouble();
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
		AppendIn(skinCategory, new wxIntProperty("test"));
	}

	CenterSplitter(true);
}

void CMaterialEditorMaterialGlobalProperties::SetCurrentMaterial(Material_t *newMaterial)
{
	if (currentMaterial)
		engine->UnloadMaterial(currentMaterial);
	currentMaterial = newMaterial;
}

Material_t *CMaterialEditorMaterialGlobalProperties::GetCurrent()
{
	return currentMaterial;
}