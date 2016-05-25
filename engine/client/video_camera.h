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

#define	CAMERA_LEGACY	// Camera automatically follows parent entity.

namespace Core
{
	ConsoleVariable_t cv_camera_forwardcycle = { "camera_forwardcycle", "0.43", true };
	ConsoleVariable_t cv_camera_sidecycle = { "camera_sidecycle", "0.86", true };
	ConsoleVariable_t cv_camera_upcycle = { "camera_upcycle", "0.45", true };
	ConsoleVariable_t cv_camera_bob = { "camera_bob", "0.002", true };

	ConsoleVariable_t cv_camera_modellag = { "camera_modellag", "0.2", true };
	ConsoleVariable_t cv_camera_modelposition = { "camera_modelposition", "1", true };

	ConsoleVariable_t cv_camera_rollangle = { "camera_rollangle", "2.0", true };
	ConsoleVariable_t cv_camera_rollspeed = { "camera_rollspeed", "200", true };

	ConsoleVariable_t cv_camera_punch = { "camera_punch", "1", true };

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

		void SetPosition(float x, float y, float z);
		void SetPosition(plVector3f_t _position);
		void PrintPosition();

#ifdef CAMERA_LEGACY
		void SetParentEntity(ClientEntity_t *_parent);
		void SetViewEntity(ClientEntity_t *_child);
#endif

		bool IsPointInsideFrustum(plVector3f_t position);
		bool IsPointOutsideFrustum(plVector3f_t position);
		bool IsBoxInsideFrustum(plVector3f_t mins, plVector3f_t maxs);
		bool IsBoxOutsideFrustum(plVector3f_t mins, plVector3f_t maxs);

		std::vector<float> GetForward() { return std::vector<float> { forward[0], forward[1], forward[2]}; }
		std::vector<float> GetRight() { return std::vector<float> { right[0], right[1], right[2] }; }
		std::vector<float> GetUp() { return std::vector<float> { up[0], up[1], up[2] }; }

		void EnableBob() { bobcam = true; }
		void DisableBob() { bobcam = false; }

		void ForceCenter() { angles[PL_PITCH] = 0; }	// Forces the pitch to become centered.
	protected:
	private:
		void CalculateBob();
		void CalculateRoll();

#ifdef CAMERA_LEGACY
		void SimulateViewEntity();
		void SimulateParentEntity();

		// TODO: Move these over into a seperate class,
		// they don't really make sense here for the camera codebase
		ClientEntity_t *viewmodel, *parententity;
#endif

		bool			bobcam;
		plVector2f_t	bobamount;

		mplane_t frustum[4];

		plVector3f_t forward, right, up;
		plVector3f_t punchangles[2];		//johnfitz -- copied from cl.punchangle.  0 is current, 1 is previous value. never the same unless map just loaded
		plVector3f_t angles, position;

		float height;	// Additional height of the camera.
	};

	class CameraManager : public CoreManager
	{
	public:
		CameraManager();
		~CameraManager();

		Camera *CreateCamera();
		void DeleteCamera(Camera *_camera);

		Camera *GetCurrentCamera() { return current_camera; }
		void SetCurrentCamera(Camera *_camera);

		void Draw();
		void Simulate();
	protected:
	private:
		std::vector<Camera*> cameras;

		Camera *current_camera;
	};
}

Core::CameraManager *g_cameramanager;