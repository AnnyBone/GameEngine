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

	SetCellBackgroundColour(wxColour(0, 0, 0));
	SetCellTextColour(wxColour(0, 255, 0));
	SetEmptySpaceColour(wxColour(0, 0, 0));
	SetCaptionBackgroundColour(wxColour(70, 70, 70));
	SetCaptionTextColour(wxColour(255, 255, 255));
	SetMarginColour(wxColour(50, 50, 50));
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
}

void CMaterialEditorMaterialGlobalProperties::SetCurrentMaterial(Material_t *newMaterial)
{

}