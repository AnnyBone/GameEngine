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
	ID_WINDOW_SCRIPTEDITOR,
	ID_WINDOW_PROPERTIES,

	ID_CONSOLE_INPUT,

	ID_BUTTON_COMMAND
};

wxBEGIN_EVENT_TABLE(CMaterialEditorFrame, wxFrame)

	EVT_CHAR_HOOK(CMaterialEditorFrame::OnKey)

	EVT_MENU(wxID_OPEN, CMaterialEditorFrame::OnOpen)
	EVT_MENU(wxID_SAVE, CMaterialEditorFrame::OnSave)
	EVT_MENU(wxID_EXIT, CMaterialEditorFrame::OnExit)
	EVT_MENU(wxID_ABOUT, CMaterialEditorFrame::OnAbout)
	EVT_MENU(ID_WINDOW_CONSOLE, CMaterialEditorFrame::OnConsole)
	EVT_MENU(ID_WINDOW_PROPERTIES, CMaterialEditorFrame::OnProperties)

	EVT_BUTTON(ID_BUTTON_COMMAND, CMaterialEditorFrame::OnCommand)

	EVT_TIMER(-1, CMaterialEditorFrame::OnTimer)

wxEND_EVENT_TABLE()

CMaterialEditorMaterialGlobalProperties *globalMaterialProperties;

CMaterialEditorFrame::CMaterialEditorFrame(const wxString & title, const wxPoint & pos, const wxSize & size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	manager = new wxAuiManager(this);

	// Load all required icons...

	wxImage::AddHandler(new wxPNGHandler);
	largeNew.LoadFile(PATH_RESOURCES"32/document-new.png", wxBITMAP_TYPE_PNG);
	largeOpen.LoadFile(PATH_RESOURCES"32/folder-open.png", wxBITMAP_TYPE_PNG);
	largeExit.LoadFile(PATH_RESOURCES"32/process-stop.png", wxBITMAP_TYPE_PNG);
	largeScriptEdit.LoadFile(PATH_RESOURCES"32/accessories-text-editor.png", wxBITMAP_TYPE_PNG);

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

	CreateStatusBar(3);
	SetStatusText("Initialized");
	SetStatusText("Currently awaiting user input...", 1);

	// Initialize the timer...

	timer = new wxTimer(this);

	SetSize(size);
	Center();

	// Create the toolbar...

	wxAuiToolBar *toolbar = new wxAuiToolBar(this);
	toolbar->AddTool(wxID_NEW, "New material", largeNew);
	toolbar->AddTool(wxID_OPEN, "Open material", largeOpen, "Open an existing file");
	toolbar->AddSeparator();
	toolbar->AddTool(ID_WINDOW_SCRIPTEDITOR, "Edit material script", largeScriptEdit);
	toolbar->AddSeparator();
	toolbar->AddTool(wxID_EXIT, "Exit application", largeExit, "Exit the application");
	toolbar->Realize();

	wxAuiPaneInfo toolbarInfo;
	toolbarInfo.Caption("Toolbar");
	toolbarInfo.ToolbarPane();
	toolbarInfo.Top();
	toolbarInfo.Movable(false);
	toolbarInfo.Floatable(false);
	toolbarInfo.Dockable(false);
	toolbarInfo.MinSize(wxSize(GetSize().GetWidth(),16));
	toolbarInfo.MaxSize(wxSize(GetSize().GetWidth(),32));

	manager->AddPane(toolbar, toolbarInfo);

	// Create the engine viewport...

	int attributes[] = {
		WX_GL_DEPTH_SIZE, 24,
		WX_GL_STENCIL_SIZE, 8,
		WX_GL_MIN_RED, 8,
		WX_GL_MIN_GREEN, 8,
		WX_GL_MIN_BLUE, 8,
		WX_GL_MIN_ALPHA, 8,
		WX_GL_MIN_ACCUM_RED, 8,
		WX_GL_MIN_ACCUM_GREEN, 8,
		WX_GL_MIN_ACCUM_BLUE, 8,
		WX_GL_MIN_ACCUM_ALPHA, 8,
		//WX_GL_DOUBLEBUFFER,	1,
	};
	engineViewport = new CMaterialEditorRenderCanvas(this, attributes);

	wxAuiPaneInfo viewportInfo;
	viewportInfo.Caption("Viewport");
	viewportInfo.Center();
	viewportInfo.Movable(true);
	viewportInfo.Floatable(true);
	viewportInfo.Dockable(true);
	viewportInfo.MaximizeButton(true);
	viewportInfo.CloseButton(false);

	manager->AddPane(engineViewport, viewportInfo);

	// Create the console...

	wxPanel *consolePanel = new wxPanel(this);
	wxAuiPaneInfo consoleInfo;
	consoleInfo.Caption("Console");
	consoleInfo.Bottom();
	consoleInfo.Movable(true);
	consoleInfo.Floatable(true);
	consoleInfo.MaximizeButton(true);
	consoleInfo.CloseButton(false);

	wxBoxSizer *vSizer = new wxBoxSizer(wxVERTICAL);

	textConsoleOut = new wxTextCtrl(consolePanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxTE_DONTWRAP);
	textConsoleOut->SetBackgroundColour(wxColour(0, 0, 0));
	textConsoleOut->SetForegroundColour(wxColour(0, 255, 0));

	vSizer->Add(textConsoleOut,1,wxEXPAND|wxTOP|wxBOTTOM|wxLEFT|wxRIGHT);

	wxBoxSizer *hSizer = new wxBoxSizer(wxHORIZONTAL);
	hSizer->Add(new wxButton(consolePanel, ID_BUTTON_COMMAND, "Submit"), 0, wxRIGHT);

	textConsoleIn = new wxTextCtrl(consolePanel, ID_CONSOLE_INPUT, "");
	hSizer->Add(textConsoleIn, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP);

	hSizer->SetSizeHints(consolePanel);

	vSizer->Add(hSizer, 0, wxEXPAND | wxLEFT| wxRIGHT | wxTOP);

	consolePanel->SetSizer(vSizer);
	consolePanel->SetSize(wxSize(wxDefaultSize.x, 256));

	manager->AddPane(consolePanel, consoleInfo);

	// Create the material props...

	wxPropertyGrid *materialProperties = new wxPropertyGrid(this);
	materialProperties->Append(new wxPropertyCategory("Global"));
	globalMaterialProperties = new CMaterialEditorMaterialGlobalProperties(materialProperties);
	materialProperties->CenterSplitter(true);
	materialProperties->SetCellBackgroundColour(wxColour(0, 0, 0));
	materialProperties->SetCellTextColour(wxColour(0, 255, 0));
	materialProperties->SetEmptySpaceColour(wxColour(0, 0, 0));
	materialProperties->SetCaptionBackgroundColour(wxColour(70, 70, 70));
	materialProperties->SetCaptionTextColour(wxColour(255, 255, 255));
	materialProperties->SetMarginColour(wxColour(50, 50, 50));
	materialProperties->SetSize(wxSize(300, wxDefaultSize.y));
	
	wxAuiPaneInfo propertiesInfo;
	propertiesInfo.Caption("Properties");
	propertiesInfo.CloseButton(false);
	propertiesInfo.Right();

	manager->AddPane(materialProperties, propertiesInfo);

	manager->Update();
}

