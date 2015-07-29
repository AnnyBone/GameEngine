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

	FRAME_EVENT_SCRIPT,

	// View
	FRAME_EVENT_WIREFRAME,
	FRAME_EVENT_FLAT,
	FRAME_EVENT_TEXTURED,
	FRAME_EVENT_LIT,

	FRAME_EVENT_TRANSFORM,

	FRAME_EVENT_CUBE,
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

// View
EVT_MENU(FRAME_EVENT_WIREFRAME, CMaterialEditorFrame::OnViewMode)
EVT_MENU(FRAME_EVENT_FLAT, CMaterialEditorFrame::OnViewMode)
EVT_MENU(FRAME_EVENT_TEXTURED, CMaterialEditorFrame::OnViewMode)
EVT_MENU(FRAME_EVENT_LIT, CMaterialEditorFrame::OnViewMode)

EVT_TIMER(-1, CMaterialEditorFrame::OnTimer)

wxEND_EVENT_TABLE()

ConsoleVariable_t
cvEditorAutoReload = { "editor_ar", "1", true, false, "Enable or disable automatic reloading." },
cvEditorAutoReloadDelay = { "editor_ar_delay", "5", true, false, "Delay before attempting to automatically reload content." },
cvEditorShowProperties = { "editor_showproperties", "1", true, false, "Can show/hide the properties." },
cvEditorShowConsole = { "editor_showconsole", "1", true, false, "Can show/hide the console." };

