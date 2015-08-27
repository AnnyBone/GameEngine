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

#pragma once
class CEditorMaterialGlobalProperties : public wxPropertyGrid
{
public:
	CEditorMaterialGlobalProperties(wxWindow *parent);
	~CEditorMaterialGlobalProperties();

	void Update();
	void SetCurrentMaterial(Material_t *newMaterial);

private:
	void OnChanged(wxPropertyGridEvent &event);

	wxPGProperty 
		*materialCategory,
		*skinCategory;

	wxStringProperty* name;
	wxStringProperty* path;

	wxBoolProperty* preserve;
	wxBoolProperty* useAlpha;
	wxBoolProperty* blend;
	wxBoolProperty* animated;
	wxBoolProperty* mirror;
	wxBoolProperty* nearest;
	wxBoolProperty* water;
	wxBoolProperty* notris;

	wxIntProperty* skins;

	wxFloatProperty* animationSpeed;
	wxFloatProperty* alpha;

	Material_t *currentMaterial;

};