CMaterialEditorFrame::~CMaterialEditorFrame()
{
	manager->UnInit();
}

void CMaterialEditorFrame::StartRendering(void)
{
	timer->Start();
}

void CMaterialEditorFrame::StopRendering(void)
{
	timer->Stop();
}

void CMaterialEditorFrame::PrintMessage(char *text)
{
	textConsoleOut->SetDefaultStyle(wxTextAttr(*wxGREEN));
	textConsoleOut->AppendText(text);
}

void CMaterialEditorFrame::PrintWarning(char *text)
{
	textConsoleOut->SetDefaultStyle(wxTextAttr(*wxYELLOW));
	textConsoleOut->AppendText(text);
}

void CMaterialEditorFrame::PrintError(char *text)
{
	textConsoleOut->SetDefaultStyle(wxTextAttr(*wxRED));
	textConsoleOut->AppendText(text);
}

void CMaterialEditorFrame::OnKey(wxKeyEvent &event)
{
	event.DoAllowNextEvent();

	switch (event.GetId())
	{
	case ID_CONSOLE_INPUT:
		if (event.GetKeyCode() == WXK_RETURN)
		{
			// Send the command to the engine.
			engine->InsertConsoleCommand(wxString(textConsoleIn->GetValue()));

			// Clear the input box.
			textConsoleIn->Clear();
		}
		break;
	}
}

void CMaterialEditorFrame::OnTimer(wxTimerEvent &event)
{
	static int consoleOutLength = 0;

	if (engine->IsRunning())
	{
		// Perform the main loop.
		engine->Loop();

		// Draw the main viewport.
		engineViewport->DrawFrame();
		engineViewport->Refresh();
	}
}

void CMaterialEditorFrame::OnCommand(wxCommandEvent &event)
{
	// Send the command to the engine.
	engine->InsertConsoleCommand(wxString(textConsoleIn->GetValue()));

	// Clear the input box.
	textConsoleIn->Clear();
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