CMaterialEditorFrame::CMaterialEditorFrame(const wxString & title, const wxPoint & pos, const wxSize & size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	manager = new wxAuiManager(this);

	// Load all required icons...

	wxImage::AddHandler(new wxPNGHandler);
	iconDocumentNew.LoadFile(PATH_16ICONS"actions/document-new.png", wxBITMAP_TYPE_PNG);
	smallFileOpen.LoadFile(PATH_16ICONS"actions/document-open.png", wxBITMAP_TYPE_PNG);
	iconDocumentSave.LoadFile(PATH_16ICONS"actions/document-save.png", wxBITMAP_TYPE_PNG);
	iconDocumentUndo.LoadFile(PATH_16ICONS"actions/edit-undo.png", wxBITMAP_TYPE_PNG);
	iconDocumentRedo.LoadFile(PATH_16ICONS"actions/edit-redo.png", wxBITMAP_TYPE_PNG);
	iconMediaPause.LoadFile(PATH_16ICONS"actions/media-playback-pause.png", wxBITMAP_TYPE_PNG);
	iconMediaPlay.LoadFile(PATH_16ICONS"actions/media-playback-start.png", wxBITMAP_TYPE_PNG);
	iconDocumentRefresh.LoadFile(PATH_16ICONS"actions/view-refresh.png", wxBITMAP_TYPE_PNG);
	iconShapeCube.LoadFile("resource/shape-cube.png", wxBITMAP_TYPE_PNG);
	iconShapeSphere.LoadFile("resource/shape-sphere.png", wxBITMAP_TYPE_PNG);
	iconShapePlane.LoadFile("resource/shape-plane.png", wxBITMAP_TYPE_PNG);
	iconScriptEdit.LoadFile(PATH_16ICONS"apps/accessories-text-editor.png", wxBITMAP_TYPE_PNG);
	smallTransform.LoadFile(PATH_16ICONS"actions/transform-move.png", wxBITMAP_TYPE_PNG);

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
	menuEdit->Append(wxID_UNDO);
	menuEdit->Append(wxID_REDO);
	menuEdit->AppendSeparator();

	wxMenu *menuView = new wxMenu;
	viewWireframe = menuView->AppendCheckItem(FRAME_EVENT_WIREFRAME, "&Wireframe");
	viewFlat = menuView->AppendCheckItem(FRAME_EVENT_FLAT, "&Flat");
	viewTextured = menuView->AppendCheckItem(FRAME_EVENT_TEXTURED, "Textured");
	viewLit = menuView->AppendCheckItem(FRAME_EVENT_LIT, "&Lit");
	menuView->AppendSeparator();
	menuView->Append(FRAME_EVENT_RELOAD, "Reload material");
	menuView->AppendSeparator();
	menuView->AppendCheckItem(wxID_ANY, "&Transform");
	menuView->AppendCheckItem(wxID_ANY, "&Rotate");
	menuView->AppendCheckItem(wxID_ANY, "&Scale");
	menuView->AppendSeparator();
	menuView->AppendCheckItem(ID_BUTTON_SPHERE, "Sphere");
	menuView->AppendCheckItem(FRAME_EVENT_CUBE, "&Cube");
	menuView->AppendCheckItem(ID_BUTTON_PLANE, "&Plane");

	viewLit->Check(true);

	wxMenu *menuWindow = new wxMenu;
	windowShowConsole = menuWindow->AppendCheckItem(FRAME_EVENT_SHOWCONSOLE, "&Console");
	windowShowProperties = menuWindow->AppendCheckItem(ID_WINDOW_PROPERTIES, "&Properties");

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuEdit, "&Edit");
	menuBar->Append(menuView, "&View");
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

	wxAuiPaneInfo toolbarInfo;
	toolbarInfo.Caption("Toolbar");
	toolbarInfo.ToolbarPane();
	toolbarInfo.Top();
	
	// File
	wxAuiToolBar *fileToolbar = new wxAuiToolBar(this);
	fileToolbar->AddTool(wxID_NEW, "New material", iconDocumentNew);
	fileToolbar->AddTool(wxID_OPEN, "Open material", smallFileOpen, "Open an existing material");
	fileToolbar->AddTool(wxID_SAVE, "Save material", iconDocumentSave, "Save the current material");
	fileToolbar->Realize();

	// Edit
	wxAuiToolBar *editToolbar = new wxAuiToolBar(this);
	editToolbar->AddTool(wxID_UNDO, "Undo", iconDocumentUndo, "Undo changes");
	editToolbar->AddTool(wxID_REDO, "Redo", iconDocumentRedo, "Redo changes");
	editToolbar->AddSeparator();
	editToolbar->AddTool(FRAME_EVENT_SCRIPT, "Script", iconScriptEdit, "Modify script");
	editToolbar->Realize();

	// View
	wxAuiToolBar *viewToolbar = new wxAuiToolBar(this);
	viewToolbar->AddTool(FRAME_EVENT_RELOAD, "Reload material", iconDocumentRefresh, "Reload the material");
	viewToolbar->AddSeparator();
	viewToolbar->AddTool(ID_BUTTON_SPHERE, "Sphere", iconShapeSphere, "Sphere shape");
	viewToolbar->AddTool(FRAME_EVENT_CUBE, "Cube", iconShapeCube, "Cube shape");
	viewToolbar->AddTool(ID_BUTTON_PLANE, "Plane", iconShapePlane, "Plane shape");
	viewToolbar->AddSeparator();
	viewToolbar->AddTool(ID_WINDOW_PAUSE, "Pause", iconMediaPause, "Pause simulation");
	viewToolbar->AddTool(ID_WINDOW_PLAY, "Play", iconMediaPlay, "Play simulation");
	viewToolbar->Realize();

	manager->AddPane(fileToolbar, toolbarInfo);
	manager->AddPane(editToolbar, toolbarInfo);
	manager->AddPane(viewToolbar, toolbarInfo);

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

	editorMaterialProperties = new CEditorMaterialGlobalProperties(this);
	wxAuiPaneInfo propertiesInfo;
	propertiesInfo.Caption("Properties");
	propertiesInfo.CloseButton(false);
	propertiesInfo.Right();
	manager->AddPane(editorMaterialProperties, propertiesInfo);

	manager->Update();

	dAutoReloadDelay = 0;
	dClientTime = 0;
}

CMaterialEditorFrame::~CMaterialEditorFrame()
{
	// Stop the engine from looping.
	StopEngineLoop();

	// Uninitialize the AUI manager.
	manager->UnInit();
}

void CMaterialEditorFrame::InitializeConsoleVariables()
{
	// TODO: These need to be able to update the editor, when modified.
	engine->RegisterConsoleVariable(&cvEditorShowConsole, NULL);
	engine->RegisterConsoleVariable(&cvEditorShowProperties, NULL);
	engine->RegisterConsoleVariable(&cvEditorAutoReload, NULL);
	engine->RegisterConsoleVariable(&cvEditorAutoReloadDelay, NULL);

	dAutoReloadDelay = cvEditorAutoReloadDelay.value;
}

