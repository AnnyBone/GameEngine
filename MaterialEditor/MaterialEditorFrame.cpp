#include "MaterialEditorBase.h"

#include "MaterialEditorFrame.h"
#include "MaterialEditorRenderCanvas.h"

enum
{
	FRAME_EVENT_SHOWCONSOLE,

	ID_WINDOW_SCRIPTEDITOR,	// Edit the material script.
	ID_WINDOW_PROPERTIES,	
	FRAME_EVENT_RELOAD,		// Reload the current material.
	ID_WINDOW_PLAY,			// Play simulation.
	ID_WINDOW_PAUSE,		// Pause simulation.

	ID_BUTTON_CUBE,
	ID_BUTTON_SPHERE,
	ID_BUTTON_PLANE
};

wxBEGIN_EVENT_TABLE(CMaterialEditorFrame, wxFrame)

EVT_MENU(wxID_OPEN, CMaterialEditorFrame::OnOpen)
EVT_MENU(wxID_SAVE, CMaterialEditorFrame::OnSave)
EVT_MENU(wxID_EXIT, CMaterialEditorFrame::OnExit)
EVT_MENU(wxID_ABOUT, CMaterialEditorFrame::OnAbout)
EVT_MENU(FRAME_EVENT_SHOWCONSOLE, CMaterialEditorFrame::OnConsole)
EVT_MENU(ID_WINDOW_PROPERTIES, CMaterialEditorFrame::OnProperties)
EVT_MENU(FRAME_EVENT_RELOAD, CMaterialEditorFrame::OnReload)
EVT_MENU(ID_WINDOW_PLAY, CMaterialEditorFrame::OnPlay)
EVT_MENU(ID_WINDOW_PAUSE, CMaterialEditorFrame::OnPause)

EVT_TIMER(-1, CMaterialEditorFrame::OnTimer)

wxEND_EVENT_TABLE()

ConsoleVariable_t
cvEditorShowProperties = { "editor_showproperties", "1", true, false, "Can show/hide the properties." },
cvEditorShowConsole = { "editor_showconsole", "1", true, false, "Can show/hide the console." };

CMaterialEditorFrame::CMaterialEditorFrame(const wxString & title, const wxPoint & pos, const wxSize & size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	manager = new wxAuiManager(this);

	// Load all required icons...

	wxImage::AddHandler(new wxPNGHandler);
	iconDocumentNew.LoadFile(PATH_RESOURCES"16x16/actions/document-new.png", wxBITMAP_TYPE_PNG);
	largeOpen.LoadFile(PATH_RESOURCES"16x16/actions/document-open.png", wxBITMAP_TYPE_PNG);
	iconDocumentSave.LoadFile("resource/16x16/actions/document-save.png", wxBITMAP_TYPE_PNG);
	iconDocumentRefresh.LoadFile(PATH_SILK"arrow_refresh.png", wxBITMAP_TYPE_PNG);
	iconDocumentUndo.LoadFile(PATH_SILK"arrow_undo.png", wxBITMAP_TYPE_PNG);
	iconDocumentRedo.LoadFile(PATH_SILK"arrow_redo.png", wxBITMAP_TYPE_PNG);
	iconMediaPause.LoadFile(PATH_SILK"control_pause.png", wxBITMAP_TYPE_PNG);
	iconMediaPlay.LoadFile(PATH_SILK"control_play.png", wxBITMAP_TYPE_PNG);
	iconShapeCube.LoadFile("resource/shape-cube.png", wxBITMAP_TYPE_PNG);
	iconShapeSphere.LoadFile("resource/shape-sphere.png", wxBITMAP_TYPE_PNG);
	iconShapePlane.LoadFile("resource/shape-plane.png", wxBITMAP_TYPE_PNG);

	pLog_Write(MATERIALEDITOR_LOG, "Setting frame icon...\n");

	SetIcon(wxIcon("resource/icon-material.png", wxBITMAP_TYPE_PNG));
	
	// Display the splash screen...

	pLog_Write(MATERIALEDITOR_LOG, "Displaying splash screen...\n");

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
	windowShowConsole = menuWindow->AppendCheckItem(FRAME_EVENT_SHOWCONSOLE, "&Console");
	windowShowProperties = menuWindow->AppendCheckItem(ID_WINDOW_PROPERTIES, "&Properties");

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
	SetSize(size);
	Center();

	// Initialize the timer...

	timer = new wxTimer(this);

	// Create the toolbar...
	
	wxAuiToolBar *toolbar = new wxAuiToolBar(this);
	toolbar->AddTool(wxID_NEW, "New material", iconDocumentNew);
	toolbar->AddTool(wxID_OPEN, "Open material", largeOpen, "Open an existing material");
	toolbar->AddTool(wxID_SAVE, "Save material", iconDocumentSave, "Save the current material");
	toolbar->AddSeparator();
	toolbar->AddTool(wxID_UNDO, "Undo", iconDocumentUndo, "Undo changes");
	toolbar->AddTool(wxID_REDO, "Redo", iconDocumentRedo, "Redo changes");
	toolbar->AddTool(FRAME_EVENT_RELOAD, "Reload material", iconDocumentRefresh, "Reload the material");
	toolbar->AddSeparator();
	toolbar->AddTool(ID_BUTTON_CUBE, "Cube", iconShapeCube, "Cube shape");
	toolbar->AddTool(ID_BUTTON_SPHERE, "Sphere", iconShapeSphere, "Sphere shape");
	toolbar->AddTool(ID_BUTTON_PLANE, "Plane", iconShapePlane, "Plane shape");
	toolbar->AddSeparator();
	toolbar->AddTool(ID_WINDOW_PAUSE, "Pause", iconMediaPause, "Pause simulation");
	toolbar->AddTool(ID_WINDOW_PLAY, "Play", iconMediaPlay, "Play simulation");
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
		WX_GL_SAMPLES, 4,
		WX_GL_SAMPLE_BUFFERS, 1
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
	
	editorConsolePanel = new CMaterialEditorConsolePanel(this);
	wxAuiPaneInfo consoleInfo;
	consoleInfo.Caption("Console");
	consoleInfo.Bottom();
	consoleInfo.Movable(true);
	consoleInfo.Floatable(true);
	consoleInfo.MaximizeButton(true);
	consoleInfo.CloseButton(false);
	manager->AddPane(editorConsolePanel, consoleInfo);

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
	timer->Stop();

	manager->UnInit();
}

