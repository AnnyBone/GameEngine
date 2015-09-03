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

#include "EditorMaterialProperties.h"

#include "MaterialFrame.h"

#define	WAD_TITLE "Material Tool"

CMaterialViewportPanel::CMaterialViewportPanel(wxWindow *wParent) 
	: CEditorViewportPanel(wParent)
{
	CubeModel = engine->LoadModel("models/editor/cube.md2");
	SphereModel = engine->LoadModel("models/editor/sphere.md2");
	PlaneModel = engine->LoadModel("models/editor/plane.md2");
	if (!CubeModel || !SphereModel || !PlaneModel)
		pLog_Write(EDITOR_LOG, "Failed to load preview mesh!\n");

	PreviewMaterial = NULL;

	PreviewEntity = engine->CreateClientEntity();
	if (!PreviewEntity)
		pLog_Write(EDITOR_LOG, "Failed to create client entity!\n");
	// TODO: error handling...

	PreviewEntity->alpha = 255;
	PreviewEntity->origin[0] = 50.0f;
	PreviewEntity->origin[1] = 0;
	PreviewEntity->origin[2] = 0;
	PreviewEntity->model = CubeModel;

	PreviewLight = engine->CreateDynamicLight(0);
	if (!PreviewLight)
	{ }

	PreviewLight->decay = 0;
	PreviewLight->die = 0;
	PreviewLight->minlight = 32.0f;
	PreviewLight->radius = 200.0f;
	Math_VectorSet(-40.0f, PreviewLight->origin);
	Math_VectorSet(255.0f, PreviewLight->color);
}

void CMaterialViewportPanel::Draw()
{
	engine->Video_PreFrame();

	//engine->DrawSetCanvas(CANVAS_DEFAULT);
	engine->DrawGradientBackground();

	if (PreviewEntity)
		engine->DrawEntity(PreviewEntity);

	engine->DrawResetCanvas();
	engine->DrawFPS();

	engine->Video_PostFrame();
}

/*	Attempts to set the given material as the active one.
	Returns true on success, false on fail.
*/
bool CMaterialViewportPanel::SetMaterial(Material_t *NewMaterial)
{
	// Don't bother if it hasn't changed.
	if (NewMaterial == PreviewMaterial)
		return true;

	// Ensure the new material is valid.
	if (!NewMaterial)
	{
		pLog_Write(EDITOR_LOG, "Invalid material!\n");
		return false;
	}

	// Update the preview entity to use the new material.
	PreviewMaterial = NewMaterial;
	PreviewEntity->model->mAssignedMaterials = PreviewMaterial;

	return true;
}

void CMaterialViewportPanel::SetModel(MaterialViewportModel PreviewModel)
{
	switch (PreviewModel)
	{
	case MATERIAL_PREVIEW_CUBE:
		PreviewEntity->model = CubeModel;
		break;
	case MATERIAL_PREVIEW_SPHERE:
		PreviewEntity->model = SphereModel;
		break;
	case MATERIAL_PREVIEW_PLANE:
		PreviewEntity->model = PlaneModel;
		break;
	default:
		// handle bad cases
		break;
	}
}

Material_t *CMaterialViewportPanel::GetMaterial()
{
	return PreviewMaterial;
}

enum MaterialFrameEvent_s
{
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

	wxMenuBar *mbMenu = new wxMenuBar;
	mbMenu->Append(mFile, "&File");
	SetMenuBar(mbMenu);

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

	Show();

	iPaneInfo.PinButton(true);
	iPaneInfo.BestSize(wxSize(256, 256));

	// Create the engine viewport...
	Viewport = new CMaterialViewportPanel(this);
	iPaneInfo.Caption("Viewport");
	iPaneInfo.Top();
	iPaneInfo.Right();
	mManager->AddPane(Viewport, iPaneInfo);

	// Create the material props...
	CEditorMaterialGlobalProperties *mgpProperties = new CEditorMaterialGlobalProperties(this);
	iPaneInfo.Caption("Properties");
	iPaneInfo.Bottom();
	iPaneInfo.Right();
	mManager->AddPane(mgpProperties, iPaneInfo);

	//

	CreateStatusBar(3);
	SetStatusText("Initialized");

	Layout();
	Centre();

	mManager->Update();
}

CMaterialFrame::~CMaterialFrame()
{
	// Uninitialize the AUI manager.
	mManager->UnInit();
}

void CMaterialFrame::ReloadCurrentFile()
{
	Material_t *CurrentMaterial = Viewport->GetMaterial();
	if (!CurrentMaterial)
		// Likely nothing loaded, just return.
		return;

	// Ensure things have actually changed.
	tCurrentModified = pFileSystem_GetModifiedTime(sCurrentFilePath);
	if (tCurrentModified == tLastModified)
		return;

	// Reload it.
	engine->UnloadMaterial(CurrentMaterial);
	CurrentMaterial = engine->LoadMaterial(CurrentMaterial->cPath);
	if (CurrentMaterial)
	{
		tLastModified = tCurrentModified;
		Viewport->SetMaterial(CurrentMaterial);
	}
}

void CMaterialFrame::LoadMaterial(wxString sFileName)
{
	wxString sMaterialName = sFileName;
	if (sMaterialName.EndsWith(".material"))
	{
		// Remove the extension.
		sMaterialName.RemoveLast(9);

		Material_t *CurrentMaterial = Viewport->GetMaterial();
		if (CurrentMaterial)
			engine->UnloadMaterial(CurrentMaterial);

		CurrentMaterial = engine->LoadMaterial(sMaterialName);
		if (CurrentMaterial)
			Viewport->SetMaterial(CurrentMaterial);

		SetTitle(sMaterialName + wxString(" - ") + wxString(WAD_TITLE));
	}
}

// Events

void CMaterialFrame::FileEvent(wxCommandEvent &event)
{
	switch (event.GetId())
	{
	case wxID_OPEN:
	{
		wxFileDialog *fdOpenMaterial = new wxFileDialog(
			this,
			"Open Material",
			engine->GetBasePath(),
			"",
			"Supported files (*.material)|*.material",
			wxFD_OPEN|wxFD_FILE_MUST_EXIST);
		if (fdOpenMaterial->ShowModal() == wxID_OK)
		{
			LoadMaterial(fdOpenMaterial->GetFilename());

			cMaterialScript->LoadFile(fdOpenMaterial->GetPath());
		}
	}
	break;
	}
}