void CMaterialEditorFrame::StartEngineLoop()
{
	// TODO: This isn't working...
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

	// TODO: Editor won't launch if engine isn't running... Can't we just make an assumption?
	if (engine->IsRunning())
	{
		// Perform the main loop.
		engine->Loop();

		// Keep the client-time updated.
		dClientTime = engine->GetClientTime();

		// Draw the main viewport.
		editorViewport->DrawFrame();
		editorViewport->Refresh();
	}

	// Check to see if it's time to check for changes.
	if (dAutoReloadDelay < dClientTime)
	{
		ReloadMaterial();
		dAutoReloadDelay = dClientTime + cvEditorAutoReloadDelay.value;
	}
}

void CMaterialEditorFrame::OnViewMode(wxCommandEvent &event)
{
	switch (event.GetId())
	{
	case FRAME_EVENT_WIREFRAME:
		viewFlat->Check(false);
		viewLit->Check(false);
		viewWireframe->Check(true);
		viewTextured->Check(false);

		engine->SetConsoleVariable("r_drawflat", "0");
		engine->SetConsoleVariable("r_showtris", "1");
		engine->SetConsoleVariable("r_fullbright", "0");
		break;
	case FRAME_EVENT_FLAT:
		viewFlat->Check(true);
		viewLit->Check(false);
		viewWireframe->Check(false);
		viewTextured->Check(false);

		engine->SetConsoleVariable("r_drawflat", "1");
		engine->SetConsoleVariable("r_showtris", "0");
		engine->SetConsoleVariable("r_fullbright", "0");
		break;
	case FRAME_EVENT_TEXTURED:
		viewFlat->Check(false);
		viewLit->Check(false);
		viewWireframe->Check(false);
		viewTextured->Check(true);

		engine->SetConsoleVariable("r_drawflat", "0");
		engine->SetConsoleVariable("r_showtris", "0");
		engine->SetConsoleVariable("r_fullbright", "1");
		break;
	case FRAME_EVENT_LIT:
		viewFlat->Check(false);
		viewLit->Check(true);
		viewWireframe->Check(false);
		viewTextured->Check(false);

		engine->SetConsoleVariable("r_drawflat", "0");
		engine->SetConsoleVariable("r_showtris", "0");
		engine->SetConsoleVariable("r_fullbright", "0");
		break;
	}
}

void CMaterialEditorFrame::OnReload(wxCommandEvent &event)
{
	ReloadMaterial();
}

void CMaterialEditorFrame::OnOpen(wxCommandEvent &event)
{
	char defaultPath[PLATFORM_MAX_PATH];

	sprintf_s(defaultPath, "%s", engine->GetBasePath());

	wxFileDialog *fileDialog = new wxFileDialog(
		this, 
		"Open File", 
		defaultPath,
		"", 
		"MATERIAL files (*.material)|*.material",wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (fileDialog->ShowModal() == wxID_OK)
	{
		wxString filename = fileDialog->GetFilename();
		filename.RemoveLast(9);

		// Unload the current material.
		Material_t *current = editorMaterialProperties->GetCurrent();
		if (current)
			engine->UnloadMaterial(current);
		
		Material_t *newMat = engine->LoadMaterial(filename);
		if (newMat)
		{
			// Update everything.
			currentFilePath = fileDialog->GetPath();
			lastTimeModified = currentTimeModified = pFileSystem_GetModifiedTime(currentFilePath);

			SetTitle(newMat->cName + wxString(" - ") + cApplicationTitle);

			// TODO: Handle this internally.
			engine->MaterialEditorDisplay(newMat);

			// TODO: This is dumb...
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
	wxAboutDialogInfo info;
	info.SetName("Yokote");
	info.SetCopyright("Copyright (C) 2011-2015 OldTimes Software");
	info.SetDescription("Editor for the Katana engine.");
	info.SetVersion(engine->GetVersion());

	wxAboutBox(info, this);
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

/*	Reload the currently active material.
*/
void CMaterialEditorFrame::ReloadMaterial()
{
	Material_t *current = editorMaterialProperties->GetCurrent();
	if (!current)
		return;

	// Ensure things have actually changed.
	currentTimeModified = pFileSystem_GetModifiedTime(currentFilePath);
	if (currentTimeModified == lastTimeModified)
		return;

	char cPath[PLATFORM_MAX_PATH];
	strcpy(cPath, current->cPath);

	engine->UnloadMaterial(current);

	Material_t *reloadedMat = engine->LoadMaterial(cPath);
	if (reloadedMat)
	{
		// Keep this up to date.
		lastTimeModified = currentTimeModified;

		engine->MaterialEditorDisplay(reloadedMat);

		editorMaterialProperties->SetCurrentMaterial(reloadedMat);
		editorMaterialProperties->Update();
	}
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