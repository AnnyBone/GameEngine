/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
Version 2, December 2004

Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#pragma once

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
#include <wx-3.0/wx/aui/framemanager.h>
#include <wx-3.0/wx/aui/auibar.h>
#include <wx-3.0/wx/splash.h>
#include <wx-3.0/wx/splitter.h>
#include <wx-3.0/wx/aboutdlg.h>
#include <wx-3.0/wx/listctrl.h>
#include <wx-3.0/wx/bmpcbox.h>
#include <wx-3.0/wx/artprov.h>
#include <wx-3.0/wx/xrc/xmlres.h>
#include <wx-3.0/wx/panel.h>
#include <wx-3.0/wx/gdicmn.h>
#include <wx-3.0/wx/font.h>
#include <wx-3.0/wx/colour.h>
#include <wx-3.0/wx/settings.h>
#include <wx-3.0/wx/string.h>
#include <wx-3.0/wx/stc/stc.h>
#include <wx/sizer.h>
#include <wx/frame.h>
#include <wx/dialog.h>
#include <wx/tglbtn.h>
#include <wx/notebook.h>
#include <wx/spinctrl.h>

#include "platform_log.h"
#include "platform_window.h"

#include "shared_engine.h"

#define EDITOR_TITLE	"Xenon World Editor"
#define EDITOR_LOG		"editor"

class EditorFrame : public wxFrame {
 public:
  EditorFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
  ~EditorFrame() {};

 protected:

 private:

 wxDECLARE_EVENT_TABLE();
};

class EditorApp : public wxApp {
 public:
  virtual bool OnInit();
  virtual int OnExit();

  EditorFrame *frame;
};

wxDECLARE_APP(EditorApp);