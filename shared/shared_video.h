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

#include "platform_video_layer.h"

#define	VIDEO_MAX_UNITS	16

// Canvas
typedef enum
{
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

#define CAMERA_LEGACY

#ifdef __cplusplus

namespace Core
{
	class ICamera;

	class IViewport
	{
	public:
		virtual std::vector<unsigned int> GetSize() = 0;
		virtual void SetSize(unsigned int width, unsigned int height) = 0;

		virtual unsigned int GetWidth() = 0;
		virtual unsigned int GetHeight() = 0;

		virtual ICamera *GetCamera() = 0;
		virtual void SetCamera(ICamera *camera) = 0;

		virtual std::vector<int> GetPosition() = 0;
		virtual void SetPosition(int x, int y) = 0;
	};

	class ICamera
	{
	public:
		virtual std::vector<float> GetAngles() = 0;
		virtual void SetAngles(float x, float y, float z) = 0;
		virtual void PrintAngles() = 0;

		virtual float GetFOV() = 0;
		virtual void SetFOV(float fov) = 0;

		virtual void SetFrustum(float fovx, float fovy) = 0;

		virtual void SetViewport(IViewport *viewport) = 0;

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
}

typedef Core::IViewport CoreViewport;
typedef Core::ICamera CoreCamera;

#else

typedef struct CoreViewport CoreViewport;
typedef struct CoreCamera CoreCamera;

#endif
