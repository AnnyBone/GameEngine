/*	Copyright (C) 2011-2015 OldTimes Software

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

#ifndef __EDITORBASE__
#define	__EDITORBASE__

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
#include "SharedFlags.h"

// Platform Library
#include "platform.h"
#include "platform_math.h"
#include "platform_module.h"
#include "platform_filesystem.h"

extern char cApplicationTitle[512];

#define	EDITOR_TITLE	"Editor"
#define	EDITOR_LOG		"editor"

#define	PATH_16ICONS	"resource/16x16/"
#define	PATH_32ICONS	"resource/32x32/"

extern "C" {
#include "SharedEngine.h"
}

void EngineInterface_Load();
void EngineInterface_Unload();

extern EngineExport_t *engine;
extern EngineImport_t editor;

enum
{
	// Tools
	EDITOR_ID_MATERIAL_TOOL,
	EDITOR_ID_WAD_TOOL,

	// Windows
	EDITOR_ID_MAIN,
	EDITOR_ID_PREFERENCES
};

extern wxString
	sEditorBasePath,
	sEditorMaterialPath;

// Icons
extern wxBitmap
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
extern wxBitmap bSmallPlaneIcon;
extern wxBitmap bSmallPrefIcon;

// Console Variables
extern ConsoleVariable_t
	cvEditorAutoReload,
	cvEditorAutoReloadDelay,
	cvEditorShowProperties,
	cvEditorShowConsole;

#include "EditorRenderCanvas.h"
#include "EditorViewportPanel.h"

#endif // !__EDITORBASE__