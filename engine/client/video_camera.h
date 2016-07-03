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

#define	CAMERA_LEGACY	// Camera automatically follows parent entity.

namespace core
{
	class Viewport;

	class Camera : public ICamera
	{
	public:
		Camera();
		Camera(Viewport *viewport);

		void Draw();
		void Simulate();

		std::vector<float> GetAngles() { return std::vector<float> { angles[0], angles[1], angles[2] }; }
		void SetAngles(float x, float y, float z);
		void PrintAngles();

		float GetFOV() { return _fovx; }

		void SetFOV(float fov);
		void SetFrustum(float fovx, float fovy);

		std::vector<float> GetPosition() { return std::vector<float> { position[0], position[1], position[2] }; }
		void SetPosition(float x, float y, float z);
		void SetPosition(plVector3f_t _position);
		void PrintPosition();
		void TracePosition();

#ifdef CAMERA_LEGACY
		void SetParentEntity(ClientEntity_t *_parent);
		void SetViewEntity(ClientEntity_t *_child);
#endif

		IViewport *GetViewport() { return _viewport; }
		void SetViewport(IViewport *viewport);

		bool IsPointInsideFrustum(plVector3f_t position);
		bool IsPointOutsideFrustum(plVector3f_t position);
		bool IsBoxInsideFrustum(plVector3f_t mins, plVector3f_t maxs);
		bool IsBoxOutsideFrustum(plVector3f_t mins, plVector3f_t maxs);

		std::vector<float> GetForward() { return std::vector<float> { _forward[0], _forward[1], _forward[2]}; }
		std::vector<float> GetRight() { return std::vector<float> { _right[0], _right[1], _right[2] }; }
		std::vector<float> GetUp() { return std::vector<float> { _up[0], _up[1], _up[2] }; }

		void EnableBob() { bobcam = true; }
		void DisableBob() { bobcam = false; }

		void ForceCenter() { angles[PL_PITCH] = 0; }	// Forces the pitch to become centered.
	
		mleaf_t *leaf, *oldleaf;
	protected:
	private:
		Viewport *_viewport;

		void SimulateFrustum();
		void SimulateBob();
		void SimulateRoll();

#ifdef CAMERA_LEGACY
		void SimulateViewEntity();
		void SimulateParentEntity();

		void DrawViewEntity();
		void DrawParentEntity();

		// TODO: Move these over into a seperate class,
		// they don't really make sense here for the camera codebase
		ClientEntity_t *_viewmodel, *parententity;
#endif

		bool			bobcam;
		plVector2f_t	bobamount;

		mplane_t frustum[4];

		plVector3f_t _forward, _right, _up;
		plVector3f_t punchangles[2];		//johnfitz -- copied from cl.punchangle.  0 is current, 1 is previous value. never the same unless map just loaded
		plVector3f_t angles, position;

		float _fovx, _fovy;
		float _fov;

		float height;	// Additional height of the camera.
	};

	class CameraManager : public CoreManager
	{
	public:
		CameraManager();
		~CameraManager();

		Camera *CreateCamera();
		void DeleteCamera(Camera *camera);

		Camera *GetCurrentCamera() { return _current_camera; }
		void SetCurrentCamera(Camera *camera);

		void Draw();
		void Simulate();
	protected:
	private:
		std::vector<Camera*> _cameras;

		Camera *_current_camera;
	};
}

extern core::CameraManager *g_cameramanager;

#endif