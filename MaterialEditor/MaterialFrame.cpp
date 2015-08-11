#include "EditorBase.h"

#include "MaterialFrame.h"

#include "EditorRenderCanvas.h"
#include "EditorMaterialProperties.h"

#define	WAD_TITLE "Material Tool"

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
	: wxFrame(parent, id, WAD_TITLE, wxDefaultPosition, wxSize(720, 640))
{
    Show();

	mManager = new wxAuiManager(this);

	// Menu

	wxMenu *mFile = new wxMenu;

	wxMenuItem *openMenuItem = new wxMenuItem(mFile, wxID_OPEN);
	openMenuItem->SetBitmap(smallDocumentOpen);

	wxMenuItem *closeDocumentMenuItem = new wxMenuItem(mFile, wxID_CLOSE);
	closeDocumentMenuItem->SetBitmap(smallDocumentClose);

	wxMenuItem *saveDocumentMenuItem = new wxMenuItem(mFile, wxID_SAVE);
	saveDocumentMenuItem->SetBitmap(smallDocumentSave);

	wxMenuItem *saveAsDocumentMenuItem = new wxMenuItem(mFile, wxID_SAVEAS);
	saveAsDocumentMenuItem->SetBitmap(smallDocumentSaveAs);

	wxMenuItem *miExit = new wxMenuItem(mFile, wxID_EXIT);
	miExit->SetBitmap(smallApplicationExit);

	mFile->Append(openMenuItem);
	mFile->AppendSeparator();
	mFile->Append(closeDocumentMenuItem);
	mFile->AppendSeparator();
	mFile->Append(saveDocumentMenuItem);
	mFile->Append(saveAsDocumentMenuItem);
	mFile->AppendSeparator();
	mFile->Append(miExit);

	wxMenuBar *mbMenu = new wxMenuBar;
	mbMenu->Append(mFile, "&File");
	SetMenuBar(mbMenu);

	//

	// Create the engine viewport...
	CEditorRenderCanvas *rcViewport = new CEditorRenderCanvas(this);
	wxAuiPaneInfo piViewportInfo;
	piViewportInfo.Caption("Viewport");
	piViewportInfo.Top();
	piViewportInfo.Right();
	mManager->AddPane(rcViewport, piViewportInfo);

	// Create the material props...
	CEditorMaterialGlobalProperties *mgpProperties = new CEditorMaterialGlobalProperties(this);
	wxAuiPaneInfo piPropertiesInfo;
	piPropertiesInfo.Caption("Properties");
	piPropertiesInfo.Bottom();
	piPropertiesInfo.Right();
	mManager->AddPane(mgpProperties, piPropertiesInfo);

	//

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
	if (!mCurrent)
		return;

	// Ensure things have actually changed.
	tCurrentModified = pFileSystem_GetModifiedTime(sCurrentFilePath);
	if (tCurrentModified == tLastModified)
		return;

	engine->UnloadMaterial(mCurrent);
	engine->LoadMaterial(mCurrent->cPath);
	if (mCurrent)
	{
		tLastModified = tCurrentModified;

		engine->MaterialEditorDisplay(mCurrent);
	}
}

void CMaterialFrame::LoadMaterial(wxString sFileName)
{
	wxString sMaterialName = sFileName;
	if (sMaterialName.EndsWith(".material"))
	{
		// Remove the extension.
		sMaterialName.RemoveLast(9);

		if (mCurrent)
			engine->UnloadMaterial(mCurrent);

		mCurrent = engine->LoadMaterial(sMaterialName);
		if (mCurrent)
			// TODO: Handle this internally.
			engine->MaterialEditorDisplay(mCurrent);

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
			LoadMaterial(fdOpenMaterial->GetFilename());
	}
	break;
	}
}