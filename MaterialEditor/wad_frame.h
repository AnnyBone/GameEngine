/*	DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
	Version 2, December 2004

	Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

	Everyone is permitted to copy and distribute verbatim or modified
	copies of this license document, and changing it is allowed as long
	as the name is changed.

	DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
	TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

	0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#ifndef WAD_FRAME_H
#define	WAD_FRAME_H

class CWADFrame : public wxFrame
{
public:
	CWADFrame(wxWindow* parent, wxWindowID id = wxID_ANY);
	~CWADFrame();

private:
	wxAuiManager *mManager;
};

#endif // !WAD_FRAME_H

