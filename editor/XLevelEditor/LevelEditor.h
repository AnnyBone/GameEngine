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

// wxWidgets Library
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#	include <wx/wx.h>
#endif

// Platform Library
#include "platform.h"
#include "platform_log.h"

// Shared Library
#include "shared_base.h"

#define XLEVELEDITOR_TITLE	"Xenon Level Editor"
#define XLEVELEDITOR_LOG	"editor"

class XLevelEditorFrame;

extern ConsoleVariable_t cv_editor_maximize;

class XLevelEditor : public wxApp
{
public:
	virtual bool OnInit();

	XLevelEditorFrame *GetPrimaryFrame() const { return _main_frame; }

protected:
private:
	XLevelEditorFrame *_main_frame;
};

class XLevelEditorFrame : public wxFrame
{
public:
	XLevelEditorFrame(const wxPoint &pos, const wxSize &size);

protected:
private:
	wxDECLARE_EVENT_TABLE();
};

wxDECLARE_APP(XLevelEditor);