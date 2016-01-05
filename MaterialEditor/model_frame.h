/*	Copyright (C) 2011-2016 OldTimes Software

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

#ifndef MODEL_FRAME_H
#define	MODEL_FRAME_H

class ModelViewportPanel : public BaseViewportPanel
{
public:

	ModelViewportPanel(wxWindow *parent) : BaseViewportPanel(parent) {}

	void Draw();
	void SetModel(model_t *newmodel);
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

	// Events
	void FileEvent(wxCommandEvent &event);
	void ViewEvent(wxCommandEvent &event);
	void CloseEvent(wxCloseEvent &event);

	wxDECLARE_EVENT_TABLE();
};

#endif // !MODEL_FRAME_H