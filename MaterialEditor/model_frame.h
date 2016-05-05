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

#pragma once

class ModelViewportPanel : public BaseViewportPanel
{
public:

	ModelViewportPanel(wxWindow *parent);

	void Draw();
	void SetModel(model_t *newmodel);
	void SetRotate(bool dorotate);

	virtual void OnKey(wxKeyEvent &event);
protected:
private:

	bool rotate;

	DynamicLight_t	*light;
	ClientEntity_t	*entity;

	void ViewEvent(wxCommandEvent &event);
};

class ModelFrame : public wxFrame
{
public:

	ModelFrame(wxWindow *parent);

	void LoadModel(wxString path);

protected:
private:

	ModelViewportPanel *viewport;

	wxMenuItem *v_showbboxes;
	wxMenuItem *v_rotate;

	// Events
	void FileEvent(wxCommandEvent &event);
	void ViewEvent(wxCommandEvent &event);
	void CloseEvent(wxCloseEvent &event);

	wxDECLARE_EVENT_TABLE();
};