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
#include "platform_module.h"
#include "platform_filesystem.h"

#define	EDITOR_TITLE	"Katana World Editor"
#define	EDITOR_LOG		"editor"
#define	EDITOR_VERSION	"v0.1"

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
	cvEditorAutoReload,
	cvEditorAutoReloadDelay,
	cvEditorShowProperties,
	cvEditorShowConsole;

#include "base_rendercanvas.h"
#include "base_viewportpanel.h"

#endif // !EDITOR_BASE_H
