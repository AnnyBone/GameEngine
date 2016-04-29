/*	DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
	Version 2, December 2004

	Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

	Everyone is permitted to copy and distribute verbatim or modified
	copies of this license document, and changing it is allowed as long
	as the name is changed.

	DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
	TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

	0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#include "EditorBase.h"

#include "editor.h"

wxIMPLEMENT_APP(CEditorApp);

wxString sEditorMaterialPath, g_apptitle;

wxBitmap
	bSmallPrefIcon,
	bSmallPlaneIcon,
	bSplashScreen,
	bSmallAuto,
	bSmallWAD,
	bSmallMDL,
	smallTransform,
	smallDocumentOpen,
	iconMediaPause,
	iconMediaPlay,
	iconDocumentRefresh,
	smallDocumentNew,
	smallDocumentSave,
	smallDocumentSaveAs,
	iconDocumentUndo,
	iconDocumentRedo,
	smallDocumentClose,
	iconShapeCube,
	smallApplicationExit,
	iconShapeSphere;

bool CEditorApp::OnInit()
{
	plClearLog(EDITOR_LOG);
	plWriteLog(EDITOR_LOG, "Initializing engine interface...\n");

	EngineInterface_Load();

	g_apptitle = EDITOR_TITLE " (" EDITOR_VERSION ")";

	wxImage::AddHandler(new wxPNGHandler);
	wxImage::AddHandler(new wxGIFHandler);
	wxImage::AddHandler(new wxICOHandler);
	bSplashScreen.LoadFile("resource/splash-editor.png", wxBITMAP_TYPE_PNG);
	bSmallMDL.LoadFile("resource/icon-mdl-small.png", wxBITMAP_TYPE_PNG);
	bSmallAuto.LoadFile("resource/view-auto.png", wxBITMAP_TYPE_PNG);
	bSmallWAD.LoadFile("resource/icon-wad-small.png", wxBITMAP_TYPE_PNG);
	bSmallPrefIcon.LoadFile(PATH_16ICONS"actions/configure.png", wxBITMAP_TYPE_PNG);
	smallDocumentNew.LoadFile(PATH_16ICONS"actions/document-new.png", wxBITMAP_TYPE_PNG);
	smallDocumentOpen.LoadFile(PATH_16ICONS"actions/document-open.png", wxBITMAP_TYPE_PNG);
	smallDocumentSave.LoadFile(PATH_16ICONS"actions/document-save.png", wxBITMAP_TYPE_PNG);
	smallDocumentClose.LoadFile(PATH_16ICONS"actions/document-close.png", wxBITMAP_TYPE_PNG);
	smallDocumentSaveAs.LoadFile(PATH_16ICONS"actions/document-save-as.png", wxBITMAP_TYPE_PNG);
	iconDocumentUndo.LoadFile(PATH_16ICONS"actions/edit-undo.png", wxBITMAP_TYPE_PNG);
	iconDocumentRedo.LoadFile(PATH_16ICONS"actions/edit-redo.png", wxBITMAP_TYPE_PNG);
	iconMediaPause.LoadFile(PATH_16ICONS"actions/media-playback-pause.png", wxBITMAP_TYPE_PNG);
	iconMediaPlay.LoadFile(PATH_16ICONS"actions/media-playback-start.png", wxBITMAP_TYPE_PNG);
	iconDocumentRefresh.LoadFile(PATH_16ICONS"actions/view-refresh.png", wxBITMAP_TYPE_PNG);
	iconShapeCube.LoadFile("resource/shape-cube.png", wxBITMAP_TYPE_PNG);
	iconShapeSphere.LoadFile("resource/shape-sphere.png", wxBITMAP_TYPE_PNG);
	bSmallPlaneIcon.LoadFile("resource/shape-plane.png", wxBITMAP_TYPE_PNG);
	smallTransform.LoadFile(PATH_16ICONS"actions/transform-move.png", wxBITMAP_TYPE_PNG);
	smallApplicationExit.LoadFile(PATH_16ICONS"actions/application-exit.png", wxBITMAP_TYPE_PNG);

	plWriteLog(EDITOR_LOG, "Creating main frame...\n");

	editor_frame = new CEditorFrame(g_apptitle, wxPoint(50, 50), wxSize(1024, 768));
	if (!editor_frame)
	{
		EngineInterface_Unload();

		wxMessageBox("Failed to create main frame!", EDITOR_TITLE" Error");
		wxExit();
	}

	if (!engine->Initialize(argc, argv, true))
	{
		EngineInterface_Unload();

		wxMessageBox("Failed to initialize engine!", EDITOR_TITLE" Error");
		wxExit();
	}

	engine->MaterialEditorInitialize();

	sEditorMaterialPath = engine->GetMaterialPath();

	editor_frame->Initialize();

	plWriteLog(EDITOR_LOG, "Starting main loop...\n");

	// Start rendering.
	editor_frame->StartEngineLoop();

	return true;
}

int CEditorApp::OnExit()
{
	plWriteLog(EDITOR_LOG, "Unloading engine interface...\n");
	EngineInterface_Unload();
	plWriteLog(EDITOR_LOG, "Done!\n");
	return 0;
}
