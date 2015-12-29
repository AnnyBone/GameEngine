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

#include "EngineEditor.h"
#include "video.h"
#include "EngineInput.h"
#include "EngineMenu.h"

/*
	The Katana Editor is perhaps the biggest addition to
	the engine. If this is finished in-time for OpenKatana's
	release then big woop! This idea was inspired by the
	1998 version of Prey's editor (also known as Preditor)
	which was a unique editor similar to the Unreal Engine's
	editor in that it shows your map while you work and the
	editor environment is in-engine which means that what you
	see is what you'll get (WYSIWYG).
	Preditor supported multiple ways of working, we'll be
	going for Matt's favourite approach and allowing users
	to edit the map	from a single window with a realtime preview
	of the map in view...
*/

ConsoleVariable_t 
cvEditorLightPreview = { "editor_lightpreview", "0", false, false, "Enables a preview showing the radius of each light." };

/*  Initialization function.
*/
void Editor_Initialize(void)
{
	Cvar_RegisterVariable(&cvEditorLightPreview,NULL);
}

/*  Handle input.
*/
void Editor_Input(int iKey)
{
	switch(iKey)
	{
	case K_UPARROW:
	case K_DOWNARROW:
	case K_LEFTARROW:
	case K_RIGHTARROW:
		break;
	}
}

void Editor_Frame(void)
{
	cl.oldtime = cl.time;
	cl.time += host_frametime;
}
