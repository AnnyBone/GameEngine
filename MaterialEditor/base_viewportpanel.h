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

class BaseViewportPanel :
	public wxPanel
{
public:
	BaseViewportPanel(wxWindow *wParent, const wxSize &size = wxSize(512, 512));
	~BaseViewportPanel();

	virtual void Initialize();
	virtual void Draw() {}

	void StartDrawing();
	void StopDrawing();

	// Draw Canvas
	BaseDrawCanvas *GetDrawCanvas() { return viewport_canvas; }
	int GetDrawCanvasWidth()		{ return viewport_canvas->GetWidth(); }
	int GetDrawCanvasHeight()		{ return viewport_canvas->GetHeight(); }
	void CreateDrawCanvas(wxWindow *parent);

	virtual void OnKey(wxKeyEvent &event) { engine->Print("Unhandled input for viewport!\n"); }

private:

	// Events
	void OnTimer(wxTimerEvent &event);

	BaseDrawCanvas *viewport_canvas;

	wxBitmapToggleButton	*bAutoToggle;
	wxTimer					*timer;

	wxDECLARE_EVENT_TABLE();
};

