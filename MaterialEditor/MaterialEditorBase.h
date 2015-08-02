#ifndef __MATERIALEDITORBASE__
#define	__MATERIALEDITORBASE__

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

// Shared Library
#include "SharedFlags.h"

// Platform Library
#include "platform.h"
#include "platform_math.h"
#include "platform_module.h"
#include "platform_filesystem.h"

extern char cApplicationTitle[512];

#define	EDITOR_TITLE	"Yokote"

#define	MATERIALEDITOR_LOG	"editor"

#define	PATH_16ICONS	"resource/16x16/"
#define	PATH_32ICONS	"resource/32x32/"

extern "C" {
#include "SharedEngine.h"
}

void EngineInterface_Load();
void EngineInterface_Unload();

extern EngineExport_t *engine;
extern EngineImport_t editor;

#endif // !__MATERIALEDITORBASE__