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

using namespace core;

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
ConsoleVariable_t cv_camera_drawfrustum = { "camera_drawfrustum", "0", false, false, "Draws the view frustum." };

ConsoleVariable_t cv_camera_maxpitch = { "camera_maxpitch", "90" };
ConsoleVariable_t cv_camera_minpitch = { "camera_minpitch", "-90" };
ConsoleVariable_t cv_camera_maxroll = { "camera_maxroll", "50" };
ConsoleVariable_t cv_camera_minroll = { "camera_minroll", "-50" };

ConsoleVariable_t cv_camera_punch = { "camera_punch", "1", true };

ConsoleVariable_t cv_camera_fov = { "camera_fov", "90", true };

CameraManager *g_cameramanager;

// Commands //////////////////////////////////////////////

void _CameraManager_PrintPositions(void)
{
	g_cameramanager->PrintPositions();
}

//////////////////////////////////////////////////////////

CameraManager::CameraManager()
{
	Con_Printf("Initializing Camera Manager...\n");

	Cvar_RegisterVariable(&cv_camera_bob, NULL);
	Cvar_RegisterVariable(&cv_camera_forwardcycle, NULL);
	Cvar_RegisterVariable(&cv_camera_sidecycle, NULL);
	Cvar_RegisterVariable(&cv_camera_upcycle, NULL);

	Cvar_RegisterVariable(&cv_camera_modellag, NULL);
	Cvar_RegisterVariable(&cv_camera_modelposition, NULL);

	Cvar_RegisterVariable(&cv_camera_nearclip, NULL);
	Cvar_RegisterVariable(&cv_camera_farclip, NULL);
	Cvar_RegisterVariable(&cv_camera_drawfrustum, NULL);

	Cvar_RegisterVariable(&cv_camera_maxpitch, NULL);
	Cvar_RegisterVariable(&cv_camera_minpitch, NULL);
	Cvar_RegisterVariable(&cv_camera_maxroll, NULL);
	Cvar_RegisterVariable(&cv_camera_minroll, NULL);

	Cvar_RegisterVariable(&cv_camera_punch, NULL);
	Cvar_RegisterVariable(&cv_camera_fov, NULL);

	Cmd_AddCommand("cm_printpos", _CameraManager_PrintPositions);
}

CameraManager::~CameraManager()
{
	for (auto camera = _cameras.begin(); camera != _cameras.end(); ++camera)
		delete (*camera);

	_cameras.clear();
}

/* Camera Creation */

Camera *CameraManager::CreateCamera()
{
	Camera *camera = new Camera();
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
	if (!_camera)
	{
		Con_Warning("Attempted to set invalid camera as current!\n");
		return;
	}

	_current_camera = _camera;
}

/**/

void CameraManager::Draw()
{
	for (unsigned int i = 0; i < _cameras.size(); i++)
	{
		SetCurrentCamera(_cameras[i]);

		// Cameras should only draw if a viewport is available.
		if (_cameras[i]->GetViewport()) _cameras[i]->Draw();
	}
}

void CameraManager::Simulate()
{
	if (cl.paused)
		return;

	for (unsigned int i = 0; i < _cameras.size(); i++)
	{
		SetCurrentCamera(_cameras[i]);

		_cameras[i]->Simulate();
	}
}

// Prints the position of all the currently created cameras.
void CameraManager::PrintPositions()
{
	for (unsigned int i = 0; i < _cameras.size(); i++)
	{
		Con_Printf("\nCAMERA %i\n", i);
		_cameras[i]->PrintPosition();
	}
}

/*	Camera Manager C Interface	*/

extern "C" void CameraManager_Simulate(void)
{
	g_cameramanager->Simulate();
}

extern "C" EngineCamera *CameraManager_GetCurrentCamera(void)
{
	return g_cameramanager->GetCurrentCamera();
}

