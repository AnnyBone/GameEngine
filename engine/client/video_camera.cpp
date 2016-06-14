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

#include "engine_base.h"

#include "video.h"
#include "effect_sprite.h"
#include "EngineGame.h"

using namespace Core;

/*
Camera Manager

This doesn't explicitly manage your cameras for you beyond
drawing and simulating them automatically. Otherwise the
expectation is you would create a camera, and then set
that up depending on its needs yourself.
*/

ConsoleVariable_t cv_camera_forwardcycle = { "camera_forwardcycle", "0.43", true };
ConsoleVariable_t cv_camera_sidecycle = { "camera_sidecycle", "0.86", true };
ConsoleVariable_t cv_camera_upcycle = { "camera_upcycle", "0.45", true };
ConsoleVariable_t cv_camera_bob = { "camera_bob", "0.002", true };

ConsoleVariable_t cv_camera_modellag = { "camera_modellag", "0.2", true };
ConsoleVariable_t cv_camera_modelposition = { "camera_modelposition", "1", true };

ConsoleVariable_t cv_camera_rollangle = { "camera_rollangle", "2.0", true };
ConsoleVariable_t cv_camera_rollspeed = { "camera_rollspeed", "200", true };

ConsoleVariable_t cv_camera_nearclip = { "camera_nearclip", "4", true };
ConsoleVariable_t cv_camera_farclip = { "camera_farclip", "16384", true };

ConsoleVariable_t cv_camera_punch = { "camera_punch", "1", true };

CameraManager *g_cameramanager;

CameraManager::CameraManager()
{
	Con_Printf("Initializing Camera Manager...\n");

	// Reserve for up to four cameras.
	_cameras.reserve(4);

	Cvar_RegisterVariable(&cv_camera_bob, NULL);
	Cvar_RegisterVariable(&cv_camera_forwardcycle, NULL);
	Cvar_RegisterVariable(&cv_camera_sidecycle, NULL);
	Cvar_RegisterVariable(&cv_camera_upcycle, NULL);

	Cvar_RegisterVariable(&cv_camera_modellag, NULL);
	Cvar_RegisterVariable(&cv_camera_modelposition, NULL);
	Cvar_RegisterVariable(&cv_camera_nearclip, NULL);
	Cvar_RegisterVariable(&cv_camera_farclip, NULL);
	Cvar_RegisterVariable(&cv_camera_punch, NULL);
}

/* Camera Creation */

Camera *CameraManager::CreateCamera()
{
	Camera *camera = new Camera();
	if (!camera)
		throw Core::Exception("Failed to allocate new camera!\n");

	memset(camera, 0, sizeof(Camera));

	_cameras.push_back(camera);

	return camera;
}

void CameraManager::DeleteCamera(Camera *camera)
{
	// Already deleted, probably.
	if (!camera) return;

	// Remove it from the list.
	for (auto iterator = _cameras.begin(); iterator != _cameras.end(); iterator++)
		if (camera == *iterator)
		{
			_cameras.erase(iterator);
			break;
		}

	delete camera;
}

/**/

void CameraManager::SetCurrentCamera(Camera *_camera)
{
	if (!_camera) return;
	_current_camera = _camera;
}

/**/

void CameraManager::Draw()
{
	for (unsigned int i = 0; i < _cameras.size(); i++)
	{
		SetCurrentCamera(_cameras[i]);

		_cameras[i]->Draw();
	}
}

void CameraManager::Simulate()
{
	for (unsigned int i = 0; i < _cameras.size(); i++)
	{
		SetCurrentCamera(_cameras[i]);

		_cameras[i]->Simulate();
	}
}

/*
	Camera
*/

Camera::Camera()
{
	plVectorClear(position);
	plVectorClear(angles);
	plVectorClear(bobamount);

	plAngleVectors(angles, _forward, _right, _up);

	SetFOV(90);
}

Camera::Camera(Viewport *viewport) : Camera()
{
	SetViewport(viewport);
}

