/*	Copyright (C) 2011-2016 OldTimes Software

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "EditorBase.h"

#include "base_rendercanvas.h"
#include "base_viewportpanel.h"

#include "main_frame.h"

void MainViewportPanel::Draw()
{
	// Currently doing nothing...
	engine->DrawGrid(0, 0, -16, 16);

	engine->DrawResetCanvas();
	engine->DrawSetCanvas(CANVAS_DEFAULT);
	engine->DrawString(10, 10, "Hello World!");
}

class XViewportPanel : public BaseViewportPanel
{
public:
	XViewportPanel(wxWindow *parent) : BaseViewportPanel(parent) {}

	virtual void Draw();

protected:
private:
};

void XViewportPanel::Draw()
{
	engine->DrawResetCanvas();
	engine->DrawSetCanvas(CANVAS_DEFAULT);
	engine->DrawGrid(0, 0, 0, 8);
}

enum
{
	FRAME_EVENT_SHOWCONSOLE,

	ID_WINDOW_SCRIPTEDITOR,	// Edit the material script.
	ID_WINDOW_PROPERTIES,
	FRAME_EVENT_RELOAD,		// Reload the current material.
//	ID_WINDOW_PLAY,			// Play simulation.
//	ID_WINDOW_PAUSE,		// Pause simulation.

	// View
	FRAME_EVENT_WIREFRAME,
	FRAME_EVENT_FLAT,
	FRAME_EVENT_TEXTURED,
	FRAME_EVENT_LIT,

	// Tools
	FRAME_EVENT_WADTOOL,		// Open WAD tool
	FRAME_EVENT_MATERIALTOOL,	// Open Material tool
	MAIN_EVENT_MODELVIEWER,		// Open model viewer.

	FRAME_EVENT_TRANSFORM,

	FRAME_EVENT_CUBE,
	ID_BUTTON_SPHERE,
	ID_BUTTON_PLANE
};

wxBEGIN_EVENT_TABLE(CEditorFrame, wxFrame)

	EVT_MENU(wxID_OPEN, CEditorFrame::OnOpen)
	EVT_MENU(wxID_SAVE, CEditorFrame::OnSave)
	EVT_MENU(wxID_EXIT, CEditorFrame::OnExit)
	EVT_MENU(wxID_ABOUT, CEditorFrame::OnAbout)
	EVT_MENU(wxID_PREFERENCES, CEditorFrame::OnPreferences)
	EVT_MENU(FRAME_EVENT_SHOWCONSOLE, CEditorFrame::OnConsole)
	EVT_MENU(ID_WINDOW_PROPERTIES, CEditorFrame::OnProperties)
	EVT_MENU(FRAME_EVENT_RELOAD, CEditorFrame::OnReload)
//	EVT_MENU(ID_WINDOW_PLAY, CEditorFrame::OnPlay)
//	EVT_MENU(ID_WINDOW_PAUSE, CEditorFrame::OnPause)

	// View
	EVT_MENU(FRAME_EVENT_WIREFRAME, CEditorFrame::OnView)
	EVT_MENU(FRAME_EVENT_FLAT, CEditorFrame::OnView)
	EVT_MENU(FRAME_EVENT_TEXTURED, CEditorFrame::OnView)
	EVT_MENU(FRAME_EVENT_LIT, CEditorFrame::OnView)

	// Tools
	EVT_MENU(FRAME_EVENT_WADTOOL, CEditorFrame::OnTool)
	EVT_MENU(FRAME_EVENT_MATERIALTOOL, CEditorFrame::OnTool)
	EVT_MENU(MAIN_EVENT_MODELVIEWER, CEditorFrame::OnTool)

	EVT_TIMER(-1, CEditorFrame::OnTimer)

wxEND_EVENT_TABLE()

ConsoleVariable_t
	cvEditorAutoReload		= { "editor_ar", "1", true, false, "Enable or disable automatic reloading." },
	cvEditorAutoReloadDelay = { "editor_ar_delay", "5", true, false, "Delay before attempting to automatically reload content." },
	cvEditorShowProperties	= { "editor_showproperties", "1", true, false, "Can show/hide the properties." },
	cvEditorShowConsole		= { "editor_showconsole", "1", true, false, "Can show/hide the console." };

CEditorFrame::CEditorFrame(const wxString & title, const wxPoint & pos, const wxSize & size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	manager = new wxAuiManager(this);

	plWriteLog(EDITOR_LOG, "Setting frame icon...\n");
	SetIcon(wxIcon("resource/icon-engine.png", wxBITMAP_TYPE_PNG));

	// Display the splash screen...

	plWriteLog(EDITOR_LOG, "Creating splash screen\n");
	new wxSplashScreen(
		bSplashScreen,
		wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
		2000,
		NULL,
		wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		wxBORDER_SIMPLE | wxSTAY_ON_TOP | wxFRAME_NO_TASKBAR);

	SetSize(size);
	Maximize();

	// Set the menu up...

	wxMenu *mFile = new wxMenu;

	wxMenuItem *iNewItem = new wxMenuItem(mFile, wxID_NEW);
	iNewItem->SetBitmap(smallDocumentNew);
	mFile->Append(iNewItem);

	wxMenuItem *openMenuItem = new wxMenuItem(mFile, wxID_OPEN);
	openMenuItem->SetBitmap(smallDocumentOpen);
	mFile->Append(openMenuItem);

	mFile->AppendSeparator();

	wxMenuItem *miCloseDocument = new wxMenuItem(mFile, wxID_CLOSE);
	miCloseDocument->SetBitmap(smallDocumentClose);
	mFile->Append(miCloseDocument);

	mFile->AppendSeparator();

	wxMenuItem *saveDocumentMenuItem = new wxMenuItem(mFile, wxID_SAVE);
	saveDocumentMenuItem->SetBitmap(smallDocumentSave);
	mFile->Append(saveDocumentMenuItem);

	wxMenuItem *saveAsDocumentMenuItem = new wxMenuItem(mFile, wxID_SAVEAS);
	saveAsDocumentMenuItem->SetBitmap(smallDocumentSaveAs);
	mFile->Append(saveAsDocumentMenuItem);

	mFile->AppendSeparator();

	wxMenuItem *miExit = new wxMenuItem(mFile, wxID_EXIT);
	miExit->SetBitmap(smallApplicationExit);
	mFile->Append(miExit);

	wxMenu *mEdit = new wxMenu;
	mEdit->Append(wxID_UNDO);
	mEdit->Append(wxID_REDO);
	mEdit->AppendSeparator();
	wxMenuItem *miEditPreferences = new wxMenuItem(mEdit, wxID_PREFERENCES);
	miEditPreferences->SetBitmap(bSmallPrefIcon);
	mEdit->Append(miEditPreferences);

	wxMenu *mView = new wxMenu;
	viewWireframe = mView->AppendCheckItem(FRAME_EVENT_WIREFRAME, "&Wireframe");
	viewFlat = mView->AppendCheckItem(FRAME_EVENT_FLAT, "&Flat");
	viewTextured = mView->AppendCheckItem(FRAME_EVENT_TEXTURED, "Textured");
	viewLit = mView->AppendCheckItem(FRAME_EVENT_LIT, "&Lit");
	mView->AppendSeparator();
	mView->Append(FRAME_EVENT_RELOAD, "Reload materials");
	mView->AppendSeparator();
	mView->AppendCheckItem(wxID_ANY, "&Transform");
	mView->AppendCheckItem(wxID_ANY, "&Rotate");
	mView->AppendCheckItem(wxID_ANY, "&Scale");

	viewLit->Check(true);

	wxMenu *mTools = new wxMenu;
	{
		wxMenuItem *menutools_wad = new wxMenuItem(mTools, FRAME_EVENT_WADTOOL, "&WAD Tool...");
		menutools_wad->SetBitmap(bSmallWAD);
		mTools->Append(menutools_wad);

		mTools->AppendSeparator();

		wxMenuItem *menutools_mbrowser = new wxMenuItem(mTools, wxID_ANY, "Material Browser...");
		menutools_mbrowser->SetBitmap(bSmallMDL);
		mTools->Append(menutools_mbrowser);

		wxMenuItem *menutools_material = new wxMenuItem(mTools, FRAME_EVENT_MATERIALTOOL, "&Material Editor...");
		menutools_material->SetBitmap(bSmallMDL);
		mTools->Append(menutools_material);

		mTools->AppendSeparator();

		wxMenuItem *menutools_model = new wxMenuItem(mTools, MAIN_EVENT_MODELVIEWER, "Model &Viewer...");
		menutools_model->SetBitmap(bSmallMDL);
		mTools->Append(menutools_model);
	}

	wxMenu *mWindow = new wxMenu;
	windowShowConsole = mWindow->AppendCheckItem(FRAME_EVENT_SHOWCONSOLE, "&Console");
	windowShowProperties = mWindow->AppendCheckItem(ID_WINDOW_PROPERTIES, "&Properties");

	wxMenu *mHelp = new wxMenu;
	mHelp->Append(wxID_ABOUT);

	wxMenuBar *mbMainMenu = new wxMenuBar;
	mbMainMenu->Append(mFile, "&File");
	mbMainMenu->Append(mEdit, "&Edit");
	mbMainMenu->Append(mView, "&View");
	mbMainMenu->Append(mTools, "&Tools");
	mbMainMenu->Append(mWindow, "&Window");
	mbMainMenu->Append(mHelp, "&Help");
	SetMenuBar(mbMainMenu);

	CreateStatusBar(3);
	SetStatusText("Initialized");

	// Initialize the timer...
	timer = new wxTimer(this);

	// Create the toolbar...

	wxAuiPaneInfo toolbarInfo;
	toolbarInfo.Caption("Toolbar");
	toolbarInfo.ToolbarPane();
	toolbarInfo.Top();

	// File
	toolbarInfo.Position(0);
	wxAuiToolBar *tb_file = new wxAuiToolBar(this);
	tb_file->AddTool(wxID_NEW, "New material", smallDocumentNew);
	tb_file->AddTool(wxID_OPEN, "Open material", smallDocumentOpen, "Open an existing material");
	tb_file->AddSeparator();
	tb_file->AddTool(wxID_SAVE, "Save material", smallDocumentSave, "Save the current material");
	tb_file->Realize();
	manager->AddPane(tb_file, toolbarInfo);

	// Edit
	toolbarInfo.Position(1);
	wxAuiToolBar *tb_edit = new wxAuiToolBar(this);
	tb_edit->AddTool(wxID_UNDO, "Undo", iconDocumentUndo, "Undo changes");
	tb_edit->AddTool(wxID_REDO, "Redo", iconDocumentRedo, "Redo changes");
	tb_edit->Realize();
	manager->AddPane(tb_edit, toolbarInfo);

	// View
	wxAuiToolBar *tbView = new wxAuiToolBar(this);
//	tbView->AddTool(ID_WINDOW_PAUSE, "Pause", iconMediaPause, "Pause simulation");
//	tbView->AddTool(ID_WINDOW_PLAY, "Play", iconMediaPlay, "Play simulation");
	tbView->Realize();

	toolbarInfo.Position(2);
	manager->AddPane(tbView, toolbarInfo);

	Center();
	Show();

	// Create the engine viewport...
	viewport = new MainViewportPanel(this);
	viewport->Initialize();
	wxAuiPaneInfo viewportInfo;
	viewportInfo.Caption("Camera Viewport");
	viewportInfo.Center();
	viewportInfo.Movable(true);
	viewportInfo.Floatable(true);
	viewportInfo.Dockable(true);
	viewportInfo.MaximizeButton(true);
	viewportInfo.CloseButton(false);
	viewportInfo.Gripper(false);
	manager->AddPane(viewport, viewportInfo);

	XViewportPanel *viewport_top = new XViewportPanel(this);
	viewport_top->Initialize();
	viewportInfo.Caption("Top Viewport");
//	viewportInfo.Top();
	viewportInfo.Right();
	viewport_top->StartDrawing();
	manager->AddPane(viewport_top, viewportInfo);

	viewport_top = new XViewportPanel(this);
	viewport_top->Initialize();
	viewportInfo.Caption("Side Viewport");
//	viewportInfo.Bottom();
//	viewportInfo.Left();
	viewport_top->StartDrawing();
	manager->AddPane(viewport_top, viewportInfo);

	// Create the console...
	pConsole = new CEditorConsolePanel(this);
	wxAuiPaneInfo consoleInfo;
	consoleInfo.Caption("Console");
	consoleInfo.Bottom();
	consoleInfo.Movable(true);
	consoleInfo.Floatable(true);
	consoleInfo.MaximizeButton(true);
	consoleInfo.CloseButton(false);
	manager->AddPane(pConsole, consoleInfo);

	manager->Update();

	preferences = new CPreferencesDialog(this);

	frame_model = new ModelFrame(this);

	dAutoReloadDelay = 0;
	dClientTime = 0;
}

CEditorFrame::~CEditorFrame()
{
	// Stop the engine from looping.
	StopEngineLoop();

	// Uninitialize the AUI manager.
	manager->UnInit();
}

void CEditorFrame::Initialize()
{
	// TODO: These need to be able to update the editor, when modified.
	engine->RegisterConsoleVariable(&cvEditorShowConsole, NULL);
	engine->RegisterConsoleVariable(&cvEditorShowProperties, NULL);
	engine->RegisterConsoleVariable(&cvEditorAutoReload, NULL);
	engine->RegisterConsoleVariable(&cvEditorAutoReloadDelay, NULL);

	viewport->StartDrawing();

	dAutoReloadDelay = cvEditorAutoReloadDelay.value;
}

void CEditorFrame::OpenWADTool(wxString sPath)
{
	if (!tool_wad)
		tool_wad = new CWADFrame(this);

	tool_wad->Show();
}

void CEditorFrame::OpenMaterial(wxString path)
{
	if (!tool_material)
		tool_material = new CMaterialFrame(this);

	tool_material->Show();
	if (!path.IsEmpty())
		tool_material->LoadMaterial(path);
}

void CEditorFrame::StartEngineLoop()
{
	// TODO: This isn't working...
	if (!cvEditorShowConsole.bValue)
	{
		manager->GetPane(pConsole).Show(false);
		manager->Update();
	}
	else
		windowShowConsole->Check(true);

	timer->Start(25);
}

void CEditorFrame::StopEngineLoop()
{
	timer->Stop();
}

void CEditorFrame::OnPause(wxCommandEvent &event)
{
	StopEngineLoop();
}

void CEditorFrame::OnPlay(wxCommandEvent &event)
{
	timer->Start(25);
}

void CEditorFrame::OnTimer(wxTimerEvent &event)
{
	if (!engine->IsRunning())
		return;

	// Perform the main loop.
	engine->Loop();

	// Keep the client-time updated.
	dClientTime = engine->GetClientTime();

	// Check to see if it's time to check for changes.
	if (dAutoReloadDelay < dClientTime)
	{
		if (tool_material && tool_material->IsActive())
			tool_material->ReloadMaterial();

		dAutoReloadDelay = dClientTime + cvEditorAutoReloadDelay.value;
	}
}

void CEditorFrame::OnView(wxCommandEvent &event)
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

void CEditorFrame::OnTool(wxCommandEvent &event)
{
	switch (event.GetId())
	{
	case FRAME_EVENT_WADTOOL:
		OpenWADTool("");
		break;
	case FRAME_EVENT_MATERIALTOOL:
		OpenMaterial("");
		break;
	case MAIN_EVENT_MODELVIEWER:
		frame_model->Show();
		break;
	}
}

void CEditorFrame::OnReload(wxCommandEvent &event)
{
	ReloadCurrentDocument();
}

void CEditorFrame::OnOpen(wxCommandEvent &event)
{
	wxFileDialog *filedialog = new wxFileDialog(
		this,
		"Open File",
		engine->GetBasePath(),
		"",

		"Supported files (*.material;*.map;*.level;*.md2;*.wad)|"
		"*.material;*.level;*.md2;*.wad|"

		"Material files (*.material)|"
		"*.material|"

		"Model files (*.md2;*.u3d;*.obj)|"
		"*.md2;*.u3d;*.obj|"

		"Level files (*.level;*.map)|"
		"*.level;*.map|"
		
		"WAD files (*.wad)|"
		"*.wad",

		wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (filedialog->ShowModal() == wxID_OK)
	{
		wxString filename = filedialog->GetFilename();
		if (filename.EndsWith(".material"))
			OpenMaterial(filedialog->GetPath());
		else if (filename.EndsWith(".map"))
		{
			SetTitle(filedialog->GetFilename() + wxString(" - ") + g_apptitle);
		}
		else if (filename.EndsWith(".level"))
		{
			// TODO: Load the level up in a "viewer" mode.
			SetTitle(filedialog->GetFilename() + wxString(" - ") + g_apptitle);
		}
		else if (filename.EndsWith(".wad"))
		{
			OpenWADTool(filedialog->GetPath());
		}
		else if (filename.EndsWith(".md2"))
		{
		}

		currentFilePath = filedialog->GetPath();
		lastTimeModified = currentTimeModified = plGetFileModifiedTime(currentFilePath);
	}
}

void CEditorFrame::OnExit(wxCommandEvent &event)
{
	// Stop rendering!
	StopEngineLoop();

	engine->Shutdown();

	// Close the frame and app.
	Close(true);
}

void CEditorFrame::OnAbout(wxCommandEvent &event)
{
	const char ccLicense[]=
	{
		"Copyright (C) 1996-2001 Id Software, Inc.\n"
		"Copyright (C) 2002-2009 John Fitzgibbons and others\n"
		"Copyright (C) 2011-2016 OldTimes Software\n"
		"\n"
		"This program is free software; you can redistribute it and / or\n"
		"modify it under the terms of the GNU General Public License\n"
		"as published by the Free Software Foundation; either version 2\n"
		"of the License, or(at your option) any later version.\n"
		"\n"
		"This program is distributed in the hope that it will be useful,\n"
		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
		"\n"
		"See the GNU General Public License for more details.\n"
		"\n"
		"You should have received a copy of the GNU General Public License\n"
		"along with this program; if not, write to the Free Software\n"
		"Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111 - 1307, USA."
	};

	wxAboutDialogInfo info;
	info.AddDeveloper("Mark \"hogsy\" Sowden");
	info.SetName(EDITOR_TITLE);
	info.SetCopyright("Copyright (C) 2011-2016 OldTimes Software");
	info.SetDescription("Editor for the Katana Engine.");
	info.SetWebSite("www.oldtimes-software.com");
	info.SetIcon(wxIcon(PATH_RESOURCES"icon-engine.png", wxBITMAP_TYPE_PNG_RESOURCE, 64, 64));
	info.SetVersion(EDITOR_VERSION);
	info.SetLicense(ccLicense);

	wxAboutBox(info, this);
}

void CEditorFrame::OnSave(wxCommandEvent &event)
{}

void CEditorFrame::OnConsole(wxCommandEvent &event)
{
	if (manager->GetPane(pConsole).IsShown())
	{
		manager->GetPane(pConsole).Show(false);
		engine->SetConsoleVariable(cvEditorShowConsole.name, "0");
	}
	else
	{
		manager->GetPane(pConsole).Show(true);
		engine->SetConsoleVariable(cvEditorShowConsole.name, "1");
	}

	manager->Update();
}

void CEditorFrame::OnPreferences(wxCommandEvent &event)
{
	preferences->Show();
}

void CEditorFrame::OnProperties(wxCommandEvent &event)
{
#if 0
	propertyWindow->Show(!propertyWindow->IsShown());
	propertyWindow->SetPosition(wxPoint(GetPosition().x - 256, GetPosition().y));
#endif
}

/*	Reload the currently active document.
*/
void CEditorFrame::ReloadCurrentDocument()
{
#if 0
	Material_t *current = editorMaterialProperties->GetCurrent();
	if (!current)
		return;

	// Ensure things have actually changed.
	currentTimeModified = plGetFileModifiedTime(currentFilePath);
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
#endif
}

void CEditorFrame::PrintMessage(char *text)
{
	if (!pConsole)
		return;

	pConsole->textConsoleOut->SetDefaultStyle(wxTextAttr(*wxGREEN));
	pConsole->textConsoleOut->AppendText(text);
}

void CEditorFrame::PrintWarning(char *text)
{
	if (!pConsole)
		return;

	pConsole->textConsoleOut->SetDefaultStyle(wxTextAttr(*wxYELLOW));
	pConsole->textConsoleOut->AppendText(text);
}

void CEditorFrame::PrintError(char *text)
{
	if (!pConsole)
		return;

	pConsole->textConsoleOut->SetDefaultStyle(wxTextAttr(*wxRED));
	pConsole->textConsoleOut->AppendText(text);
}