EngineCamera *CameraManager_GetPrimaryCamera(void)
{
	Viewport *view = GetPrimaryViewport();
	if (!view)
	{
		Con_Warning("Failed to get primary viewport!\n");
		return nullptr;
	}

	EngineCamera *camera = dynamic_cast<EngineCamera*>(view->GetCamera());
	if (!camera)
	{
		Con_Warning("Failed to get primary camera!\n");
		return nullptr;
	}

	return camera;
}

#ifdef CAMERA_LEGACY

extern "C" void CameraManager_SetParentEntity(EngineCamera *camera, ClientEntity_t *parent)
{
	if (!camera) { Sys_Error("Received invalid camera when attempting to set parent entity!\n"); return; } // Keep compiler happy...
	camera->SetParentEntity(parent);
}

extern "C" void CameraManager_SetViewEntity(EngineCamera *camera, ClientEntity_t *child)
{
	if (!camera) { Sys_Error("Received invalid camera when attempting to set view entity!\n"); return; } // Keep compiler happy...
	camera->SetViewEntity(child);
}

extern "C" ClientEntity_t *CameraManager_GetParentEntity(EngineCamera *camera)
{
	if (!camera) { Sys_Error("Received invalid camera when attempting to get parent entity!\n"); return NULL; } // Keep compiler happy...
	return camera->GetParentEntity();
}

extern "C" ClientEntity_t *CameraManager_GetViewEntity(EngineCamera *camera)
{
	if (!camera) { Sys_Error("Received invalid camera when attempting to get view entity!\n"); return NULL; } // Keep compiler happy...
	return camera->GetParentEntity();
}

#endif

/*	Camera	*/

Camera::Camera() :
	_viewport(nullptr),
	_viewmodel(nullptr),
	_parententity(nullptr),

	_height(0)
{
	plVectorClear(_position);
	plVectorClear(_angles);
	plVectorClear(bobamount);

	plAngleVectors(_angles, _forward, _right, _up);

	SetFOV(cv_camera_fov.value);
}

Camera::Camera(Viewport *viewport) : Camera()
{
	SetViewport(viewport);
}

///////////////////////////////////////////////////////////////

void Camera::SetupProjectionMatrix()
{
#if defined (VL_MODE_OPENGL)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	unsigned int width = 640, height = 480;
	if (_viewport)
	{
		width = _viewport->GetWidth();
		height = _viewport->GetHeight();
	}

#if 1	// Cheekily stolen from Quake 2.
	double ymin, ymax;
	ymax = cv_camera_nearclip.value * tan(_fovy * PL_PI / 360);
	ymin = -ymax;

	double aspect = width / height;
	double xmin, xmax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;

	glFrustum(xmin, xmax, ymin, ymax, (double)cv_camera_nearclip.value, (double)cv_camera_farclip.value);
#elif 2
	float xmax = cv_camera_nearclip.value * tanf(_fovx * PL_PI / 360);
	float ymax = cv_camera_nearclip.value * tanf(_fovy * PL_PI / 360);
	glFrustum(-xmax, xmax, -ymax, ymax, cv_camera_nearclip.value, cv_camera_farclip.value);
#else
	float aspect = (float)width / height;
	glm::mat4 matrix_projection = glm::perspective(_fovy, aspect, cv_camera_nearclip.value, cv_camera_farclip.value);
	glLoadMatrixf(glm::value_ptr(matrix_projection));
#endif
#endif
}

void Camera::SetupViewMatrix()
{
#if defined (VL_MODE_OPENGL)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotatef(-90, 1, 0, 0);	// Z going up.
	glRotatef(90, 0, 0, 1);		// Z going up.

	glRotatef(-_angles[2], 1, 0, 0);
	glRotatef(-_angles[0], 0, 1, 0);
	glRotatef(-_angles[1], 0, 0, 1);

	glTranslatef(-_position[0], -_position[1], -_position[2]);

#if 0
	// todo, not needed?
	glGetFloatv(GL_MODELVIEW_MATRIX, r_world_matrix);
#endif
#endif
}

