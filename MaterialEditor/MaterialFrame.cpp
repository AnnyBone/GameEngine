#include "EditorBase.h"

#include "MaterialFrame.h"

#include "EditorViewportPanel.h"
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
	: wxFrame(parent, id, WAD_TITLE, wxDefaultPosition, wxSize(1024, 640))
{
    Show();

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

	iPaneInfo.PinButton(true);
	iPaneInfo.BestSize(wxSize(256, 256));

	// Create the engine viewport...
	CEditorViewportPanel *rcViewport = new CEditorViewportPanel(this);
	iPaneInfo.Caption("Viewport");
	iPaneInfo.Top();
	iPaneInfo.Right();
	mManager->AddPane(rcViewport, iPaneInfo);

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
		{
			LoadMaterial(fdOpenMaterial->GetFilename());

			cMaterialScript->LoadFile(fdOpenMaterial->GetPath());
		}
	}
	break;
	}
}