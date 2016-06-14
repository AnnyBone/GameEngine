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

#ifdef __cplusplus

namespace Core
{
	class Camera;

	class Viewport : public IViewport
	{
	public:
		Viewport(unsigned int width, unsigned int height);

		std::vector<unsigned int> GetSize() { return std::vector<unsigned int> { _width, _height }; }	// Returns array of both width and height.
		unsigned int GetWidth() { return _width; }														// Returns width.
		unsigned int GetHeight() { return _height; }													// Returns height.
		void SetSize(unsigned int width, unsigned int height);

		ICamera *GetCamera();
		void SetCamera(ICamera *camera);

		std::vector<int> GetPosition() { return std::vector<int> { _x, _y }; }	// Returns position on screen.
		void SetPosition(int x, int y);
		
		void Draw();

	protected:
	private:
		Camera *_camera;

		unsigned int _width, _height;

		int _x, _y;
	};

	void CreatePrimaryViewport();
	Viewport *GetPrimaryViewport();
	void DestroyPrimaryViewport();
}

#endif