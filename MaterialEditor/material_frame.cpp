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

#include "EditorBase.h"

#include "base_rendercanvas.h"
#include "base_viewportpanel.h"

#include "material_frame.h"

#define	WAD_TITLE "Material Editor"

/*
	Global Properties Grid
*/

class MaterialGlobalProperties : public wxPropertyGrid
{
public:
	MaterialGlobalProperties(wxWindow *parent) : wxPropertyGrid(parent) 
	{
		material = NULL;
		name = new wxStringProperty("Name", "name");
		path = new wxStringProperty("Path", "path");
		skins = new wxIntProperty("Skins", "skins");
		animation_speed = new wxFloatProperty("Animation Speed", "animspeed");
		alpha = new wxFloatProperty("Alpha", "alpha");
		preserve = new wxBoolProperty("Preserve", "preserve");
		use_alpha = new wxBoolProperty("Alpha-Tested", "alphatest");
		blend = new wxBoolProperty("Blend", "blend");
		animated = new wxBoolProperty("Animated", "animated");
		mirror = new wxBoolProperty("Mirror", "mirror");
		water = new wxBoolProperty("Water", "water");
		notris = new wxBoolProperty("Override Wireframe", "wireframe");

		SetSize(wxSize(300, wxDefaultSize.y));
		CenterSplitter(true);
	}
	~MaterialGlobalProperties() {}

	void Initialize();
	void GetProperties(Material_t *newmaterial);

protected:
private:
	wxStringProperty	*name,
						*path;
	wxIntProperty		*skins;
	wxBoolProperty		*preserve,
						*use_alpha,
						*blend,
						*animated,
						*mirror,
						*water,
						*notris;
	wxFloatProperty		*animation_speed,
						*alpha;

	Material_t *material;
};

void MaterialGlobalProperties::Initialize()
{
}

void MaterialGlobalProperties::GetProperties(Material_t *newmaterial)
{
	if (!newmaterial)
		return;

	if (material != newmaterial)
		material = newmaterial;

	name->SetValue(material->cName);
	path->SetValue(material->cPath);
	skins->SetValue(material->iSkins);
	animation_speed->SetValue(material->fAnimationSpeed);

	if (material->iFlags & MATERIAL_FLAG_PRESERVE)
		preserve->SetValue(true);
	else
		preserve->SetValue(false);

	if (material->iFlags & MATERIAL_FLAG_ANIMATED)
		animated->SetValue(true);
	else
		animated->SetValue(false);

	notris->SetValue(material->bWireframeOverride);
}

/*
	Viewport
*/

MaterialViewportPanel::MaterialViewportPanel(wxWindow *wParent)
	: BaseViewportPanel(wParent)
{
	CubeModel = engine->LoadModel("models/editor/cube.md2");
	SphereModel = engine->LoadModel("models/editor/sphere.md2");
	PlaneModel = engine->LoadModel("models/editor/plane.md2");
	if (!CubeModel || !SphereModel || !PlaneModel)
		pLog_Write(EDITOR_LOG, "Failed to load preview mesh!\n");

	preview_material = NULL;

	preview_entity = engine->CreateClientEntity();
	if (preview_entity)
	{
		preview_entity->alpha = 255;
		preview_entity->origin[0] = 50.0f;
		preview_entity->origin[1] = 0;
		preview_entity->origin[2] = 0;
		preview_entity->model = CubeModel;
	}
	else
		pLog_Write(EDITOR_LOG, "Failed to create client entity!\n");

	preview_light = engine->CreateDynamicLight(0);
	if (preview_light)
	{
		preview_light->decay = 0;
		preview_light->die = 0;
		preview_light->minlight = 32.0f;
		preview_light->radius = 200.0f;
		Math_VectorSet(-40.0f, preview_light->origin);
		Math_VectorSet(255.0f, preview_light->color);
	}
	else
		pLog_Write(EDITOR_LOG, "Failed to create dynamic light!\n");
}

