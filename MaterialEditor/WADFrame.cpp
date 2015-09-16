#include "EditorBase.h"

#include "WADFrame.h"

#define	WAD_TITLE "WAD Tool"

CWADFrame::CWADFrame(wxWindow* parent, wxWindowID id) 
	: wxFrame(parent, id, WAD_TITLE, wxDefaultPosition, wxSize(512, 512))
{
	SetIcon(wxIcon("resource/icon-wad.png", wxBITMAP_TYPE_PNG));

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

	wxAuiPaneInfo iPaneInfo;
	iPaneInfo.Movable(true);
	iPaneInfo.Floatable(true);
	iPaneInfo.Left();

	// List

	iPaneInfo.Caption("Textures");

	wxListCtrl *lcObjectList = new wxListCtrl(
		this,
		wxID_ANY,
		wxDefaultPosition,
		wxDefaultSize,
		wxLC_ICON|wxLC_EDIT_LABELS);

	wxListItem liColumn;
	liColumn.SetId(0);
	liColumn.SetText("Name");
	liColumn.SetWidth(50);
	lcObjectList->InsertColumn(0, liColumn);

	liColumn.SetId(1);
	liColumn.SetText("Type");
	liColumn.SetWidth(50);
	lcObjectList->InsertColumn(1, liColumn);

	lcObjectList->InsertItem(0, "HELLO");
	lcObjectList->InsertItem(0, "WE");
	lcObjectList->InsertItem(0, "ARE");
	lcObjectList->InsertItem(0, "PLACEMENT");
	lcObjectList->InsertItem(0, "HOLDERS");
	lcObjectList->InsertItem(0, "HELLO");
	lcObjectList->InsertItem(0, "WE");
	lcObjectList->InsertItem(0, "ARE");
	lcObjectList->InsertItem(0, "PLACEMENT");
	lcObjectList->InsertItem(0, "HOLDERS");
	lcObjectList->InsertItem(0, "HELLO");
	lcObjectList->InsertItem(0, "WE");
	lcObjectList->InsertItem(0, "ARE");
	lcObjectList->InsertItem(0, "PLACEMENT");
	lcObjectList->InsertItem(0, "HOLDERS");
	lcObjectList->InsertItem(0, "HELLO");
	lcObjectList->InsertItem(0, "WE");
	lcObjectList->InsertItem(0, "ARE");
	lcObjectList->InsertItem(0, "PLACEMENT");
	lcObjectList->InsertItem(0, "HOLDERS");
	lcObjectList->InsertItem(0, "HELLO");
	lcObjectList->InsertItem(0, "WE");
	lcObjectList->InsertItem(0, "ARE");
	lcObjectList->InsertItem(0, "PLACEMENT");
	lcObjectList->InsertItem(0, "HOLDERS");
	lcObjectList->InsertItem(0, "HELLO");
	lcObjectList->InsertItem(0, "WE");
	lcObjectList->InsertItem(0, "ARE");
	lcObjectList->InsertItem(0, "PLACEMENT");
	lcObjectList->InsertItem(0, "HOLDERS");
	lcObjectList->InsertItem(0, "HELLO");
	lcObjectList->InsertItem(0, "WE");
	lcObjectList->InsertItem(0, "ARE");
	lcObjectList->InsertItem(0, "PLACEMENT");
	lcObjectList->InsertItem(0, "HOLDERS");

	mManager->AddPane(lcObjectList, iPaneInfo);

	Layout();
	Centre();
}

CWADFrame::~CWADFrame()
{
	mManager->UnInit();
}
