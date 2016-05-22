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

#define	CAMERA_LEGACY	// Camera automatically follows parent entity.

namespace Core
{
	namespace Client
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
#endif

			bool			bobcam;
			plVector2f_t	bobamount;

			mplane_t frustum[4];

			plVector3f_t forward, right, up;
			plVector3f_t punchangles[2];		//johnfitz -- copied from cl.punchangle.  0 is current, 1 is previous value. never the same unless map just loaded
			plVector3f_t angles, position;

			float height;	// Additional height of the camera.

#ifdef CAMERA_LEGACY
			// TODO: Move these over into a seperate class,
			// they don't really make sense here for the camera codebase
			ClientEntity_t *viewmodel, *parententity;
#endif
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

		CameraManager *g_cameramanager;
	}
}

using namespace Core::Client;

/*
	Camera Manager
*/

CameraManager::CameraManager()
{
	cameras.reserve(5);

	Cvar_RegisterVariable(&cv_camera_bob, NULL);
	Cvar_RegisterVariable(&cv_camera_forwardcycle, NULL);
	Cvar_RegisterVariable(&cv_camera_sidecycle, NULL);
	Cvar_RegisterVariable(&cv_camera_upcycle, NULL);

	Cvar_RegisterVariable(&cv_camera_modellag, NULL);
	Cvar_RegisterVariable(&cv_camera_modelposition, NULL);

	Cvar_RegisterVariable(&cv_camera_punch, NULL);
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

// C Interface

void CameraManager_Draw(void)
{
	if (g_cameramanager)
		g_cameramanager->Draw();
}

void CameraManager_Simulate(void)
{
	if (g_cameramanager)
		g_cameramanager->Simulate();
}

/*
	Camera
*/

Camera::Camera() : 
#ifdef CAMERA_LEGACY
	parententity(nullptr),
	viewmodel(nullptr),
#endif
	bobcam(false)
{
	plVectorClear(position);
	plVectorClear(angles);
	plVectorClear(bobamount);

	plAngleVectors(angles, forward, right, up);
}

void Camera::Draw()
{
}

/*	Calculate and add view bobbing.
*/
void Camera::CalculateBob()
{
	if (!bobcam) return;

	plVector2f_t cycle;

	// Forward cycle
	cycle[0] = (cl.time - (int)(cl.time / cv_camera_forwardcycle.value) * cv_camera_forwardcycle.value) / cv_camera_forwardcycle.value;
	if (cycle[0] < cv_camera_upcycle.value)
		cycle[0] = PL_PI * cycle[0] / cv_camera_upcycle.value;
	else
		cycle[0] = PL_PI + PL_PI * (cycle[0] - cv_camera_upcycle.value) / (1.0f - cv_camera_upcycle.value);

	// Side cycle
	cycle[1] = (cl.time - (int)(cl.time / cv_camera_sidecycle.value) * cv_camera_sidecycle.value) / cv_camera_sidecycle.value;
	if (cycle[1] < cv_camera_upcycle.value)
		cycle[1] = PL_PI * cycle[1] / cv_camera_upcycle.value;
	else
		cycle[1] = PL_PI + PL_PI * (cycle[1] - cv_camera_upcycle.value) / (1.0f - cv_camera_upcycle.value);

	plVector2f_t bob;

	// Bob is proportional to velocity in the xy plane
	// (don't count Z, or jumping messes it up)
	bob[0] = sqrtf(
		cl.velocity[0] * cl.velocity[0] +
		cl.velocity[1] * cl.velocity[1]) *
		cv_camera_bob.value;
	bob[1] = sqrtf(
		cl.velocity[0] * cl.velocity[0] +
		cl.velocity[1] * cl.velocity[1]) *
		cv_camera_sidecycle.value;

	for (int i = 0; i < 2; i++)
	{
		bob[i] *= bob[i] * 0.7f * sinf(cycle[i]);
		if (bob[i] > 4.0f)
			bob[i] = 4.0f;
		else if (bob[i] < -7.0f)
			bob[i] = -7.0f;
	}

	position[2] += bob[0];
	
	// Store the bob amount, so it can be used later.
	bobamount[0] = bob[0];
	bobamount[1] = bob[1];
}

void Camera::CalculateRoll()
{
	float side = Math_DotProduct(cl.velocity, right);
	float sign = side < 0 ? -1 : 1;
	side = fabsf(side);

	float value = cv_camera_rollangle.value;
	if (side < cv_camera_rollspeed.value)
		side *= value / cv_camera_rollspeed.value;
	else
		side = value;

	side *= sign;
	angles[PL_ROLL] += side;

#if 0 // TODO: Move into seperate class?? Game specific...
	static double dmg_time = 0;
	if (dmg_time = 0)
	{
		angles[PL_ROLL] += dmg_time /
		angles[PL_PITCH] += dmg_time
	}

	if (cl.stats[STAT_HEALTH] <= 0)
		angles[PL_ROLL] = 80.0f;
#endif
}

void Camera::Simulate()
{
	CalculateBob();
	CalculateRoll();

	// Add height (needs to be done after bob).
	position[2] += height;

	/*	
	Never let it sit exactly on a node line, because a water plane can
	dissapear when viewed with the eye exactly on it.

	The server protocol only specifies to 1/16 pixel, so add 1/32 in each axis.
	*/
	plVectorAddf(position, 1.0f / 32.0f, position);

#ifdef CAMERA_LEGACY
	SimulateParentEntity();
	SimulateViewEntity();
#endif

	// Apply camera punch, if it's enabled.
	if (cv_camera_punch.value)
	{
		static MathVector3f_t punch = { 0, 0, 0 };
		for (int i = 0; i < 3; i++)
			if (pl_origin3f[i] != punchangles[0][i])
			{
				// Speed determined by how far we need to lerp in 1/10th of a second.
				float delta = (punchangles[0][i] - punchangles[1][i]) * host_frametime * 10.0f;
				if (delta > 0)
					punch[i] = Math_Min(punch[i] + delta, punchangles[0][i]);
				else if (delta < 0)
					punch[i] = Math_Max(punch[i] + delta, punchangles[0][i]);
			}

		plVectorAdd3fv(r_refdef.viewangles, punch, r_refdef.viewangles);
	}

	plAngleVectors(angles, forward, right, up);
}

#ifdef CAMERA_LEGACY
void Camera::SimulateViewEntity()
{
	// View is the weapon model (only visible from inside body).
	viewmodel = &cl.viewent;
	if (!viewmodel)
		return;

	viewmodel->model = cl.model_precache[cl.stats[STAT_WEAPON]];
	viewmodel->frame = cl.stats[STAT_WEAPONFRAME];

	if (!viewmodel->model)
		return;

	// Apply view model angles.
	plVector3f_t oldangles;
	plVectorCopy(viewmodel->angles, oldangles);
	plVectorCopy(angles, viewmodel->angles);
	
	// Update view model origin.
	plVectorCopy(parententity->origin, viewmodel->origin);
	viewmodel->origin[2] += height;

	// Apply view model bob.
	if (bobcam)
	{
		for (int i = 0; i < 3; i++)
			viewmodel->origin[i] +=
			(up[i] * bobamount[0] * 0.2f) +
			(right[i] * bobamount[1] * 0.3f);

		viewmodel->origin[2] += bobamount[0];
	}

	// Apple the view model drift.
	if (host_frametime != 0)
	{
		static plVector3f_t lastforward;
		plVector3f_t difference;
		plVectorSubtract3fv(forward, lastforward, difference);

		float speed = 3.0f, scale = 0;
		float diff = plLengthf(difference);
		if ((diff > cv_camera_modellag.value) && (cv_camera_modellag.value > 0))
			speed *= scale = diff / cv_camera_modellag.value;

		for (int i = 0; i < 3; i++)
			lastforward[i] += difference[i] * (speed * host_frametime);

		plVectorNormalize(lastforward);

		for (int i = 0; i < 3; i++)
		{
			viewmodel->origin[i] += (difference[i] * -1) * 5;

			// TODO: wait, we're doing this in this loop!? Okay...
			// This is probably a mistake but I'll wait before I sort this.
			viewmodel->angles[ROLL] += difference[YAW];
		}
	}

	// Apply some slight movement.
	viewmodel->angles[PL_PITCH] = -(angles[PL_PITCH] - (sinf(cl.time * 1.5f) * 0.2f));
	viewmodel->angles[PL_ROLL] = -(angles[PL_ROLL] - (sinf(cl.time * 1.5f) * 0.2f));

	// Finally, offset!
	float offset = 0;
	if (cv_camera_modelposition.iValue == 1)
		offset = -5;
	else if (cv_camera_modelposition.iValue == 2)
		offset = 5;
	else return;

	for (int i = 0; i < 3; i++)
		viewmodel->origin[i] += forward[i] + offset * right[i] + up[i];
}

void Camera::SimulateParentEntity()
{
	// Parent is the player model (visible when out of body).
	parententity = &cl_entities[cl.viewentity];

	// Transform the view offset by the model's matrix to get the offset from
	// model origin for the view.
	parententity->angles[YAW]		= angles[YAW];
	parententity->angles[PITCH]		= -angles[PITCH];
	parententity->angles[ROLL]		= -angles[ROLL];

	// Refresh view position.
	SetPosition(parententity->origin);

	// Offsets
	angles[PL_PITCH]	= -parententity->angles[PL_PITCH];	// Because entity pitches are actually backward.
	angles[PL_YAW]		= parententity->angles[PL_YAW];
	angles[PL_ROLL]		= parententity->angles[PL_ROLL];

	/*
	Absolutely bound refresh reletive to entity clipping hull
	so the view can never be inside a solid wall.
	*/
	if (position[0] < (parententity->origin[0] - 14.0f))
		position[0] = parententity->origin[0] - 14.0f;
	else if (position[0] > (parententity->origin[0] + 14.0f))
		position[0] = parententity->origin[0] + 14.0f;

	if (position[1] < (parententity->origin[1] - 14.0f))
		position[1] = parententity->origin[1] - 14.0f;
	else if (position[1] > (parententity->origin[1] + 14.0f))
		position[1] = parententity->origin[1] + 14.0f;

	if (position[2] < (parententity->origin[2] - 22.0f))
		position[2] = parententity->origin[2] - 22.0f;
	else if (position[2] > (parententity->origin[2] + 30.0f))
		position[2] = parententity->origin[2] + 30.0f;
}
#endif

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
	Con_Printf("CAMERA ANGLES : %i %i %i\n",
		(int)angles[PL_PITCH],
		(int)angles[PL_YAW],
		(int)angles[PL_ROLL]);
}

// Position

void Camera::SetPosition(float x, float y, float z)
{
	position[0] = x;
	position[1] = y;
	position[2] = z;
}

void Camera::SetPosition(plVector3f_t _position)
{
	plVectorCopy(_position, position);
}

void Camera::PrintPosition()
{
	Con_Printf("CAMERA POSITION : %i %i %i\n",
		(int)position[0],
		(int)position[1],
		(int)position[2]);
}