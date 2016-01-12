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

ModelViewportPanel::ModelViewportPanel(wxWindow *parent) : BaseViewportPanel(parent)
{
	entity = engine->CreateClientEntity();
	if (entity)
	{
		entity->alpha = 255;
		entity->origin[0] = 50.0f;
		entity->origin[1] = 0;
		entity->origin[2] = 0;
		entity->model = NULL;
	}
	else
		plWriteLog(EDITOR_LOG, "Failed to create client entity!\n");
}

void ModelViewportPanel::Draw()
{
	engine->DrawGradientBackground();

	if (entity && entity->model)
	{
		entity->frame++;
		if (entity->frame >= entity->model->numframes)
			entity->frame = 0;

		engine->DrawEntity(entity);
		// Dolly rotate
		//if (rotate)
			entity->angles[1] += 0.5f;
	}
}

void ModelViewportPanel::SetModel(model_t *newmodel)
{
	if (entity->model)
	{
		// TODO: unload it.
	}
	entity->model = newmodel;
}

void ModelViewportPanel::SetRotate(bool dorotate)
{
	if ((rotate == true) && (dorotate == false))
		// If we're setting it to false, reset the models current angle.
		entity->angles[1] = 0;

	rotate = dorotate;
}

/*
	Frame
*/

enum
{
	MODELFRAME_EVENT_ROTATE,
};

wxBEGIN_EVENT_TABLE(ModelFrame, wxFrame)

EVT_MENU(wxID_OPEN, ModelFrame::FileEvent)
EVT_MENU(wxID_EXIT, ModelFrame::FileEvent)

EVT_MENU(MODELFRAME_EVENT_ROTATE, ModelFrame::ViewEvent)

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
		// File
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
		
		// View
		{
			wxMenu *menu_view = new wxMenu;

			wxMenuItem *menu_view_rotate = new wxMenuItem(menu_view, MODELFRAME_EVENT_ROTATE, "&Rotate", "Dolly rotates the current preview");
			menu_view->Append(menu_view_rotate);

			menubar->Append(menu_view, "&View");
		}
	}
	SetMenuBar(menubar);

	viewport = new ModelViewportPanel(this);
	viewport->Initialize();
	viewport->StartDrawing();
}

void ModelFrame::LoadModel(wxString path)
{
	wxString newpath = path;

	int stringmod = newpath.Find(wxString("models/").RemoveLast(1));
	if (stringmod == wxNOT_FOUND)
	{
		wxMessageBox(wxString("Failed to update path! (" + newpath + ")\nPlease ensure your model is inside the game directory."), MODEL_TITLE);
		return;
	}
	newpath.Remove(0, stringmod);

	model_t *model = engine->LoadModel(newpath);
	if (!model)
		return;

	viewport->SetModel(model);

	SetTitle(newpath + " - " MODEL_TITLE);
}

void ModelFrame::FileEvent(wxCommandEvent &event)
{
	switch (event.GetId())
	{
	case wxID_OPEN:
		{
			wxFileDialog *filed = new wxFileDialog(
				this,
				"Open Model",
				wxString(engine->GetBasePath()) + "/models",
				"",
				"Supported files (*.3d;*.md2)|*.3d;*.md2",
				wxFD_OPEN | wxFD_FILE_MUST_EXIST);
			if (filed->ShowModal() == wxID_OK)
				LoadModel(filed->GetPath());
		}
		break;
	case wxID_EXIT:
		Show(false);
		break;
	}
}

void ModelFrame::ViewEvent(wxCommandEvent &event)
{
	switch (event.GetId())
	{
	case MODELFRAME_EVENT_ROTATE:
		viewport->SetRotate(false);
		break;
	}
}

void ModelFrame::CloseEvent(wxCloseEvent &event)
{
	Show(false);
}