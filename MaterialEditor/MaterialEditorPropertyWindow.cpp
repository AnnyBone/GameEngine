#include "MaterialEditorBase.h"

#include "MaterialEditorPropertyWindow.h"

#include <wx/propgrid/propgrid.h>

wxBEGIN_EVENT_TABLE(CMaterialEditorPropertyWindow, wxFrame)
EVT_MENU(wxID_EXIT, CMaterialEditorPropertyWindow::OnExit)
wxEND_EVENT_TABLE()

CMaterialEditorPropertyWindow::CMaterialEditorPropertyWindow(const wxPoint & pos, const wxSize & size)
: wxFrame(NULL, wxID_ANY, "Properties", pos, size, wxRESIZE_BORDER | wxCAPTION)
{
	wxPropertyGrid *propertyGrid = new wxPropertyGrid(this);

	// Globals

	propertyGrid->Append(new wxPropertyCategory("Global"));
	propertyGrid->Append(new wxFileProperty("Path"));
	propertyGrid->Append(new wxIntProperty("Animation Frame"));
	propertyGrid->Append(new wxFloatProperty("Alpha"));
	propertyGrid->Append(new wxFloatProperty("Animation Speed"));
	propertyGrid->Append(new wxFloatProperty("Animation Time"));

	// Skins

	propertyGrid->Append(new wxPropertyCategory("Skin"));

#if 0
	int i;
	for (i = 0; i < 10; i++)
	{
		wxPGProperty *newSkin = propertyGrid->Append(new wxStringProperty("Skin", wxPG_LABEL));
	}
#endif

	// Textures

	propertyGrid->Append(new wxPropertyCategory("Texture"));

	Show();
}

void CMaterialEditorPropertyWindow::OnExit(wxCommandEvent &event)
{}
