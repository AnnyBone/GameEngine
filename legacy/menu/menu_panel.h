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

#pragma once

namespace Menu
{
	class Panel
	{
	public:
		Panel(Panel *parent);
		~Panel();

		bool IsInside(unsigned int xpos, unsigned int ypos);	// Within x and y coords.
		bool IsInside(Panel *panel);							// Test if we're inside another panel.

		// Events
		virtual void Click(float x, float y) {}
		virtual void Activated() {}
		virtual void Deactivated() {}
		virtual void MouseMove(float x, float y) {}
		virtual void MouseLeave() {}
		virtual void Moved(int nx, int ny);
		virtual void Close() {}
		virtual void Open() {}

		virtual void Draw();

		void AddChild(Panel *child);
		void RemoveChild(Panel *child);

		virtual void SetWidth(unsigned int w) { _width = w; }
		virtual void SetHeight(unsigned int h) { _height = h; }

		unsigned int GetWidth() { return _width; }
		unsigned int GetHeight() { return _height; }
	protected:
	private:
		std::vector<Panel*> _children;

		unsigned int _width, _height;	// Size.
		unsigned int _x, _y;			// Position coordinates.

		Panel *_parent;
	};
}