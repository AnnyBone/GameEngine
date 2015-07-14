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

// Shared Library
#include "SharedFlags.h"

// Platform Library
#include "platform.h"
#include "platform_math.h"
#include "platform_module.h"

#define	MATERIALEDITOR_TITLE	"Material Editor"
#define	MATERIALEDITOR_LOG		"editor"

#define	PATH_SILK	"resource/silk/"
#define	PATH_SILKA	"resource/silka/"

extern "C" {
#include "SharedEngine.h"
}

void EngineInterface_Load();
void EngineInterface_Unload();

extern EngineExport_t *engine;
extern EngineImport_t editor;

#endif // !__MATERIALEDITORBASE__