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

#include "EditorBase.h"

#include "model_frame.h"

/*
	Viewport
*/

using namespace core;

ModelViewportPanel::ModelViewportPanel(wxWindow *parent) : BaseViewportPanel(parent)
{
	entity = engine->CreateClientEntity();
	if (entity)
	{
		entity->alpha = 255;
		entity->origin[0] = 50;
		entity->origin[1] = 0;
		entity->origin[2] = 0;
		entity->model = NULL;
	}
	else
		plWriteLog(EDITOR_LOG, "Failed to create client entity!\n");

	rotate = false;
}

void ModelViewportPanel::Draw()
{
	draw::GradientBackground(pl_white, pl_black);

	if (entity && entity->model)
	{
		entity->frame++;
		if (entity->frame >= entity->model->numframes)
			entity->frame = 0;

		engine->DrawEntity(entity);

		// Dolly rotate
		if (rotate)
			entity->angles[1] += 0.5f;
	}

	draw::ResetCanvas();
	engine->DrawSetCanvas(CANVAS_DEFAULT);

	if (entity && entity->model)
	{
		std::string strout = "Model: ";
		strout.append(entity->model->name);
		draw::String(10, 10, strout.c_str());

		strout.clear();
		strout = "Frame: ";
		strout += 
			std::to_string(entity->frame + 1)
			+ "/"
			+ std::to_string(entity->model->numframes);
		draw::String(10, 20, strout.c_str());
	}
}

void ModelViewportPanel::SetModel(model_t *newmodel)
{
	if (entity->model)
	{
		engine->Print("TODO: UNLOAD PREVIOUS MODEL!!!!\n");
	}
	entity->model = newmodel;

	entity->origin[0] = 0;
	for (unsigned int i = 0; i < 1000; i++)
	{
		
	}
}

void ModelViewportPanel::SetRotate(bool dorotate)
{
	if ((rotate == true) && (dorotate == false))
		// If we're setting it to false, reset the models current angle.
		entity->angles[1] = 0;

	rotate = dorotate;
}

// Handle input.
void ModelViewportPanel::OnKey(wxKeyEvent &event)
{
	switch (event.GetKeyCode())
	{
	case WXK_UP:
		entity->origin[0] -= 2.0f;
		break;
	case WXK_DOWN:
		entity->origin[0] += 2.0f;
		break;
	case WXK_LEFT:
		entity->angles[1] -= 2.0f;
		break;
	case WXK_RIGHT:
		entity->angles[1] += 2.0f;
		break;
	default:
		break;
	}
}

/*
	Frame
*/

enum
{
	MODELFRAME_EVENT_ROTATE,
	MODELFRAME_VIEW_BBOX,
};

wxBEGIN_EVENT_TABLE(ModelFrame, wxFrame)

EVT_MENU(wxID_OPEN, ModelFrame::FileEvent)
EVT_MENU(wxID_EXIT, ModelFrame::FileEvent)

EVT_MENU(MODELFRAME_EVENT_ROTATE, ModelFrame::ViewEvent)
EVT_MENU(MODELFRAME_VIEW_BBOX, ModelFrame::ViewEvent)

EVT_CLOSE(ModelFrame::CloseEvent)

wxEND_EVENT_TABLE()

#define	MODELFRAME_TITLE "Model Viewer"

ModelFrame::ModelFrame(wxWindow *parent)
	: wxFrame(parent, EDITOR_WINDOW_MODEL, MODELFRAME_TITLE, wxDefaultPosition, wxSize(640, 480))
{
	SetIcon(wxIcon("resource/icon-mdl.ico", wxBITMAP_TYPE_ICO));

	wxMenuBar *menubar = new wxMenuBar;
	
	// File
	wxMenu *menu_file = new wxMenu;
	menu_file->Append(new wxMenuItem(menu_file, wxID_OPEN));
	menu_file->Append(new wxMenuItem(menu_file, wxID_CLOSE));
	menu_file->Append(new wxMenuItem(menu_file, wxID_EXIT));
	menubar->Append(menu_file, "&File");
	
	// View
	wxMenu *menu_view = new wxMenu;
	v_rotate = new wxMenuItem(menu_view, MODELFRAME_EVENT_ROTATE, "&Rotate", "Dolly rotates the current preview", wxITEM_CHECK);
	menu_view->Append(v_rotate);
	menu_view->AppendSeparator();
	v_showbboxes = new wxMenuItem(menu_view, MODELFRAME_VIEW_BBOX, "&Show bounding box", "Show renderable area for the model", wxITEM_CHECK);
	menu_view->Append(v_showbboxes);
	menubar->Append(menu_view, "&View");
	
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
		wxMessageBox(wxString("Failed to update path! (" + newpath + ")\nPlease ensure your model is inside the game directory."), MODELFRAME_TITLE);
		return;
	}
	newpath.Remove(0, stringmod);

	model_t *model = engine->LoadModel(newpath);
	if (!model)
		return;

	viewport->SetModel(model);

	SetTitle(newpath + " - " MODELFRAME_TITLE);
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
				"Supported files"
				"(*.3d;*.md2;*.obj)|*.3d;*.md2;*.obj",
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
		viewport->SetRotate(v_rotate->IsChecked());
		break;
	case MODELFRAME_VIEW_BBOX:
		engine->SetConsoleVariable("r_showbboxes", v_showbboxes->IsChecked() ? "1" : "0");
		break;
	default:
		engine->Print("Unknown view event!\n");
	}
}

void ModelFrame::CloseEvent(wxCloseEvent &event)
{
	Show(false);
}