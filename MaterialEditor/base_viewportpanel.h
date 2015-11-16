/*	Copyright (C) 2011-2015 OldTimes Software

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef VIEWPORT_PANEL_H
#define	VIEWPORT_PANEL_H
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

private:

	// Events
	void OnTimer(wxTimerEvent &event);

	BaseDrawCanvas *viewport_canvas;

	wxBitmapToggleButton	*bAutoToggle;
	wxTimer					*timer;

	wxDECLARE_EVENT_TABLE();
};

#endif	// VIEWPORT_PANEL_H