#ifdef CAMERA_LEGACY
void Camera::DrawViewEntity()
{
	if (!r_drawviewmodel.bValue || !viewmodel || !viewmodel->model)
		return;

#ifdef VL_MODE_OPENGL
	// Hack the depth range to prevent view model from poking into walls.
	glDepthRange(0, 0.3);
#endif

	Draw_Entity(viewmodel);

#ifdef VL_MODE_OPENGL
	glDepthRange(0, 1);
#endif
}
#endif

void Camera::Draw()
{
#if defined (VL_MODE_OPENGL)
	glRotatef(-90, 1, 0, 0);	// Put Z going up.
	glRotatef(90, 0, 0, 1);		// Put Z going up.

	glRotatef(-angles[2], 1, 0, 0);
	glRotatef(-angles[0], 0, 1, 0);
	glRotatef(-angles[1], 0, 0, 1);

	glTranslatef(-position[0], -position[1], -position[2]);

	glGetFloatv(GL_MODELVIEW_MATRIX, r_world_matrix);
#endif

	Fog_SetupFrame();	// todo, really necessary to call this at the start of every draw call!?

	float fovxx = fovx, fovyy = fovy;
	if (cl.worldmodel)
	{
		// Current view leaf.
		oldleaf = leaf;
		leaf = Mod_PointInLeaf(position, cl.worldmodel);

		if (r_waterwarp.value)
		{
			int contents = Mod_PointInLeaf(position, cl.worldmodel)->contents;
			if (contents == BSP_CONTENTS_WATER || contents == BSP_CONTENTS_SLIME || contents == BSP_CONTENTS_LAVA)
			{
				//variance is a percentage of width, where width = 2 * tan(fov / 2) otherwise the effect is too dramatic at high FOV and too subtle at low FOV.  what a mess!
				fovxx = atanf(tan(PL_DEG2RAD(fovx) / 2) * (0.97 + sin(cl.time * 1.5) * 0.03)) * 2 / PL_PI_DIV180;
				fovyy = atanf(tan(PL_DEG2RAD(fovy) / 2) * (1.03 - sin(cl.time * 1.5) * 0.03)) * 2 / PL_PI_DIV180;
			}
		}

		R_MarkSurfaces();
		R_CullSurfaces();
	}

	SetFrustum(fovxx, fovyy);

	if(gl_cull.value)	vlEnable(VL_CAPABILITY_CULL_FACE);
	else				vlDisable(VL_CAPABILITY_CULL_FACE);

	//vlEnableCapability(VL_CAPABILITY_DEPTH_TEST);	// is this needed??

	//Fog_EnableGFog();
	Sky_Draw();
	World_Draw();
	Draw::Shadows();
	Draw::Entities(false);
	World_DrawWater();
	Draw::Entities(true);
	// todo, particles
	if (g_spritemanager) g_spritemanager->Draw();
	Light_Draw();
	//Fog_DisableGFog();
	if ((cl.maxclients <= 1) && !bIsDedicated) Game->Server_Draw();
	DrawViewEntity();
	Draw::BoundingBoxes();

#if 0
	//johnfitz -- cheat-protect some draw modes
	r_drawflat_cheatsafe = r_fullbright_cheatsafe = r_lightmap_cheatsafe = false;
	r_drawworld_cheatsafe = true;
	if (cl.maxclients == 1)
	{
		if (!r_drawworld.value)
			r_drawworld_cheatsafe = false;

		if (r_drawflat.value)
			r_drawflat_cheatsafe = true;
		else if (r_fullbright.value || !cl.worldmodel->lightdata)
			r_fullbright_cheatsafe = true;
		else if (r_lightmap.value)
			r_lightmap_cheatsafe = true;
	}
	//johnfitz
#endif
}

/*	Calculate and add view bobbing.
*/
void Camera::SimulateBob()
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

