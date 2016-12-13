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

namespace core {
class Camera;

class Viewport : public IViewport {
 public:
  Viewport(int x, int y, unsigned int width, unsigned int height);
  Viewport(unsigned int width, unsigned int height);

  std::vector<unsigned int> GetSize() {
	return std::vector<unsigned int> {_width, _height};
  }    // Returns array of both width and height.
  unsigned int GetWidth() { return _width; }                                                        // Returns width.
  unsigned int GetHeight() { return _height; }                                                    // Returns height.
  void SetSize(unsigned int width, unsigned int height);

  ICamera *GetCamera() const;
  void SetCamera(ICamera *camera);

  std::vector<int> GetPosition() { return std::vector<int> {_x, _y}; }    // Returns position on screen.
  void SetPosition(int x, int y);

  void Draw();
  void Screenshot();

 protected:
 private:
  Camera *_camera;

  std::vector<Viewport *> _children;
  Viewport *_parent;

  unsigned int _width, _height;
  int _x, _y;
};

namespace client {
class ViewportManager : public XManager {
 public:
  ViewportManager();
  ~ViewportManager();

  Viewport *GetPrimaryViewport() const { return _primary_viewport; }
  Viewport *GetCurrentViewport() const { return _current_viewport; }

 protected:
 private:
  std::vector<Viewport *> _viewports;

  Viewport *_primary_viewport;    // The primary viewport the user controls.
  Viewport *_current_viewport;    // Current viewport being used for draw.
};
}

// Input, sound and more
Viewport *GetPrimaryViewport();
void SetPrimaryViewport(Viewport *viewport);

// Rendering
Viewport *GetCurrentViewport();
void SetCurrentViewport(Viewport *viewport);
}

#endif