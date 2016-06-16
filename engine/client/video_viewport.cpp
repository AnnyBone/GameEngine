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

#include "EngineMenu.h"

/*	Viewport Manager	*/

using namespace Core;

Viewport *viewport_main = nullptr;

// Creates a new viewport and camera for that viewport.
void Core::CreatePrimaryViewport()
{
	if (viewport_main)
		return;

	viewport_main = new Viewport(g_mainwindow.width, g_mainwindow.height);
	if (!viewport_main)
		throw Exception("Failed to allocate primary viewport!\n");

	viewport_main->SetCamera(new Camera);
}

Viewport *Core::GetPrimaryViewport()
{
	if (!viewport_main)
	{
		Con_Warning("Attempted to get viewport, despite not yet being initialized!\n");
		return nullptr;
	}

	return viewport_main;
}

void Core::DestroyPrimaryViewport()
{
	if (!viewport_main) return;
	delete viewport_main;
}

////////////////////////////////////////////////////////////////////////////////

Viewport::Viewport(unsigned int width, unsigned int height)
	: _width(width), _height(height)
{
	_x = 0; _y = 0;
}

void Viewport::Draw()
{
	if (scr_disabled_for_loading)
		return;

#if defined (VL_MODE_OPENGL)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#endif

	vlViewport(_x, _y, _width, _height);
	vlScissor(_x, _y, _width, _height);

	/*
		glx + r_refdef.vrect.x,
		gly + glheight - r_refdef.vrect.y - r_refdef.vrect.height,
		r_refdef.vrect.width,
		r_refdef.vrect.height
	*/

	vlClearBuffers(VL_MASK_DEPTH | VL_MASK_COLOUR | VL_MASK_STENCIL);

	double time1;
	if (r_speeds.value)
	{
		vlFinish();

		time1 = System_DoubleTime();

		rs_brushpolys = rs_aliaspolys = rs_skypolys = rs_particles = rs_fogpolys =
		rs_dynamiclightmaps = rs_aliaspasses = rs_skypasses = rs_brushpasses = 0;
	}

#if defined (VL_MODE_OPENGL)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
#endif

	if (_camera && !con_forcedup)
	{
		// Let the camera manager know we're drawing from this
		// camera.
		g_cameramanager->SetCurrentCamera(_camera);

		if (cv_video_msaasamples.iValue > 0)
			vlEnable(VL_CAPABILITY_MULTISAMPLE);

		_camera->Draw();

		if (cv_video_msaasamples.iValue > 0)
			vlDisable(VL_CAPABILITY_MULTISAMPLE);
	}

	Draw_ResetCanvas();

	g_menu->Draw(this);

	// todo, move the following into the menu.
	Screen_DrawNet();
	Screen_DrawConsole();
	Screen_DrawFPS();

	if (cv_video_finish.bValue)
		vlFinish();

	if (!Video.bSkipUpdate)
		Window_Swap();
}

/*	Camera	*/

ICamera *Viewport::GetCamera()
{
	return dynamic_cast<ICamera*>(_camera);
}

void Viewport::SetCamera(ICamera *camera)
{
	Camera *newcam = dynamic_cast<Camera*>(camera);
	if (!newcam)
	{
		Con_Warning("Attempted to assign invalid camera to viewport!\n");
		return;
	}
	
	newcam->SetViewport(this);
	newcam->SetFOV(90);
	_camera = newcam;
}

/*	Size	*/

void Viewport::SetSize(unsigned int width, unsigned int height)
{
	if (width == 0) width = 1;
	if (height == 0) height = 1;

	_width = width; _height = height;

	if (_camera) _camera->SetFOV(90);
}

void Viewport::SetPosition(int x, int y)
{
	_x = x; _y = y;
}