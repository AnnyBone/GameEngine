/*	
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

/*	Shared Video Interface */

// Canvas
typedef enum {
    CANVAS_NONE,
    CANVAS_DEFAULT,
    CANVAS_CONSOLE,
    CANVAS_MENU,
    CANVAS_SBAR,
    CANVAS_WARPIMAGE,
    CANVAS_CROSSHAIR,
    CANVAS_BOTTOMLEFT,
    CANVAS_BOTTOMRIGHT,
    CANVAS_TOPRIGHT
} VideoCanvasType_t;

#ifdef __cplusplus

namespace core {
    class ICamera;

    // todo, move this into its own header.
    class IViewport {
    public:
        virtual std::vector<unsigned int> GetSize() = 0;

        virtual void SetSize(unsigned int width, unsigned int height) = 0;

        virtual unsigned int GetWidth() = 0;

        virtual unsigned int GetHeight() = 0;

        virtual ICamera *GetCamera() const = 0;

        virtual void SetCamera(ICamera *camera) = 0;

        virtual std::vector<int> GetPosition() = 0;

        virtual void SetPosition(int x, int y) = 0;

        virtual void Draw() = 0;
    };

    // todo, move this into its own header.
    class ICamera {
    public:
        virtual std::vector<float> GetAngles() = 0;

        virtual void SetAngles(float x, float y, float z) = 0;

        virtual void PrintAngles() = 0;

        virtual float GetFOV() = 0;

        virtual void SetFOV(float fov) = 0;

        virtual void SetupProjectionMatrix() = 0;

        virtual void SetupViewMatrix() = 0;

        virtual std::vector<float> GetPosition() = 0;

        virtual void SetPosition(float x, float y, float z) = 0;

        virtual void SetPosition(plVector3f_t position) = 0;

        virtual void PrintPosition() = 0;

        virtual IViewport *GetViewport() = 0;

        virtual void SetViewport(IViewport *viewport) = 0;

        virtual bool IsPointInsideFrustum(plVector3f_t position) = 0;

        virtual bool IsPointOutsideFrustum(plVector3f_t position) = 0;

        virtual bool IsBoxInsideFrustum(plVector3f_t mins, plVector3f_t maxs) = 0;

        virtual bool IsBoxOutsideFrustum(plVector3f_t mins, plVector3f_t maxs) = 0;

        virtual std::vector<float> GetForward() = 0;

        virtual std::vector<float> GetRight() = 0;

        virtual std::vector<float> GetUp() = 0;

        virtual void EnableBob() = 0;

        virtual void DisableBob() = 0;

        virtual void ForceCenter() = 0;
    };

    namespace draw {
        ENGINE_FUNCTION void ClearBuffers();    // Clears the color, stencil and depth buffers.
        ENGINE_FUNCTION void DepthBuffer();    // Draws the depth buffer.

        ENGINE_FUNCTION void SetDefaultState();    // Sets default rendering state.

        ENGINE_FUNCTION void GradientBackground(PLColour top, PLColour bottom);

        ENGINE_FUNCTION void GradientFill(int x, int y, PLuint width, PLuint height, PLColour top, PLColour bottom);

        ENGINE_FUNCTION void WireBox(plVector3f_t mins, plVector3f_t maxs, float r, float g, float b);

        ENGINE_FUNCTION void CoordinateAxes(plVector3f_t position);

        ENGINE_FUNCTION void Character(PLint x, PLint y, PLint num);

        ENGINE_FUNCTION void String(int x, int y, const char *msg);

        ENGINE_FUNCTION void Line(plVector3f_t start, plVector3f_t end);

        ENGINE_FUNCTION void Grid(plVector3f_t position, PLuint grid_size);

        ENGINE_FUNCTION void Rectangle(PLint x, PLint y, PLuint w, PLuint h, PLColour colour);

        ENGINE_FUNCTION void ScreenFade();

        // Canvas
        ENGINE_FUNCTION void ResetCanvas();
    }
}

typedef core::IViewport CoreViewport;
typedef core::ICamera CoreCamera;

#else

typedef struct CoreViewport CoreViewport;
typedef struct CoreCamera CoreCamera;

#endif
