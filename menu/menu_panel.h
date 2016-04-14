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

#pragma once

namespace Menu
{
	class Panel
	{
	public:
		Panel();
		~Panel();

		bool IsInside(int xpos, int ypos);	// Within x and y coords.
		bool IsInside(Panel *panel);		// Test if we're inside another panel.

		// Events
		virtual void Click(float x, float y) {}
		virtual void Activated() {}
		virtual void Deactivated() {}
		virtual void MouseMove(float x, float y) {}
		virtual void MouseLeave() {}

		virtual void Draw();
	protected:
	private:
		unsigned int x, y;			// Position coordinates.

		Panel *parent;

		unsigned int width, height;
	};
}