void MaterialViewportPanel::Initialize()
{
	CreateDrawCanvas(this);

	wxBoxSizer *vertical = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *horizontal = new wxBoxSizer(wxHORIZONTAL);

	wxBitmapButton *button_cube = new wxBitmapButton(
		this,
		wxID_ANY,
		iconShapeCube,
		wxDefaultPosition,
		wxSize(24, 24));
	horizontal->Add(button_cube);

	wxBitmapButton *button_sphere = new wxBitmapButton(
		this,
		wxID_ANY,
		iconShapeSphere,
		wxDefaultPosition,
		wxSize(24, 24));
	horizontal->Add(button_sphere);

	wxBitmapButton *button_plane = new wxBitmapButton(
		this,
		wxID_ANY,
		bSmallPlaneIcon,
		wxDefaultPosition,
		wxSize(24, 24));
	horizontal->Add(button_plane);

	vertical->Add(horizontal, 0, wxTOP | wxLEFT | wxRIGHT);
	vertical->Add(GetDrawCanvas(), 1, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT);

	SetSizer(vertical);
}

void MaterialViewportPanel::Draw()
{
	engine->DrawGradientBackground();

	if (preview_entity && preview_entity->model)
	{
		engine->DrawEntity(preview_entity);
		preview_entity->angles[1] += 0.5f;
		engine->DrawResetCanvas();
	}
	else // Plane
	{
		engine->DrawSetCanvas(CANVAS_DEFAULT);
		engine->DrawMaterialSurface(preview_material, 0, 0, 0, GetDrawCanvasWidth(), GetDrawCanvasHeight(), 1.0f);
	}
}

/*	Attempts to set the given material as the active one.
	Returns true on success, false on fail.
*/
bool MaterialViewportPanel::SetMaterial(Material_t *NewMaterial)
{
	// Don't bother if it hasn't changed.
	if (NewMaterial == preview_material)
		return true;

	// Ensure the new material is valid.
	if (!NewMaterial)
	{
		pLog_Write(EDITOR_LOG, "Invalid material!\n");
		return false;
	}

	// Update the preview entity to use the new material.
	preview_material = NewMaterial;
	preview_entity->model->mAssignedMaterials = preview_material;

	return true;
}

void MaterialViewportPanel::SetModel(MaterialViewportModel PreviewModel)
{
	switch (PreviewModel)
	{
	case MATERIAL_PREVIEW_CUBE:
		preview_entity->model = CubeModel;
		break;
	case MATERIAL_PREVIEW_SPHERE:
		preview_entity->model = SphereModel;
		break;
	case MATERIAL_PREVIEW_PLANE:
		preview_entity->model = 0;
		break;
	default:
		// handle bad cases
		break;
	}
}

Material_t *MaterialViewportPanel::GetMaterial()
{
	return preview_material;
}

/*
	Frame
*/

enum MaterialFrameEvent
{
	MATERIAL_EVENT_CUBE,
	MATERIAL_EVENT_SPHERE,
	MATERIAL_EVENT_PLANE
};

wxBEGIN_EVENT_TABLE(CMaterialFrame, wxFrame)
EVT_MENU(wxID_OPEN, CMaterialFrame::FileEvent)
EVT_MENU(wxID_SAVE, CMaterialFrame::FileEvent)
EVT_MENU(wxID_EXIT, CMaterialFrame::FileEvent)
EVT_MENU(wxID_ABOUT, CMaterialFrame::FileEvent)
wxEND_EVENT_TABLE()

