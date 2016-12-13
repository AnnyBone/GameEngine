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

#define	CAMERA_LEGACY	// Camera automatically follows parent entity.

#ifdef __cplusplus

namespace core
{
	class Viewport;

	class Camera : public ICamera
	{
	public:
		Camera();
		Camera(Viewport *viewport);

		virtual void Draw();
		virtual void Simulate();

		void Input(ClientCommand_t *cmd, plVector2i_t mpos);

		std::vector<float> GetAngles() { return std::vector<float> { _angles[0], _angles[1], _angles[2] }; }
		void SetAngles(float x, float y, float z);
		void SetAngles(plVector3f_t angles);
		void SetPitch(float pitch) { _angles[PL_PITCH] = pitch; }
		void SetYaw(float yaw) { _angles[PL_YAW] = yaw; }
		void SetRoll(float roll) { _angles[PL_ROLL] = roll; }
		void PrintAngles();

		float GetFOV() { return _fovx; }
		void SetFOV(float fov);

		void SetupProjectionMatrix();
		void SetupViewMatrix();

		std::vector<float> GetPosition() { return std::vector<float> { _position[0], _position[1], _position[2] }; }
		void SetPosition(float x, float y, float z);
		void SetPosition(plVector3f_t position);
		void PrintPosition();
		void TracePosition();

#ifdef CAMERA_LEGACY
		void SetParentEntity(ClientEntity_t *parent);
		void SetViewEntity(ClientEntity_t *child);

		ClientEntity_t *GetParentEntity() { return _parententity; }
		ClientEntity_t *GetViewEntity() { return _viewmodel; }
#endif

		IViewport *GetViewport() { return reinterpret_cast<IViewport*>(_viewport); }
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

		void ForceCenter() { _angles[PL_PITCH] = 0; }	// Forces the pitch to become centered.
	
		mleaf_t *leaf, *oldleaf;

	protected:
	private:
		Viewport *_viewport;

		void SetFrustum(float fovx, float fovy);

		void SimulateBob();
		void SimulateRoll();
		void SimulatePunch();

#ifdef CAMERA_LEGACY
		void SimulateViewEntity();
		void SimulateParentEntity();

		void DrawViewEntity();
		void DrawParentEntity();

		// TODO: Move these over into a seperate class,
		// they don't really make sense here for the camera codebase
		ClientEntity_t *_viewmodel, *_parententity;
#endif

		bool			bobcam;
		plVector2f_t	bobamount;

		mplane_t _frustum[4];

		glm::mat4x4 _matrix_view, _matrix_projection;

		plVector3f_t _forward, _right, _up;
		plVector3f_t punchangles[2];		//johnfitz -- copied from cl.punchangle.  0 is current, 1 is previous value. never the same unless map just loaded
		plVector3f_t _angles, _position;

		float _fov, _fovx, _fovy;

		float _height;	// Additional height of the camera.
	};

	class CameraManager : public XManager
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

		void PrintPositions();
	protected:
	private:
		std::vector<Camera*> _cameras;

		Camera *_current_camera;
	};
}

extern core::CameraManager *g_cameramanager;

#else

void CameraManager_Simulate(void);

#ifdef CAMERA_LEGACY
void CameraManager_SetParentEntity(EngineCamera *camera, ClientEntity_t *parent);
void CameraManager_SetViewEntity(EngineCamera *camera, ClientEntity_t *child);

ClientEntity_t *CameraManager_GetParentEntity(EngineCamera *camera);
ClientEntity_t *CameraManager_GetViewEntity(EngineCamera *camera);
#endif

EngineCamera *CameraManager_GetCurrentCamera(void);

#endif

PL_EXTERN_C

EngineCamera *CameraManager_GetPrimaryCamera(void);

void Camera_SetPosition(EngineCamera *camera, plVector3f_t position);
void Camera_SetAngles(EngineCamera *camera, plVector3f_t angles);

PL_EXTERN_C_END