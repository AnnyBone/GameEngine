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

#include "engine_base.h"

#include "video.h"

namespace Core
{
	namespace Client
	{
		class Camera
		{
		public:
			Camera();
			Camera(plVector3f_t position);

			void Draw();
			void Simulate();

			void SetAngles(float x, float y, float z);
			void PrintAngles();

			void SetFrustum(float fovx, float fovy);

			bool IsPointInsideFrustum(plVector3f_t position);
			bool IsPointOutsideFrustum(plVector3f_t position);
			bool IsBoxInsideFrustum(plVector3f_t mins, plVector3f_t maxs);
			bool IsBoxOutsideFrustum(plVector3f_t mins, plVector3f_t maxs);

			void ForceCenter() { angles[PL_PITCH] = 0; }	// Forces the pitch to become centered.
		protected:
		private:
			mplane_t frustum[4];

			plVector3f_t forward, right, up;
			plVector3f_t punchangles[2];		//johnfitz -- copied from cl.punchangle.  0 is current, 1 is previous value. never the same unless map just loaded
			plVector3f_t angles, position;
		};

		class CameraManager : public CoreManager
		{
		public:
			CameraManager();
			~CameraManager();

			void Draw();
			void Simulate();
		protected:
		private:
			std::vector<Camera*> cameras;
		};
	}
}

using namespace Core::Client;

/*
	Camera Manager
*/

CameraManager::CameraManager()
{
	cameras.reserve(5);
}

void CameraManager::Draw()
{
	for (unsigned int i = 0; i < cameras.size(); i++)
		cameras[i]->Draw();
}

void CameraManager::Simulate()
{
	for (unsigned int i = 0; i < cameras.size(); i++)
		cameras[i]->Simulate();
}

/*
	Camera
*/

void Camera::Draw()
{
}

void Camera::Simulate()
{
}

// Frustum

extern "C" {
	int SignbitsForPlane(mplane_t *out);
}

void Camera::SetFrustum(float fovx, float fovy)
{
	plTurnVector(frustum[0].normal, position, right, fovx / 2 - 90);	// Left plane
	plTurnVector(frustum[1].normal, position, right, 90 - fovx / 2);	// Right plane
	plTurnVector(frustum[2].normal, position, up, 90 - fovy / 2);		// Bottom plane
	plTurnVector(frustum[3].normal, position, up, fovy / 2 - 90);		// Top plane

	for (int i = 0; i < 4; i++)
	{
		frustum[i].type = PLANE_ANYZ;
		frustum[i].dist = Math_DotProduct(position, frustum[i].normal); // FIXME: shouldn't this always be zero?
		frustum[i].signbits = SignbitsForPlane(&frustum[i]);
	}
}

bool Camera::IsPointInsideFrustum(plVector3f_t position)
{
	for (int i = 0; i < 4; i++)
	{
		mplane_t *p = frustum + i;
		if (p->normal[0] * position[0] + p->normal[1] * position[1] + p->normal[2] * position[2] > p->dist)
			return false;
	}

	return true;
}

bool Camera::IsPointOutsideFrustum(plVector3f_t position)
{
	for (int i = 0; i < 4; i++)
	{
		mplane_t *p = frustum + i;
		if (p->normal[0] * position[0] + p->normal[1] * position[1] + p->normal[2] * position[2] < p->dist)
			return false;
	}

	return true;
}

bool Camera::IsBoxInsideFrustum(plVector3f_t mins, plVector3f_t maxs)
{
	for (int i = 0; i < 4; i++)
	{
		mplane_t *p = frustum + i;
		switch (p->signbits)
		{
		default:
		case 0:
			if (p->normal[0] * maxs[0] + p->normal[1] * maxs[1] + p->normal[2] * maxs[2] > p->dist)
				return true;
			break;
		case 1:
			if (p->normal[0] * mins[0] + p->normal[1] * maxs[1] + p->normal[2] * maxs[2] > p->dist)
				return true;
			break;
		case 2:
			if (p->normal[0] * maxs[0] + p->normal[1] * mins[1] + p->normal[2] * maxs[2] > p->dist)
				return true;
			break;
		case 3:
			if (p->normal[0] * mins[0] + p->normal[1] * mins[1] + p->normal[2] * maxs[2] > p->dist)
				return true;
			break;
		case 4:
			if (p->normal[0] * maxs[0] + p->normal[1] * maxs[1] + p->normal[2] * mins[2] > p->dist)
				return true;
			break;
		case 5:
			if (p->normal[0] * mins[0] + p->normal[1] * maxs[1] + p->normal[2] * mins[2] > p->dist)
				return true;
			break;
		case 6:
			if (p->normal[0] * maxs[0] + p->normal[1] * mins[1] + p->normal[2] * mins[2] > p->dist)
				return true;
			break;
		case 7:
			if (p->normal[0] * mins[0] + p->normal[1] * mins[1] + p->normal[2] * mins[2] > p->dist)
				return true;
			break;
		}
	}

	return false;
}

bool Camera::IsBoxOutsideFrustum(plVector3f_t mins, plVector3f_t maxs)
{
	for (int i = 0; i < 4; i++)
	{
		mplane_t *p = frustum + i;
		switch (p->signbits)
		{
		default:
		case 0:
			if (p->normal[0] * maxs[0] + p->normal[1] * maxs[1] + p->normal[2] * maxs[2] < p->dist)
				return true;
			break;
		case 1:
			if (p->normal[0] * mins[0] + p->normal[1] * maxs[1] + p->normal[2] * maxs[2] < p->dist)
				return true;
			break;
		case 2:
			if (p->normal[0] * maxs[0] + p->normal[1] * mins[1] + p->normal[2] * maxs[2] < p->dist)
				return true;
			break;
		case 3:
			if (p->normal[0] * mins[0] + p->normal[1] * mins[1] + p->normal[2] * maxs[2] < p->dist)
				return true;
			break;
		case 4:
			if (p->normal[0] * maxs[0] + p->normal[1] * maxs[1] + p->normal[2] * mins[2] < p->dist)
				return true;
			break;
		case 5:
			if (p->normal[0] * mins[0] + p->normal[1] * maxs[1] + p->normal[2] * mins[2] < p->dist)
				return true;
			break;
		case 6:
			if (p->normal[0] * maxs[0] + p->normal[1] * mins[1] + p->normal[2] * mins[2] < p->dist)
				return true;
			break;
		case 7:
			if (p->normal[0] * mins[0] + p->normal[1] * mins[1] + p->normal[2] * mins[2] < p->dist)
				return true;
			break;
		}
	}

	return false;
}

// Angles

void Camera::SetAngles(float x, float y, float z)
{
	plVectorSet3f(angles, x, y, z);
}

void Camera::PrintAngles()
{
	Con_Printf("CAMERA: (%i %i %i) %i %i %i\n",
		(int)position[PL_X],
		(int)position[PL_Y],
		(int)position[PL_Z],
		(int)angles[PL_PITCH],
		(int)angles[PL_YAW],
		(int)angles[PL_ROLL]);
}