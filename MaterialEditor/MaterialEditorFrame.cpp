#include "MaterialEditorBase.h"

#include "MaterialEditorFrame.h"
#include "MaterialEditorRenderCanvas.h"
#include "MaterialEditorPropertyWindow.h"
#include "MaterialEditorMaterialProperties.h"

#include <wx/splash.h>
#include <wx/splitter.h>

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
EVT_TIMER(-1, CMaterialEditorFrame::OnTimer)
wxEND_EVENT_TABLE()

CMaterialEditorMaterialGlobalProperties *globalMaterialProperties;

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

	// Initialize the timer...

	timer = new wxTimer(this);

	// Organise everything...

	wxBoxSizer *sizermain = new wxBoxSizer(wxVERTICAL);

	wxSplitterWindow *splittermain = new wxSplitterWindow(this, wxID_ANY);
	splittermain->SetSashGravity(0.5);
	splittermain->SetMinimumPaneSize(20); // Smalest size the

	wxPanel *pnl1 = new wxPanel(splittermain, wxID_ANY);

	wxBoxSizer *txt1sizer = new wxBoxSizer(wxVERTICAL);
	engineViewport = new CMaterialEditorRenderCanvas(pnl1);
	txt1sizer->Add(engineViewport, 1, wxEXPAND, 0);
	pnl1->SetSizer(txt1sizer);

	wxPanel *pnl2 = new wxPanel(splittermain, wxID_ANY);
	wxBoxSizer *txt2sizer = new wxBoxSizer(wxVERTICAL);
	wxPropertyGrid *materialProperties = new wxPropertyGrid(pnl2);
	materialProperties->Append(new wxPropertyCategory("Global"));
	globalMaterialProperties = new CMaterialEditorMaterialGlobalProperties(materialProperties);
	materialProperties->CenterSplitter(true);
	materialProperties->SetCellBackgroundColour(wxColour(0, 0, 0));
	materialProperties->SetCellTextColour(wxColour(0, 255, 0));
	materialProperties->SetEmptySpaceColour(wxColour(0, 0, 0));
	txt2sizer->Add(materialProperties, 1, wxEXPAND, 0);
	pnl2->SetSizer(txt2sizer);

	splittermain->SplitVertically(pnl1, pnl2);

	sizermain->Add(splittermain, 1, wxEXPAND, 0);

	this->SetSizer(sizermain);
	sizermain->SetSizeHints(this);

	SetSize(size);
}

void CMaterialEditorFrame::StartRendering(void)
{
	timer->Start();
}

void CMaterialEditorFrame::StopRendering(void)
{
	timer->Stop();
}

void CMaterialEditorFrame::OnTimer(wxTimerEvent &event)
{
	if (engine->IsRunning())
	{
		// Perform the main loop.
		engine->Loop();

		// Draw the main viewport.
		engineViewport->DrawFrame();
		engineViewport->Refresh();

#if 0
		// Draw the console.
		engineConsoleViewport->DrawFrame();
		engineConsoleViewport->Refresh();
#endif
	}
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
		{
			engine->MaterialEditorDisplay(newMat);

			globalMaterialProperties->Update(newMat);
		}
	}
}

void CMaterialEditorFrame::OnExit(wxCommandEvent &event)
{
	// Stop rendering!
	StopRendering();

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
#if 0
	propertyWindow->Show(!propertyWindow->IsShown());
	propertyWindow->SetPosition(wxPoint(GetPosition().x - 256, GetPosition().y));
#endif
}
