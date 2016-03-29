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

			void Draw();
			void Simulate();

			void SetAngles(float x, float y, float z);

			void PrintAngles();

			void ForceCenter() { angles[PL_PITCH] = 0; }	// Forces the pitch to become centered.
		protected:
		private:
			plVector3f_t forward, right, up;

			plVector3f_t punchangles[2];	//johnfitz -- copied from cl.punchangle.  0 is current, 1 is previous value. never the same unless map just loaded

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