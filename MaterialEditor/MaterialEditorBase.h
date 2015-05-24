#ifndef __MATERIALEDITORBASE__
#define	__MATERIALEDITORBASE__

#define	wxUSE_GUI 1

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#	include <wx/wx.h>
#endif

#include <wx/glcanvas.h>
#include <wx/wfstream.h>
#include <wx/minifram.h>

#include "shared_flags.h"

#include "platform.h"
#include "platform_math.h"
#include "platform_module.h"

#define	MATERIALEDITOR_TITLE "Katana Material Editor"

extern "C" {
#include "shared_module.h"
#include "shared_engine.h"
}

void EngineInterface_Load();
void EngineInterface_Unload();

bool EngineInterface_LoadEditorMaterial(char *cPath);

extern EngineExport_t *engine;
extern EngineImport_t *editor;

#endif // !__MATERIALEDITORBASE__