CMaterialFrame::CMaterialFrame(wxWindow* parent, wxWindowID id)
	: wxFrame(parent, id, WAD_TITLE, wxDefaultPosition, wxSize(1024, 640))
{
	SetIcon(wxIcon("resource/icon-material.png", wxBITMAP_TYPE_PNG));

	mManager = new wxAuiManager(this);

	// Menu
	{
		wxMenu *mFile = new wxMenu;
		wxMenuItem *iNewDocument = new wxMenuItem(mFile, wxID_NEW);
		iNewDocument->SetBitmap(smallDocumentNew);
		wxMenuItem *iOpenDocument = new wxMenuItem(mFile, wxID_OPEN);
		iOpenDocument->SetBitmap(smallDocumentOpen);
		wxMenuItem *iCloseDocument = new wxMenuItem(mFile, wxID_CLOSE);
		iCloseDocument->SetBitmap(smallDocumentClose);
		wxMenuItem *iSaveDocument = new wxMenuItem(mFile, wxID_SAVE);
		iSaveDocument->SetBitmap(smallDocumentSave);
		wxMenuItem *iSaveDocumentAs = new wxMenuItem(mFile, wxID_SAVEAS);
		iSaveDocumentAs->SetBitmap(smallDocumentSaveAs);
		wxMenuItem *iExit = new wxMenuItem(mFile, wxID_EXIT);
		iExit->SetBitmap(smallApplicationExit);
		mFile->Append(iNewDocument);
		mFile->Append(iOpenDocument);
		mFile->AppendSeparator();
		mFile->Append(iCloseDocument);
		mFile->AppendSeparator();
		mFile->Append(iSaveDocument);
		mFile->Append(iSaveDocumentAs);
		mFile->AppendSeparator();
		mFile->Append(iExit);

		wxMenu *menu_view = new wxMenu;
		menu_view->Append(wxID_ANY, "Cube");
		menu_view->Append(wxID_ANY, "Sphere");
		menu_view->Append(wxID_ANY, "Plane");
		menu_view->AppendSeparator();
		menu_view->Append(wxID_ANY, "Reload material");

		wxMenuBar *menubar = new wxMenuBar;
		menubar->Append(mFile, "&File");
		menubar->Append(menu_view, "&View");
		SetMenuBar(menubar);
	}

	//

	wxAuiPaneInfo iPaneInfo;
	iPaneInfo.CloseButton(false);
	iPaneInfo.PinButton(false);

	cMaterialScript = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxEmptyString);
	cMaterialScript->SetUseTabs(true);
	cMaterialScript->SetTabWidth(4);
	cMaterialScript->SetIndent(4);
	cMaterialScript->SetTabIndents(true);
	cMaterialScript->SetBackSpaceUnIndents(true);
	cMaterialScript->SetViewEOL(false);
	cMaterialScript->SetViewWhiteSpace(false);
	cMaterialScript->SetMarginWidth(2, 0);
	cMaterialScript->SetIndentationGuides(true);
	cMaterialScript->SetMarginType(1, wxSTC_MARGIN_SYMBOL);
	cMaterialScript->SetMarginMask(1, wxSTC_MASK_FOLDERS);
	cMaterialScript->SetMarginWidth(1, 16);
	cMaterialScript->SetMarginSensitive(1, true);
	cMaterialScript->SetProperty(wxT("fold"), wxT("1"));
	cMaterialScript->SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);
	cMaterialScript->SetMarginType(0, wxSTC_MARGIN_NUMBER);
	cMaterialScript->SetMarginWidth(0, cMaterialScript->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("_99999")));
	cMaterialScript->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS);
	cMaterialScript->MarkerSetBackground(wxSTC_MARKNUM_FOLDER, wxColour(wxT("BLACK")));
	cMaterialScript->MarkerSetForeground(wxSTC_MARKNUM_FOLDER, wxColour(wxT("WHITE")));
	cMaterialScript->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS);
	cMaterialScript->MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPEN, wxColour(wxT("BLACK")));
	cMaterialScript->MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPEN, wxColour(wxT("WHITE")));
	cMaterialScript->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY);
	cMaterialScript->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUS);
	cMaterialScript->MarkerSetBackground(wxSTC_MARKNUM_FOLDEREND, wxColour(wxT("BLACK")));
	cMaterialScript->MarkerSetForeground(wxSTC_MARKNUM_FOLDEREND, wxColour(wxT("WHITE")));
	cMaterialScript->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUS);
	cMaterialScript->MarkerSetBackground(wxSTC_MARKNUM_FOLDEROPENMID, wxColour(wxT("BLACK")));
	cMaterialScript->MarkerSetForeground(wxSTC_MARKNUM_FOLDEROPENMID, wxColour(wxT("WHITE")));
	cMaterialScript->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY);
	cMaterialScript->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY);
	cMaterialScript->SetSelBackground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
	cMaterialScript->SetSelForeground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));

	iPaneInfo.Caption("Script");
	iPaneInfo.Center();
	mManager->AddPane(cMaterialScript, iPaneInfo);

	//

	iPaneInfo.PinButton(true);
	iPaneInfo.BestSize(wxSize(256, 256));

	Centre();
	Show();

	// Create the engine viewport...
	Viewport = new MaterialViewportPanel(this);
	iPaneInfo.Caption("Viewport");
	iPaneInfo.Top();
	iPaneInfo.Right();
	iPaneInfo.MaximizeButton(true);
	mManager->AddPane(Viewport, iPaneInfo);
	Viewport->Initialize();
	Viewport->StartDrawing();

	// Create the material props...
	MaterialGlobalProperties *globalprops = new MaterialGlobalProperties(this);
	iPaneInfo.Caption("Properties");
	iPaneInfo.Bottom();
	iPaneInfo.Right();
	iPaneInfo.MaximizeButton(false);
	mManager->AddPane(globalprops, iPaneInfo);

	//

	CreateStatusBar();
	SetStatusText("Initialized");

	Layout();

	mManager->Update();
}