void Camera::SimulateRoll()
{
	float side = Math_DotProduct(cl.velocity, _right);
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
	if(cl.bIsPaused)
		return;

	SimulateFrustum();

	SimulateBob();
	SimulateRoll();

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
					punch[i] = std::fminf(punch[i] + delta, punchangles[0][i]);
				else if (delta < 0)
					punch[i] = std::fmaxf(punch[i] + delta, punchangles[0][i]);
			}

		plVectorAdd3fv(angles, punch, angles);
	}

	plAngleVectors(angles, _forward, _right, _up);

	// Simulate camera-specific effects...
	g_spritemanager->Simulate();
}

#ifdef CAMERA_LEGACY
void Camera::SetParentEntity(ClientEntity_t *_parent)
{
	if (!_parent)
	{
		// Clear it out, not an issue since we
		// want to support switching between these.
		parententity = nullptr;
		return;
	}

	parententity = _parent;

	SetPosition(parententity->origin);
	position[2] += height;
}

void Camera::SetViewEntity(ClientEntity_t *_child)
{
	if (!_child)
	{
		// Clear it out, not an issue since we
		// want to support switching between these.
		viewmodel = nullptr;
		return;
	}

	viewmodel = _child;
}

void Camera::SimulateViewEntity()
{
	// View is the weapon model (only visible from inside body).
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
			(_up[i] * bobamount[0] * 0.2f) +
			(_right[i] * bobamount[1] * 0.3f);

		viewmodel->origin[2] += bobamount[0];
	}

	// Apple the view model drift.
	if (host_frametime != 0)
	{
		static plVector3f_t lastforward;
		plVector3f_t difference;
		plVectorSubtract3fv(_forward, lastforward, difference);

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
		viewmodel->origin[i] += _forward[i] + offset * _right[i] + _up[i];
}

void Camera::SimulateParentEntity()
{
	// Parent is the player model (visible when out of body).
	if (!parententity)
		return;

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

/*	Frustum	*/

void Camera::SetFOV(float fov)
{
	// Clamp the FOV to ensure we don't
	// get anything rediculous.
	Math_Clamp(10, fov, 170);
	
	_fov = fov;
	_fovx = _fov;

	unsigned int width = 640, height = 480;
	if (_viewport)
	{
		width	= _viewport->GetWidth();
		height	= _viewport->GetHeight();
	}
	
	// Taken from CalcFovy.
	float x = width * std::tanf(fovx / 360.0f * PL_PI);
	float a = std::atanf(height / x);
	a *= 360.0f / PL_PI;

	_fovy = a;
}

void Camera::SetFrustum(float fovx, float fovy)
{
	_fovx = fovx; _fovy = fovy;

	float xmax = cv_camera_nearclip.value * tanf(_fovx * PL_PI / 360);
	float ymax = cv_camera_nearclip.value * tanf(_fovy * PL_PI / 360);
#if defined (VL_MODE_OPENGL)
	glFrustumf(-xmax, xmax, -ymax, ymax, cv_camera_nearclip.value, cv_camera_farclip.value);
#endif
}

extern "C" {
	int SignbitsForPlane(mplane_t *out);
}

void Camera::SimulateFrustum()
{
	// Update the frustum.
	plTurnVector(frustum[0].normal, position, _right, fovx / 2 - 90);	// Left plane
	plTurnVector(frustum[1].normal, position, _right, 90 - fovx / 2);	// Right plane
	plTurnVector(frustum[2].normal, position, _up, 90 - fovy / 2);		// Bottom plane
	plTurnVector(frustum[3].normal, position, _up, fovy / 2 - 90);		// Top plane

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

void Camera::TracePosition()
{
	plVector3f_t v;
	plVectorScalef(_forward, 8192.0f, v);

	plVector3f_t w;
	TraceLine(position, v, w);

	if (plLengthf(w) == 0)
	{
		Con_Printf("Didn't hit anything!\n");
		return;
	}
	
	Con_Printf("TRACE POSITION : %i %i %i\n",
		(int)w[0],
		(int)w[1],
		(int)w[2]);
}