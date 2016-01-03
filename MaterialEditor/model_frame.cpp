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

#include "EditorBase.h"

#include "model_frame.h"

/*
	Viewport
*/

void ModelViewportPanel::Draw()
{
	engine->DrawGradientBackground();

	if (entity)
	{
		engine->DrawEntity(entity);
		// Dolly rotate
		if (rotate)
			entity->angles[1] += 0.5f;
	}
}

/*
	Frame
*/

wxBEGIN_EVENT_TABLE(ModelFrame, wxFrame)

EVT_MENU(wxID_EXIT, ModelFrame::FileEvent)

EVT_CLOSE(ModelFrame::CloseEvent)

wxEND_EVENT_TABLE()

#define	MODEL_TITLE	"Model Viewer"

ModelFrame::ModelFrame(wxWindow *parent)
	: wxFrame(parent, EDITOR_WINDOW_MODEL, MODEL_TITLE, wxDefaultPosition, wxSize(640, 480))
{
	SetIcon(wxIcon("resource/icon-mdl.png", wxBITMAP_TYPE_PNG));

	// Setup the menu...
	wxMenuBar *menubar = new wxMenuBar;
	{
		wxMenu *menu_file = new wxMenu;

		wxMenuItem *menu_file_open = new wxMenuItem(menu_file, wxID_OPEN);
		menu_file->Append(menu_file_open);

		wxMenuItem *menu_file_close = new wxMenuItem(menu_file, wxID_CLOSE);
		menu_file->Append(menu_file_close);

		wxMenuItem *menu_file_exit = new wxMenuItem(menu_file, wxID_EXIT);
		menu_file->Append(menu_file_exit);

		menubar->Append(menu_file, "&File");
	}
	SetMenuBar(menubar);

	viewport = new ModelViewportPanel(this);
	viewport->Initialize();
	viewport->StartDrawing();
}

void ModelFrame::LoadModel(const char *path)
{

}

void ModelFrame::FileEvent(wxCommandEvent &event)
{
	switch (event.GetId())
	{
	case wxID_CLOSE_FRAME:
	case wxID_EXIT:
		Show(false);
		break;
	}
}

void ModelFrame::CloseEvent(wxCloseEvent &event)
{
	Show(false);
}