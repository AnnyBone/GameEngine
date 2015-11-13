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

class CEditorViewportPanel :
	public wxPanel
{
public:
	CEditorViewportPanel(wxWindow *wParent);
	~CEditorViewportPanel();

	virtual void Draw() {}

	int GetCanvasWidth()
	{
		return viewport_canvas->GetWidth();
	}

	int GetCanvasHeight()
	{
		return viewport_canvas->GetHeight();
	}

private:

	// Events
	void OnTimer(wxTimerEvent &event);

	EditorDrawCanvas *viewport_canvas;

	wxBitmapToggleButton *bAutoToggle;
	wxTimer *tRenderTimer;

	wxDECLARE_EVENT_TABLE();
};

#endif	// VIEWPORT_PANEL_H

