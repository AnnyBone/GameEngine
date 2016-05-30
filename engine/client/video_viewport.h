/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2011-2016 OldTimes Software

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

namespace Core
{
	class Camera;

	class Viewport
	{
	public:
		Viewport(unsigned int width, unsigned int height);

		std::vector<unsigned int> GetSize() { return std::vector<unsigned int> { _width, _height }; }	// Returns array of both width and height.
		unsigned int GetWidth() { return _width; }														// Returns width.
		unsigned int GetHeight() { return _height; }													// Returns height.
		void SetSize(unsigned int width, unsigned int height);

		Camera *GetCamera() { return _camera; }
		void SetCamera(Camera *camera);

		std::vector<int> GetPosition() { return std::vector<int> { _x, _y }; }	// Returns position on screen.
		void SetPosition(int x, int y);
		
		void Draw();

	protected:
	private:
		Camera *_camera;

		unsigned int _width, _height;

		int _x, _y;
	};
}