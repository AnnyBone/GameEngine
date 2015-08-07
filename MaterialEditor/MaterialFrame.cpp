#include "EditorBase.h"

#include "MaterialFrame.h"

#define	WAD_TITLE "Material Tool"

CMaterialFrame::CMaterialFrame(wxWindow* parent, wxWindowID id)
	: wxFrame(parent, id, WAD_TITLE, wxDefaultPosition, wxSize(640, 640))
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

	//

	Layout();
	Centre();
}

CMaterialFrame::~CMaterialFrame()
{
}
