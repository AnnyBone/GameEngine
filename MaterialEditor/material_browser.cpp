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

#include "EditorBase.h"

#include "base_rendercanvas.h"
#include "base_viewportpanel.h"

#define	MATERIALBROWSER_TITLE "Material Browser"

class MaterialBrowserFrame : public wxFrame
{
public:
	MaterialBrowserFrame(wxWindow *parent);

	void Initialize();

	virtual bool Show(bool show = true);

protected:
private:
};

MaterialBrowserFrame::MaterialBrowserFrame(wxWindow *parent)
	: wxFrame(
	parent,
	EDITOR_WINDOW_MATERIALBROWSER,
	MATERIALBROWSER_TITLE,
	wxDefaultPosition,
	wxSize(640, 640))
{}

void MaterialBrowserFrame::Initialize()
{
	SetIcon(wxIcon("resource/icon-material.png", wxBITMAP_TYPE_PNG));
}

bool MaterialBrowserFrame::Show(bool show)
{
	Centre();

	return wxFrame::Show(show);
}