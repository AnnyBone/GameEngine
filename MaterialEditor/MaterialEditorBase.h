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

#define	MATERIALEDITOR_TITLE "Katana Material Editor"

bool EngineInterface_Load();

void EngineInterface_Unload();
void EngineInterface_Initialize();
void EngineInterface_Loop();
void EngineInterface_SetViewportSize(int width, int height);
void EngineInterface_MaterialEditorInitialize(void);

bool EngineInterface_LoadEditorMaterial(char *cPath);

char *EngineInterface_GetBasePath();
char *EngineInterface_GetMaterialPath();

#endif // !__MATERIALEDITORBASE__