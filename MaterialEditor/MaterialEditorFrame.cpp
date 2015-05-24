#include "MaterialEditorBase.h"

#include "MaterialEditorFrame.h"
#include "MaterialEditorRenderCanvas.h"
#include "MaterialEditorPropertyWindow.h"

#include <wx/splash.h>

enum
{
	ID_WINDOW_CONSOLE,
	ID_WINDOW_PROPERTIES
};

wxBEGIN_EVENT_TABLE(CMaterialEditorFrame, wxFrame)
EVT_MENU(wxID_OPEN, CMaterialEditorFrame::OnOpen)
EVT_MENU(wxID_SAVE, CMaterialEditorFrame::OnSave)
EVT_MENU(wxID_EXIT, CMaterialEditorFrame::OnExit)
EVT_MENU(wxID_ABOUT, CMaterialEditorFrame::OnAbout)
EVT_MENU(ID_WINDOW_CONSOLE, CMaterialEditorFrame::OnConsole)
EVT_MENU(ID_WINDOW_PROPERTIES, CMaterialEditorFrame::OnProperties)
wxEND_EVENT_TABLE()

CMaterialEditorPropertyWindow *propertyWindow;

CMaterialEditorFrame::CMaterialEditorFrame(const wxString & title, const wxPoint & pos, const wxSize & size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	// Load all required icons...

	Center();

	wxImage::AddHandler(new wxPNGHandler);
	largeExit.LoadFile(PATH_RESOURCES"door_open.png", wxBITMAP_TYPE_PNG);
	largeOpen.LoadFile(PATH_RESOURCES"folder.png", wxBITMAP_TYPE_PNG);

	// Display the splash screen...

	wxBitmap splashImage;
	if (splashImage.LoadFile(PATH_RESOURCES"material_editor/splash.png", wxBITMAP_TYPE_PNG))
	{
		new wxSplashScreen(
			splashImage,
			wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
			4000,
			NULL,
			-1,
			wxDefaultPosition,
			wxDefaultSize,
			wxBORDER_SIMPLE | wxSTAY_ON_TOP);
	}

	// Set the menu up...

	wxMenu *menuFile = new wxMenu;
	menuFile->Append(wxID_OPEN);
	menuFile->Append(wxID_EXIT);

	wxMenu *menuEdit = new wxMenu;

	wxMenu *menuWindow = new wxMenu;
	menuWindow->Append(ID_WINDOW_CONSOLE, "&Console");
	menuWindow->Append(ID_WINDOW_PROPERTIES, "&Properties");

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuEdit, "&Edit");
	menuBar->Append(menuWindow, "&Window");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);

	// Set the toolbar up...

	wxToolBar *toolbar = CreateToolBar();
	toolbar->AddTool(wxID_OPEN, "Open file", largeOpen, "Open an existing file");
	toolbar->AddSeparator();
	toolbar->AddTool(wxID_EXIT, "Exit application", largeExit, "Exit the application");
	toolbar->Realize();

	// Status bar...

	CreateStatusBar(3);
	SetStatusText("Initialized");
	SetStatusText("Currently awaiting user input...", 1);

	// Set the icon...

	SetIcon(wxIcon(PATH_RESOURCES"/material_editor/icon.png",wxBITMAP_TYPE_PNG));

	// Create the OpenGL canvas...

	wxGridSizer *sizer = new wxGridSizer(wxHORIZONTAL);

	sizer->Add(new CMaterialEditorRenderCanvas(this), 1, wxEXPAND);
	sizer->Add(new wxButton(this, -1, wxT("7")), 1, wxEXPAND | wxLEFT);
	
	propertyWindow = new CMaterialEditorPropertyWindow(wxPoint(GetPosition().x - 256, GetPosition().y), wxSize(256, 480));
}

void CMaterialEditorFrame::OnOpen(wxCommandEvent &event)
{
	char defaultPath[PLATFORM_MAX_PATH];

	sprintf_s(defaultPath, "%s/%s", engine->GetBasePath(), engine->GetMaterialPath());

	wxFileDialog *fileDialog = new wxFileDialog(
		this, 
		"Open MATERIAL file", 
		defaultPath,
		"", 
		"MATERIAL files (*.material)|*.material",wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (fileDialog->ShowModal() == wxID_OK)
	{
		Material_t *newMat = engine->LoadMaterial("dngalax3");
		if (newMat)
			engine->MaterialEditorDisplay(newMat);
	}
}

void CMaterialEditorFrame::OnExit(wxCommandEvent &event)
{
	// Close the properties first.
	propertyWindow->Close(true);

	// Close the frame and app.
	Close(true);
}

void CMaterialEditorFrame::OnAbout(wxCommandEvent &event)
{
	wxMessageBox(
		wxString(
		"Copyright (C) 2011-2015 OldTimes Software\n\
		Developed by Mark \"hogsy\" Sowden"),
		"About Material Editor", wxOK | wxICON_INFORMATION);
}

void CMaterialEditorFrame::OnSave(wxCommandEvent &event)
{}

void CMaterialEditorFrame::OnConsole(wxCommandEvent &event)
{}

void CMaterialEditorFrame::OnProperties(wxCommandEvent &event)
{
	propertyWindow->Show(!propertyWindow->IsShown());
	propertyWindow->SetPosition(wxPoint(GetPosition().x - 256, GetPosition().y));
}