void CMaterialEditorFrame::InitializeConsoleVariables()
{
	// TODO: These need to be able to update the editor, when modified.
	engine->RegisterConsoleVariable(&cvEditorShowConsole, NULL);
	engine->RegisterConsoleVariable(&cvEditorShowProperties, NULL);
}

void CMaterialEditorFrame::StartEngineLoop()
{
	if (!cvEditorShowConsole.bValue)
	{
		manager->GetPane(editorConsolePanel).Show(false);
		manager->Update();
	}
	else
		windowShowConsole->Check(true);

	timer->Start();
}

void CMaterialEditorFrame::StopEngineLoop()
{
	timer->Stop();
}

void CMaterialEditorFrame::OnPause(wxCommandEvent &event)
{
	timer->Stop();
}

void CMaterialEditorFrame::OnPlay(wxCommandEvent &event)
{
	timer->Start();
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

void CMaterialEditorFrame::OnReload(wxCommandEvent &event)
{
	Material_t *current = editorMaterialProperties->GetCurrent();
	if (!current)
		return;

	Material_t *reloadedMat = engine->LoadMaterial(current->cPath);
	if (reloadedMat)
	{
		engine->MaterialEditorDisplay(reloadedMat);

		editorMaterialProperties->SetCurrentMaterial(reloadedMat);
		editorMaterialProperties->Update();
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
		wxString filename = fileDialog->GetFilename();
		filename.RemoveLast(9);

		Material_t *newMat = engine->LoadMaterial(filename);
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

	engine->Shutdown();

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
{
	if (manager->GetPane(editorConsolePanel).IsShown())
	{
		manager->GetPane(editorConsolePanel).Show(false);
		engine->SetConsoleVariable(cvEditorShowConsole.name, "0");
	}
	else
	{
		manager->GetPane(editorConsolePanel).Show(true);
		engine->SetConsoleVariable(cvEditorShowConsole.name, "1");
	}

	manager->Update();
}

void CMaterialEditorFrame::OnProperties(wxCommandEvent &event)
{
#if 0
	propertyWindow->Show(!propertyWindow->IsShown());
	propertyWindow->SetPosition(wxPoint(GetPosition().x - 256, GetPosition().y));
#endif
}

void CMaterialEditorFrame::PrintMessage(char *text)
{
	if (!editorConsolePanel)
		return;

	editorConsolePanel->textConsoleOut->SetDefaultStyle(wxTextAttr(*wxGREEN));
	editorConsolePanel->textConsoleOut->AppendText(text);
}

void CMaterialEditorFrame::PrintWarning(char *text)
{
	if (!editorConsolePanel)
		return;

	editorConsolePanel->textConsoleOut->SetDefaultStyle(wxTextAttr(*wxYELLOW));
	editorConsolePanel->textConsoleOut->AppendText(text);
}

void CMaterialEditorFrame::PrintError(char *text)
{
	if (!editorConsolePanel)
		return;

	editorConsolePanel->textConsoleOut->SetDefaultStyle(wxTextAttr(*wxRED));
	editorConsolePanel->textConsoleOut->AppendText(text);
}