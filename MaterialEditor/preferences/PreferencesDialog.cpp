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

#include "../EditorBase.h"

#include "PreferencesDialog.h"

wxBEGIN_EVENT_TABLE(CPreferencesDialog, wxDialog)
wxEND_EVENT_TABLE()

CPreferencesDialog::CPreferencesDialog(wxWindow *wParent)
	: wxDialog(wParent, wxID_ANY, "Preferences", wxDefaultPosition, wxSize(512, 512))
{
	SetSizeHints(wxSize(512, 512), wxSize(512, 512));

	wxBoxSizer *MainGrid = new wxBoxSizer(wxVERTICAL);

	Notebook = new wxNotebook(this, wxID_ANY);

	VideoPanel = new wxPanel(Notebook);
	AudioPanel = new wxPanel(Notebook);
	InputPanel = new wxPanel(Notebook);

	// General Settings

	wxGridSizer *GeneralGrid = new wxGridSizer(4, 4, 0, 0);

	GeneralPanel = new wxPanel(Notebook);
	GeneralPanel->SetSizer(GeneralGrid);

	wxCheckBox *AutoReload = new wxCheckBox(GeneralPanel, wxID_ANY, "Automatically reload documents?");
	AutoReload->SetValue(true);

	GeneralGrid->Add(AutoReload);

	GeneralPanel->Layout();

	//

	// Add everything.
	Notebook->AddPage(GeneralPanel, "General", true);
	Notebook->AddPage(VideoPanel, "Video");
	Notebook->AddPage(AudioPanel, "Audio");
	Notebook->AddPage(InputPanel, "Input");

	MainGrid->Add(Notebook, 1, wxEXPAND | wxALL, 4);

	wxButton *ApplyButton = new wxButton(this, wxID_APPLY);
	wxButton *OKButton = new wxButton(this, wxID_OK);
	wxButton *CancelButton = new wxButton(this, wxID_CANCEL);

	wxStdDialogButtonSizer *ButtonSizer = new wxStdDialogButtonSizer();
	ButtonSizer->AddButton(ApplyButton);
	ButtonSizer->AddButton(OKButton);
	ButtonSizer->AddButton(CancelButton);
	ButtonSizer->Realize();

	MainGrid->Add(ButtonSizer, 0, wxBOTTOM | wxRIGHT | wxALIGN_RIGHT | wxEXPAND, 10);

	SetSizer(MainGrid);

	Layout();
	Centre();
}

CPreferencesDialog::~CPreferencesDialog()
{}