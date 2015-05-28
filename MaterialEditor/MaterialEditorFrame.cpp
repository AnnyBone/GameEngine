#include "MaterialEditorBase.h"

#include "MaterialEditorFrame.h"
#include "MaterialEditorRenderCanvas.h"

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

CMaterialEditorFrame::CMaterialEditorFrame(const wxString & title, const wxPoint & pos, const wxSize & size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	manager = new wxAuiManager(this);

	// Load all required icons...

	wxImage::AddHandler(new wxPNGHandler);
	largeNew.LoadFile(PATH_RESOURCES"16x16/actions/document-new.png", wxBITMAP_TYPE_PNG);
	largeOpen.LoadFile(PATH_RESOURCES"16x16/actions/document-open.png", wxBITMAP_TYPE_PNG);
	iconDocumentSave.LoadFile("resource/16x16/actions/document-save.png", wxBITMAP_TYPE_PNG);
	iconMediaPause.LoadFile(PATH_RESOURCES"16x16/actions/media-playback-pause.png", wxBITMAP_TYPE_PNG);
	iconMediaPlay.LoadFile(PATH_RESOURCES"16x16/actions/media-playback-start.png", wxBITMAP_TYPE_PNG);
	iconShapeCube.LoadFile("resource/shape-cube.png", wxBITMAP_TYPE_PNG);
	iconShapeSphere.LoadFile("resource/shape-sphere.png", wxBITMAP_TYPE_PNG);
	iconShapePlane.LoadFile("resource/shape-plane.png", wxBITMAP_TYPE_PNG);
	
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
	menuFile->Append(wxID_SAVE);
	menuFile->Append(wxID_SAVEAS);
	menuFile->Append(wxID_EXIT);

	wxMenu *menuEdit = new wxMenu;

	wxMenu *menuWindow = new wxMenu;
	menuWindow->AppendCheckItem(ID_WINDOW_CONSOLE, "&Console");
	menuWindow->AppendCheckItem(ID_WINDOW_PROPERTIES, "&Properties");

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

	// Initialize the timer...

	timer = new wxTimer(this);

	SetSize(size);
	Center();

	// Create the toolbar...

	wxAuiToolBar *toolbar = new wxAuiToolBar(this);
	toolbar->AddTool(wxID_NEW, "New material", largeNew);
	toolbar->AddTool(wxID_OPEN, "Open material", largeOpen, "Open an existing material");
	toolbar->AddTool(wxID_SAVE, "Save material", iconDocumentSave, "Save the current material");
	toolbar->AddSeparator();
	toolbar->AddTool(wxID_ANY, "Cube", iconShapeCube);
	toolbar->AddTool(wxID_ANY, "Sphere", iconShapeSphere);
	toolbar->AddTool(wxID_ANY, "Plane", iconShapePlane);
	toolbar->AddSeparator();
	toolbar->AddTool(wxID_ANY, "Pause", iconMediaPause);
	toolbar->AddTool(wxID_ANY, "Play", iconMediaPlay);
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
	editorViewport = new CMaterialEditorRenderCanvas(this, attributes);

	wxAuiPaneInfo viewportInfo;
	viewportInfo.Caption("Viewport");
	viewportInfo.Center();
	viewportInfo.Movable(true);
	viewportInfo.Floatable(true);
	viewportInfo.Dockable(true);
	viewportInfo.MaximizeButton(true);
	viewportInfo.CloseButton(false);

	manager->AddPane(editorViewport, viewportInfo);

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

	editorMaterialProperties = new CMaterialEditorMaterialGlobalProperties(this);
	
	wxAuiPaneInfo propertiesInfo;
	propertiesInfo.Caption("Properties");
	propertiesInfo.CloseButton(false);
	propertiesInfo.Right();

	manager->AddPane(editorMaterialProperties, propertiesInfo);

	manager->Update();
}

CMaterialEditorFrame::~CMaterialEditorFrame()
{
	manager->UnInit();
}

void CMaterialEditorFrame::StartEngineLoop(void)
{
	timer->Start();
}

void CMaterialEditorFrame::StopEngineLoop(void)
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
		editorViewport->DrawFrame();
		editorViewport->Refresh();
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
		Material_t *newMat = engine->LoadMaterial("debug_scroll");
		if (newMat)
		{
			engine->MaterialEditorDisplay(newMat);

			editorMaterialProperties->SetCurrentMaterial(newMat);
			editorMaterialProperties->Update();
		}
	}
}

void CMaterialEditorFrame::OnExit(wxCommandEvent &event)
{
	// Stop rendering!
	StopEngineLoop();

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
