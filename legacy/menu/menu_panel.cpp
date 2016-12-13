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

#include "menu_main.h"

#include "menu_panel.h"

namespace Menu
{
	Panel::Panel(Panel *parent) :
		_x(0), _y(0),
		_width(128), _height(128)
	{
		if (!parent)
			Engine.Con_Warning("Invalid parent!\n");

		_parent = parent;
	}

	Panel::~Panel()
	{}

	bool Panel::IsInside(unsigned int xpos, unsigned int ypos)
	{
		if ((xpos < _x) || (ypos < _y) || (xpos >(_x + _width)) || (ypos >(_y + _height)))
			return false;

		return true;
	}

	bool Panel::IsInside(Menu::Panel *panel)
	{
		return IsInside(panel->_x, panel->_y);
	}

	//=========================
	// Sorting
	//=========================

	void Panel::AddChild(Panel *child)
	{
		if (!child)
			return;
		_children.push_back(child);
	}

	void Panel::RemoveChild(Panel *child)
	{
		if (!child)
			return;
	}

	//=========================
	// Events
	//=========================

	void Panel::Moved(int nx, int ny)
	{
		// Let all the children know we've moved.
		for (unsigned int i = 0; i < _children.size(); i++)
			_children.at(i)->Moved(nx, ny);
	}

	//=========================

	void Panel::Draw()
	{
	}
}