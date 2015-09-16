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

#ifndef __PREFERENCESDIALOG_H__
#define	__PREFERENCESDIALOG_H__

#pragma once

class CPreferencesDialog : public wxDialog
{
public:
	CPreferencesDialog(wxWindow *wParent);
	~CPreferencesDialog();

	virtual bool Show();

	void SyncSettings();
	void ApplySettings();

private:
	// Events
	void HandleCommandEvent(wxCommandEvent &event);

	wxNotebook *Notebook;

	wxCheckBox *AutoReload;
	wxSpinCtrl *AutoReloadDelay;

	wxCheckBox *VideoLegacyMode;
	wxCheckBox *VideoAlphaTrick;

	wxPanel *VideoPanel;
	wxPanel *AudioPanel;
	wxPanel *InputPanel;
	wxPanel *GeneralPanel;

	wxDECLARE_EVENT_TABLE();
};

#endif