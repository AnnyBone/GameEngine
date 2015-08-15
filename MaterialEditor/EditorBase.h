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

// Icons
extern wxBitmap
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
	iconShapeSphere,
	iconShapePlane;

#endif // !__EDITORBASE__
