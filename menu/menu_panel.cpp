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

#include "menu_main.h"

class MenuPanel
{
public:
	MenuPanel();

	bool IsInside(int xpos, int ypos);
	bool IsInside(MenuPanel *panel);
protected:
private:
	int x, y;			// Position coordinates.

	int width, height;
};

MenuPanel::MenuPanel()
{
	x = 0;
	y = 0;

	width = 128;
	height = 128;
}

bool MenuPanel::IsInside(int xpos, int ypos)
{
	if ((xpos < x) || (ypos < y) || (xpos >(x + width)) || (ypos >(y + height)))
		return false;
	
	return true;
}

bool MenuPanel::IsInside(MenuPanel *panel)
{
	return IsInside(panel->x, panel->y);
}