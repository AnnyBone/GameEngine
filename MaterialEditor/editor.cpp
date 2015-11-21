#include "EditorBase.h"

#include "editor.h"

wxIMPLEMENT_APP(CEditorApp);

char cApplicationTitle[512];

wxString
	sEditorBasePath, 
	sEditorMaterialPath;

wxBitmap
	bSmallPrefIcon,
	bSmallPlaneIcon,
	bSplashScreen,
	bSmallAuto,
	bSmallWAD,
	bSmallMDL,
	iconScriptEdit,
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
	pLog_Clear(EDITOR_LOG);
	pLog_Write(EDITOR_LOG, "Initializing engine interface...\n");

	EngineInterface_Load();

	sprintf(cApplicationTitle, EDITOR_TITLE" ("EDITOR_VERSION")");

	wxImage::AddHandler(new wxPNGHandler);
	wxImage::AddHandler(new wxGIFHandler);
	bSplashScreen.LoadFile("resource/splash-editor.png", wxBITMAP_TYPE_PNG);
	bSmallMDL.LoadFile("resource/icon-mdl-small.gif", wxBITMAP_TYPE_GIF);
	bSmallAuto.LoadFile("resource/view-auto.png", wxBITMAP_TYPE_PNG);
	bSmallWAD.LoadFile("resource/icon-wad-small.gif", wxBITMAP_TYPE_GIF);
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
	iconScriptEdit.LoadFile(PATH_16ICONS"apps/accessories-text-editor.png", wxBITMAP_TYPE_PNG);
	smallTransform.LoadFile(PATH_16ICONS"actions/transform-move.png", wxBITMAP_TYPE_PNG);
	smallApplicationExit.LoadFile(PATH_16ICONS"actions/application-exit.png", wxBITMAP_TYPE_PNG);

	pLog_Write(EDITOR_LOG, "Creating main frame...\n");

	efMainFrame = new CEditorFrame(cApplicationTitle, wxPoint(50, 50), wxSize(1024, 768));
	if (!efMainFrame)
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

	sEditorBasePath = engine->GetBasePath();
	sEditorMaterialPath = engine->GetMaterialPath();

	efMainFrame->Initialize();

	pLog_Write(EDITOR_LOG, "Starting main loop...\n");

	// Start rendering.
	efMainFrame->StartEngineLoop();

	return true;
}

int CEditorApp::OnExit()
{
	pLog_Write(EDITOR_LOG, "Unloading engine interface...\n");
	EngineInterface_Unload();
	pLog_Write(EDITOR_LOG, "Done!\n");
	return 0;
}