///////////////////////////////////////////////////////////////

#ifdef CAMERA_LEGACY
void Camera::DrawViewEntity()
{
	if (!r_drawviewmodel.bValue || !_viewmodel || !_viewmodel->model)
		return;

#ifdef VL_MODE_OPENGL
	// Hack the depth range to prevent view model from poking into walls.
	glDepthRange(0, 0.3);
#endif

	draw::Entity(_viewmodel);

#ifdef VL_MODE_OPENGL
	glDepthRange(0, 1);
#endif
}
#endif

void Camera::Draw()
{
	//float fovxx = _fovx, fovyy = _fovy;
	if (cl.worldmodel) 
	{
		// Current view leaf.
		oldleaf = leaf;
		leaf = Mod_PointInLeaf(_position, cl.worldmodel);

		if (r_waterwarp.value) 
		{
			int contents = Mod_PointInLeaf(_position, cl.worldmodel)->contents;
			if (contents == BSP_CONTENTS_WATER || contents == BSP_CONTENTS_SLIME || contents == BSP_CONTENTS_LAVA)
			{
#if 0
				//variance is a percentage of width, where width = 2 * tan(fov / 2) otherwise the effect is too dramatic at high FOV and too subtle at low FOV.  what a mess!
				fovxx = atanf(tan(PL_DEG2RAD(_fovx) / 2) * (0.97 + sin(cl.time * 1.5) * 0.03)) * 2 / PL_PI_DIV180;
				fovyy = atanf(tan(PL_DEG2RAD(_fovy) / 2) * (1.03 - sin(cl.time * 1.5) * 0.03)) * 2 / PL_PI_DIV180;
#endif
			}
		}
		
		World_MarkSurfaces();
		World_CullSurfaces();
	}

	SetupProjectionMatrix();
	SetupViewMatrix();

	//Fog_SetupFrame();	// todo, really necessary to call this at the start of every draw call!?

	if(gl_cull.value)	plEnableGraphicsStates(VL_CAPABILITY_CULL_FACE);
	else				plDisableGraphicsStates(VL_CAPABILITY_CULL_FACE);

	plEnableGraphicsStates(VL_CAPABILITY_DEPTH_TEST);	// is this needed??

	//Fog_EnableGFog();
	Sky_Draw();
	World_Draw();
	draw::Shadows();
	draw::Entities(false);
	World_DrawWater();
	draw::Entities(true);
	// todo, particles
	if (g_spritemanager) g_spritemanager->Draw();
	Light_Draw();
	//Fog_DisableGFog();
	DrawViewEntity();
	draw::BoundingBoxes();

	if ((cl.maxclients <= 1) && !bIsDedicated)
		Game->Server_Draw();

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

#ifdef CAMERA_LEGACY
void Camera::SetParentEntity(ClientEntity_t *parent)
{
	// Clear it out, not an issue since we
	// want to support switching between these.
	_parententity = nullptr;

	if (!parent)
	{
		Con_Warning("Failed to set camera parent entity!\n");
		return;
	}
	_parententity = parent;

	SetPosition(_parententity->origin);
	_position[2] += _height;
}

void Camera::SetViewEntity(ClientEntity_t *_child)
{
	// Clear it out, not an issue since we
	// want to support switching between these.
	_viewmodel = nullptr;

	if (!_child)
	{
		Con_Warning("Failed to set camera child entity!\n");
		return;
	}
	_viewmodel = _child;
}

void Camera::SimulateViewEntity()
{
	// View is the weapon model (only visible from inside body).
	if (!_viewmodel)
		return;

	_viewmodel->model = cl.model_precache[cl.stats[STAT_WEAPON]];
	_viewmodel->frame = cl.stats[STAT_WEAPONFRAME];

	if (!_viewmodel->model)
		return;

	// Apply view model angles.
	PLVector3f oldangles = { 0 };
	plVectorCopy(_viewmodel->angles, oldangles);
	plVectorCopy(_angles, _viewmodel->angles);

	// Update view model origin.
	plVectorCopy(_parententity->origin, _viewmodel->origin);
	_viewmodel->origin[2] += _height;

	// Apply view model bob.
	if (bobcam)
	{
		for (int i = 0; i < 3; i++)
			_viewmodel->origin[i] +=
			(_up[i] * bobamount[0] * 0.2f) +
			(_right[i] * bobamount[1] * 0.3f);

		_viewmodel->origin[2] += bobamount[0];
	}

	// Apple the view model drift.
	if (host_frametime != 0)
	{
		static PLVector3f lastforward;
		PLVector3f difference = { 0, 0, 0 };
		plVectorSubtract3fv(_forward, lastforward, difference);

		float speed = 3.0f;
		float diff = plLengthf(difference);
		if ((diff > cv_camera_modellag.value) && (cv_camera_modellag.value > 0))
			speed *= diff / cv_camera_modellag.value;

		for (int i = 0; i < 3; i++)
			lastforward[i] += difference[i] * (speed * host_frametime);

		plVectorNormalize(lastforward);

		for (int i = 0; i < 3; i++)
		{
			_viewmodel->origin[i] += (difference[i] * -1) * 5;

			// TODO: wait, we're doing this in this loop!? Okay...
			// This is probably a mistake but I'll wait before I sort this.
			_viewmodel->angles[ROLL] += difference[YAW];
		}
	}

	// Apply some slight movement.
	_viewmodel->angles[PL_PITCH] = -(_angles[PL_PITCH] - (sinf((float)cl.time * 1.5f) * 0.2f));
	_viewmodel->angles[PL_ROLL] = -(_angles[PL_ROLL] - (sinf((float)cl.time * 1.5f) * 0.2f));

	// Finally, offset!
	float offset = 0;
	if (cv_camera_modelposition.iValue == 1)		offset = -5;
	else if (cv_camera_modelposition.iValue == 2)	offset = 5;
	else return;

	for (int i = 0; i < 3; i++)
		_viewmodel->origin[i] += _forward[i] + offset * _right[i] + _up[i];
}

void Camera::SimulateParentEntity()
{
	// Parent is the player model (visible when out of body).
	if (!_parententity)
		return;

	// Transform the view offset by the model's matrix to get the offset from
	// model origin for the view.
	_parententity->angles[YAW] = _angles[YAW];
	_parententity->angles[PITCH] = -_angles[PITCH];
	_parententity->angles[ROLL] = -_angles[ROLL];

	// Refresh view position.
	SetPosition(_parententity->origin);

	// Offsets
	_angles[PL_PITCH] = -_parententity->angles[PL_PITCH];	// Because entity pitches are actually backward.
	_angles[PL_YAW] = _parententity->angles[PL_YAW];
	_angles[PL_ROLL] = _parententity->angles[PL_ROLL];

	/*
	Absolutely bound refresh reletive to entity clipping hull
	so the view can never be inside a solid wall.
	*/
	if (_position[0] < (_parententity->origin[0] - 14.0f))
		_position[0] = _parententity->origin[0] - 14.0f;
	else if (_position[0] > (_parententity->origin[0] + 14.0f))
		_position[0] = _parententity->origin[0] + 14.0f;

	if (_position[1] < (_parententity->origin[1] - 14.0f))
		_position[1] = _parententity->origin[1] - 14.0f;
	else if (_position[1] > (_parententity->origin[1] + 14.0f))
		_position[1] = _parententity->origin[1] + 14.0f;

	if (_position[2] < (_parententity->origin[2] - 22.0f))
		_position[2] = _parententity->origin[2] - 22.0f;
	else if (_position[2] > (_parententity->origin[2] + 30.0f))
		_position[2] = _parententity->origin[2] + 30.0f;
}
#endif

/*	Calculate and add view bobbing.
*/
void Camera::SimulateBob()
{
	if (!bobcam) return;

	PLVector3f cycle = { 0, 0, 0 };

	// Forward cycle
	cycle[0] = ((float)cl.time - (int)(cl.time / cv_camera_forwardcycle.value) * cv_camera_forwardcycle.value) / cv_camera_forwardcycle.value;
	if (cycle[0] < cv_camera_upcycle.value)
		cycle[0] = PL_PIf * cycle[0] / cv_camera_upcycle.value;
	else
		cycle[0] = PL_PIf + PL_PIf * (cycle[0] - cv_camera_upcycle.value) / (1.0f - cv_camera_upcycle.value);

	// Side cycle
	cycle[1] = ((float)cl.time - (int)(cl.time / cv_camera_sidecycle.value) * cv_camera_sidecycle.value) / cv_camera_sidecycle.value;
	if (cycle[1] < cv_camera_upcycle.value)
		cycle[1] = PL_PIf * cycle[1] / cv_camera_upcycle.value;
	else
		cycle[1] = PL_PIf + PL_PIf * (cycle[1] - cv_camera_upcycle.value) / (1.0f - cv_camera_upcycle.value);

	PLVector3f bob = { 0, 0, 0 };

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

	_position[2] += bob[0];
	
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
	_angles[PL_ROLL] += side;

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

void Camera::SimulatePunch()
{
	// Apply camera punch, if it's enabled.
	if (!cv_camera_punch.value)
		return;

	static PLVector3f punch = { 0, 0, 0 };
	for (int i = 0; i < 3; i++)
		if (pl_origin3f[i] != punchangles[0][i])
		{
			// Speed determined by how far we need to lerp in 1/10th of a second.
			float delta = (punchangles[0][i] - punchangles[1][i]) * (float)host_frametime * 10.0f;
			if (delta > 0)
				punch[i] = std::fminf(punch[i] + delta, punchangles[0][i]);
			else if (delta < 0)
				punch[i] = std::fmaxf(punch[i] + delta, punchangles[0][i]);
		}

	plVectorAdd3fv(_angles, punch, _angles);
}

void Camera::Simulate()
{
	SimulateFrustum();
	//SimulateBob();
	SimulateRoll();

	// Add height (needs to be done after bob).
	_position[2] += _height;

	/*	
	Never let it sit exactly on a node line, because a water plane can
	dissapear when viewed with the eye exactly on it.

	The server protocol only specifies to 1/16 pixel, so add 1/32 in each axis.
	*/
	plVectorAddf(_position, 1.0f / 32.0f, _position);

#ifdef CAMERA_LEGACY
	SimulateParentEntity();
	SimulateViewEntity();
#endif
	SimulatePunch();

	if (_angles[PL_PITCH] > cv_camera_maxpitch.value) _angles[PL_PITCH] = cv_camera_maxpitch.value;
	else if (_angles[PL_PITCH] < cv_camera_minpitch.value) _angles[PL_PITCH] = cv_camera_minpitch.value;

	if (_angles[PL_ROLL] > cv_camera_maxroll.value) _angles[PL_ROLL] = cv_camera_maxroll.value;
	else if (_angles[PL_ROLL] < cv_camera_minroll.value) _angles[PL_ROLL] = cv_camera_minroll.value;

	plAngleVectors(_angles, _forward, _right, _up);

	// Simulate camera-specific effects...
	g_spritemanager->Simulate();
}

///////////////////////////////////////////////////////////////

void Camera::Input(ClientCommand_t *cmd, plVector2i_t mpos)
{
	_angles[PL_YAW] -= m_yaw.value * mpos[PL_X];
	_angles[PL_PITCH] += m_pitch.value * mpos[PL_Y];

	plVectorCopy(_angles, cmd->viewangles);
}

/*	Frustum	*/

void Camera::SetFOV(float fov)
{
	// Clamp the FOV to ensure we don't
	// get anything rediculous.
	plClamp(10, fov, 180.0f);
	
	_fov = fov;
	_fovx = _fov;

	unsigned int width = 640, height = 480;
	if (_viewport)
	{
		width	= _viewport->GetWidth();
		height	= _viewport->GetHeight();
	}

	// Taken from CalcFovy.
	float x = width * std::tan(_fovx / 360.0f * PL_PIf);
	float a = std::atan(height / x);
	a *= 360.0f / PL_PIf;
	_fovy = a;
}

int SignbitsForPlane(mplane_t *out);

void Camera::SimulateFrustum()
{
	// Update the frustum.
	plTurnVector(_frustum[0].normal, _forward, _right, _fovx / 2 - 90);	// Left plane
	plTurnVector(_frustum[1].normal, _forward, _right, 90 - _fovx / 2);	// Right plane
	plTurnVector(_frustum[2].normal, _forward, _up, 90 - _fovy / 2);		// Bottom plane
	plTurnVector(_frustum[3].normal, _forward, _up, _fovy / 2 - 90);		// Top plane

	for (int i = 0; i < 4; i++)
	{
		_frustum[i].type = PLANE_ANYZ;
		_frustum[i].dist = Math_DotProduct(_position, _frustum[i].normal); // FIXME: shouldn't this always be zero?
		_frustum[i].signbits = (uint8_t)SignbitsForPlane(&_frustum[i]);
	}
}

bool Camera::IsPointInsideFrustum(plVector3f_t position)
{
	for (int i = 0; i < 4; i++)
	{
		mplane_t *p = _frustum + i;
		if (p->normal[0] * position[0] + p->normal[1] * position[1] + p->normal[2] * position[2] > p->dist)
			return false;
	}

	return true;
}

bool Camera::IsPointOutsideFrustum(plVector3f_t position)
{
	for (int i = 0; i < 4; i++)
	{
		mplane_t *p = _frustum + i;
		if (p->normal[0] * position[0] + p->normal[1] * position[1] + p->normal[2] * position[2] < p->dist)
			return false;
	}

	return true;
}

bool Camera::IsBoxInsideFrustum(plVector3f_t mins, plVector3f_t maxs)
{
	for (int i = 0; i < 4; i++)
	{
		mplane_t *p = _frustum + i;
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
		mplane_t *p = _frustum + i;
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
	plVectorSet3f(_angles, x, y, z);
}

void Camera::SetAngles(plVector3f_t angles)
{
	plVectorCopy(angles, _angles);
}

void Camera::PrintAngles()
{
	Con_Printf("CAMERA ANGLES : %i %i %i\n",
		(int)_angles[PL_PITCH],
		(int)_angles[PL_YAW],
		(int)_angles[PL_ROLL]);
}

// Position

void Camera::SetPosition(float x, float y, float z)
{
	_position[0] = x;
	_position[1] = y;
	_position[2] = z;
}

void Camera::SetPosition(plVector3f_t position)
{
	plVectorCopy(position, _position);
}

void Camera::PrintPosition()
{
	Con_Printf("CAMERA POSITION : %i %i %i\n",
		(int)_position[0],
		(int)_position[1],
		(int)_position[2]);
}

void Camera::TracePosition()
{
	PLVector3f v = { 0, 0, 0 };
	plVectorScalef(_forward, 8192.0f, v);

	PLVector3f w = { 0, 0, 0 };
	TraceLine(_position, v, w);

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

////////////////////////////////////////////////////

void Camera::SetViewport(IViewport *viewport)
{
	if (_viewport && (_viewport == viewport))
		return;

	_viewport = dynamic_cast<Viewport*>(viewport);
	if (!_viewport)
	{
		Con_Warning("Invalid viewport!\n");
		return;
	}
}

/*	Camera C Interface	*/

void Camera_SetPosition(Camera *camera, plVector3f_t position)
{
	if (!camera)
		return;

	camera->SetPosition(position);
}

void Camera_SetAngles(Camera *camera, plVector3f_t angles)
{
	if (!camera)
		return;

	camera->SetAngles(angles);
}