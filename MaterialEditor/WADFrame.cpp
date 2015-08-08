#include "EditorBase.h"

#include "WADFrame.h"

#define	WAD_TITLE "WAD Tool"

CWADFrame::CWADFrame(wxWindow* parent, wxWindowID id) 
	: wxFrame(parent, id, WAD_TITLE, wxDefaultPosition, wxSize(512, 512))
{
	mManager = new wxAuiManager(this);

	// Menu

	wxMenu *mFile = new wxMenu;

#if 0
	wxMenuItem *openMenuItem = new wxMenuItem(mFile, wxID_OPEN);
	openMenuItem->SetBitmap(smallDocumentOpen);

	wxMenuItem *closeDocumentMenuItem = new wxMenuItem(mFile, wxID_CLOSE);
	closeDocumentMenuItem->SetBitmap(smallDocumentClose);
#endif

	wxMenuItem *saveDocumentMenuItem = new wxMenuItem(mFile, wxID_SAVE);
	saveDocumentMenuItem->SetBitmap(smallDocumentSave);

	wxMenuItem *saveAsDocumentMenuItem = new wxMenuItem(mFile, wxID_SAVEAS);
	saveAsDocumentMenuItem->SetBitmap(smallDocumentSaveAs);

	wxMenuItem *miExit = new wxMenuItem(mFile, wxID_EXIT);
	miExit->SetBitmap(smallApplicationExit);

#if 0
	mFile->Append(openMenuItem);
	mFile->AppendSeparator();
	mFile->Append(closeDocumentMenuItem);
	mFile->AppendSeparator();
#endif
	mFile->Append(saveDocumentMenuItem);
	mFile->Append(saveAsDocumentMenuItem);
	mFile->AppendSeparator();
	mFile->Append(miExit);

	wxMenuBar *mbMenu = new wxMenuBar;
	mbMenu->Append(mFile, "&File");
	SetMenuBar(mbMenu);

	// List

	wxListCtrl *lcObjectList = new wxListCtrl(this);

	wxListItem liColumn;
	liColumn.SetId(0);
	liColumn.SetText("Name");
	liColumn.SetWidth(50);
	lcObjectList->InsertColumn(0, liColumn);

	liColumn.SetId(1);
	liColumn.SetText("Type");
	liColumn.SetWidth(50);
	lcObjectList->InsertColumn(1, liColumn);

	wxAuiPaneInfo piList;
	piList.Caption("Contents");
	piList.Movable(true);
	piList.Floatable(true);
	piList.Left();
	mManager->AddPane(lcObjectList, piList);

	//

	Layout();
	Centre();
}

CWADFrame::~CWADFrame()
{
	mManager->UnInit();
}
