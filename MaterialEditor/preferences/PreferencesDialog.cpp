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

#include "../EditorBase.h"

#include "PreferencesDialog.h"

wxBEGIN_EVENT_TABLE(CPreferencesDialog, wxDialog)
EVT_BUTTON(wxID_APPLY, CPreferencesDialog::HandleCommandEvent)
EVT_BUTTON(wxID_OK, CPreferencesDialog::HandleCommandEvent)
EVT_BUTTON(wxID_CANCEL, CPreferencesDialog::HandleCommandEvent)
wxEND_EVENT_TABLE()

CPreferencesDialog::CPreferencesDialog(wxWindow *wParent)
	: wxDialog(wParent, wxID_ANY, "Preferences", wxDefaultPosition, wxSize(512, 512))
{
	SetSizeHints(wxSize(512, 512), wxSize(512, 512));

	wxBoxSizer *MainGrid = new wxBoxSizer(wxVERTICAL);

	Notebook = new wxNotebook(this, wxID_ANY);

	AudioPanel = new wxPanel(Notebook);
	InputPanel = new wxPanel(Notebook);

	// General Settings
	// TODO: Move this into its own seperate class
	{
		wxGridSizer *GeneralGrid = new wxGridSizer(2, 2, 0, 0);
		GeneralPanel = new wxPanel(Notebook);
		GeneralPanel->SetSizer(GeneralGrid);

		wxStaticBoxSizer *AutoReloadBox = new wxStaticBoxSizer(wxVERTICAL, GeneralPanel, "Auto Reload");
		GeneralGrid->Add(AutoReloadBox, 1, wxEXPAND | wxALL, 5);

		AutoReload = new wxCheckBox(GeneralPanel, wxID_ANY, "Automatically reload documents");
		AutoReloadBox->Add(AutoReload, 0, wxALL, 5);

		// Reload Delay
		{
			wxBoxSizer *AutoReloadDelayBox = new wxBoxSizer(wxHORIZONTAL);

			AutoReloadDelay = new wxSpinCtrl(GeneralPanel, wxID_ANY, "test", wxDefaultPosition, wxSize(64, wxDefaultSize.GetHeight()));
			AutoReloadDelayBox->Add(AutoReloadDelay, 0, wxALL);

			wxStaticText *AutoReloadDelayText = new wxStaticText(GeneralPanel, wxID_ANY, "Delay");
			AutoReloadDelayBox->Add(AutoReloadDelayText, 0, wxALL, 5);

			AutoReloadBox->Add(AutoReloadDelayBox, 0, wxALL, 5);
		}

		GeneralPanel->Layout();
	}

	// Video Settings
	// TODO: Move this into its own seperate class
	{
		wxGridSizer *VideoGrid = new wxGridSizer(1, 1, 0, 0);
		VideoPanel = new wxPanel(Notebook);
		VideoPanel->SetSizer(VideoGrid);

		{
			wxBoxSizer *VideoListLeft = new wxBoxSizer(wxVERTICAL);

			VideoListLeft->Add(new wxCheckBox(VideoPanel, wxID_ANY, "Dynamic lightmap updates"), 0, wxALL, 5);
			VideoListLeft->Add(new wxCheckBox(VideoPanel, wxID_ANY, "Stereoscopic 3D"), 0, wxALL, 5);

			VideoLegacyMode = new wxCheckBox(VideoPanel, wxID_ANY, "Legacy mode (no shader support)");
			VideoListLeft->Add(VideoLegacyMode, 0, wxALL, 5);

			VideoAlphaTrick = new wxCheckBox(VideoPanel, wxID_ANY, "Multiple pass alpha-test");
			VideoListLeft->Add(VideoAlphaTrick, 0, wxALL, 5);

			// Water
			{
				wxStaticBoxSizer *VideoWaterBox = new wxStaticBoxSizer(wxVERTICAL, VideoPanel, "Water");

				// Subdivision Slider
				{
					wxBoxSizer *SubSliderBox = new wxBoxSizer(wxHORIZONTAL);

					wxSlider *Subdivision = new wxSlider(VideoPanel, wxID_ANY, 0, 0, 1024, wxDefaultPosition, wxSize(128, wxDefaultSize.GetHeight()), wxSL_VALUE_LABEL);
					Subdivision->SetLineSize(2);
					Subdivision->SetPageSize(2);
					SubSliderBox->Add(Subdivision, 0, wxALL);

					wxStaticText *SubdivisionText = new wxStaticText(VideoPanel, wxID_ANY, "Subdivision");
					SubSliderBox->Add(SubdivisionText, 0, wxALL, 5);

					VideoWaterBox->Add(SubSliderBox, 0, wxALL, 5);
				}

				VideoWaterBox->Add(new wxCheckBox(VideoPanel, wxID_ANY, "Vertex lighting"), 0, wxALL, 5);
				VideoWaterBox->Add(new wxCheckBox(VideoPanel, wxID_ANY, "Vertex warping"), 0, wxALL, 5);

				VideoListLeft->Add(VideoWaterBox, 1, wxEXPAND | wxALL, 5);
			}

			VideoGrid->Add(VideoListLeft, 0, wxALL, 5);
		}

		VideoPanel->Layout();
	}

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

bool CPreferencesDialog::Show()
{
	SyncSettings();

	return wxDialog::Show();
}

void CPreferencesDialog::HandleCommandEvent(wxCommandEvent &event)
{
	switch (event.GetId())
	{
	case wxID_OK:
		ApplySettings();
	case wxID_CANCEL:
		Hide();
		break;
	case wxID_APPLY:
		ApplySettings();
		SyncSettings();
		break;
	default:
		// Do nothing.
		break;
	}
}

/*	Called to sync the preferences settings
	with those of the engine.
*/
void CPreferencesDialog::SyncSettings()
{
	AutoReload->SetValue(cvEditorAutoReload.bValue);
	AutoReloadDelay->SetValue(cvEditorAutoReloadDelay.iValue);

	// Video Settings
	VideoLegacyMode->SetValue(engine->GetConsoleVariableBoolValue("video_legacy"));
}

void CPreferencesDialog::ApplySettings()
{
	char buf[16];

	snprintf(buf, sizeof(buf), "%i", AutoReload->GetValue());
	engine->SetConsoleVariable(cvEditorAutoReload.name, buf);

	snprintf(buf, sizeof(buf), "%i", AutoReloadDelay->GetValue());
	engine->SetConsoleVariable(cvEditorAutoReloadDelay.name, buf);

	snprintf(buf, sizeof(buf), "%i", VideoLegacyMode->GetValue());
	engine->SetConsoleVariable("video_legacy", buf);
}