CMaterialFrame::~CMaterialFrame()
{
	// Uninitialize the AUI manager.
	mManager->UnInit();
}

void CMaterialFrame::ReloadMaterial()
{
	Material_t *CurrentMaterial = Viewport->GetMaterial();
	if (!CurrentMaterial)
		// Likely nothing loaded, just return.
		return;

	// Ensure things have actually changed.
	tCurrentModified = pFileSystem_GetModifiedTime(sCurrentFilePath);
	if (tCurrentModified == tLastModified)
		return;

	Viewport->SetMaterial(NULL);

	// Unload it.
	wxString sOldPath = CurrentMaterial->cPath;
	engine->UnloadMaterial(CurrentMaterial);

	// Reload it.
	CurrentMaterial = engine->LoadMaterial(sOldPath);
	if (CurrentMaterial)
	{
		tLastModified = tCurrentModified;
		Viewport->SetMaterial(CurrentMaterial);
	}

	// Reload the file into the script editor.
	cMaterialScript->LoadFile(sCurrentFilePath);
}

void CMaterialFrame::LoadMaterial(wxString sFileName)
{
	wxString materialname = sFileName;
	if (materialname.EndsWith(".material"))
		// Remove the extension.
		materialname.RemoveLast(9);

	Material_t *mCurrent = Viewport->GetMaterial();
	if (mCurrent)
		engine->UnloadMaterial(mCurrent);

	mCurrent = engine->LoadMaterial(materialname);
	if (mCurrent)
		Viewport->SetMaterial(mCurrent);

	SetTitle(materialname + wxString(" - ") + wxString(WAD_TITLE));
}

void CMaterialFrame::UnloadMaterial()
{
	Material_t *mCurrent = Viewport->GetMaterial();
	if (!mCurrent)
		// Likely nothing loaded, just return.
		return;

	Viewport->SetMaterial(NULL);

	sOldFilePath = sCurrentFilePath;
	sCurrentFilePath.Clear();

	cMaterialScript->ClearAll();

	engine->UnloadMaterial(mCurrent);
}

bool CMaterialFrame::Destroy()
{
	UnloadMaterial();

	return wxFrame::Destroy();
}

// Events

void CMaterialFrame::FileEvent(wxCommandEvent &event)
{
	char defaultpath[PLATFORM_MAX_PATH];

	sprintf(defaultpath, "%s/%s", engine->GetBasePath(), engine->GetMaterialPath());

	switch (event.GetId())
	{
	case wxID_OPEN:
		{
			wxFileDialog *fdOpenMaterial = new wxFileDialog(
				this,
				"Open Material",
				defaultpath,
				"",
				"Supported files (*.material)|*.material",
				wxFD_OPEN | wxFD_FILE_MUST_EXIST);
			if (fdOpenMaterial->ShowModal() == wxID_OK)
			{
				LoadMaterial(fdOpenMaterial->GetFilename());

				// Ensure the current path is kept up to date!
				sCurrentFilePath = fdOpenMaterial->GetPath();

				// Check when it was last modified.
				tLastModified = pFileSystem_GetModifiedTime(sCurrentFilePath);

				// Load the file into the script editor.
				cMaterialScript->LoadFile(sCurrentFilePath);
			}
		}
		break;
	case wxID_CLOSE:
		UnloadMaterial();

		DestroyChildren();

		Close(false);
		break;
	case wxID_SAVE:
		if (sCurrentFilePath.IsEmpty())
			return;

		cMaterialScript->SaveFile(sCurrentFilePath);

		// Reload it so we instantly see the changes.
		ReloadMaterial();
		break;
	}
}