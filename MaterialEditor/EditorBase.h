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

#ifndef EDITOR_BASE_H
#define	EDITOR_BASE_H

#define	wxUSE_GUI 1

// wxWidgets Library
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#	include <wx/wx.h>
#endif
#include <wx/glcanvas.h>
#include <wx/wfstream.h>
#include <wx/minifram.h>
#include <wx/propgrid/propgrid.h>
#include <wx/aui/framemanager.h>
#include <wx/aui/auibar.h>
#include <wx/splash.h>
#include <wx/splitter.h>
#include <wx/aboutdlg.h>
#include <wx/listctrl.h>
#include <wx/bmpcbox.h>
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/panel.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stc/stc.h>
#include <wx/sizer.h>
#include <wx/frame.h>
#include <wx/dialog.h>
#include <wx/tglbtn.h>
#include <wx/notebook.h>
#include <wx/spinctrl.h>

// Shared Library
#include "shared_flags.h"

// Platform Library
#include "platform.h"
#include "platform_library.h"
#include "platform_filesystem.h"

#define	EDITOR_TITLE	"Katana World Editor"
#define	EDITOR_LOG		"editor"
#define	EDITOR_VERSION	"V0.2"

#define	PATH_16ICONS	"resource/16x16/"
#define	PATH_32ICONS	"resource/32x32/"

extern "C" {
#include "shared_engine.h"
}

void EngineInterface_Load();
void EngineInterface_Unload();

extern EngineExport_t *engine;
extern EngineImport_t editor;

enum
{
	EDITOR_WINDOW_MAIN,				// Main
	EDITOR_WINDOW_PREFERENCES,		// Preferences
	EDITOR_WINDOW_MATERIAL,			// Material Tool
	EDITOR_WINDOW_MATERIALBROWSER,	// Material Browser
	EDITOR_WINDOW_WAD,				// WAD Tool
	EDITOR_WINDOW_MODEL				// Model Viewer
};

extern wxString	g_apptitle, sEditorMaterialPath;

// Icons
extern wxBitmap
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
extern wxBitmap bSmallPlaneIcon;
extern wxBitmap bSmallPrefIcon;

// Console Variables
extern ConsoleVariable_t
	cv_editor_autoreload,
	cv_editor_autoreload_delay,
	cv_editor_showproperties,
	cv_editor_showconsole;

#include "base_rendercanvas.h"
#include "base_viewportpanel.h"

#endif // !EDITOR_BASE_H
