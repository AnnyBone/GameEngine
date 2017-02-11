/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others
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

// Platform Library
#include "platform_filesystem.h"
#include "platform_library.h"

#ifdef _DEBUG
#	include <assert.h>
#endif

#ifdef _MSC_VER
#	pragma warning(disable:4115)
#	pragma warning(disable:4996)
#	pragma warning(disable:4229)   // mgraph gets this
#	pragma warning(disable:4305)
#endif

#include "shared_base.h"

#include "XenonBuild.h"

#define MINIMUM_MEMORY	0x2000000	// 32MB

#define MAX_NUM_ARGVS	50

#define DATAGRAM_MTU	1400	// johnfitz -- actual limit for unreliable messages to nonlocal clients

// per-level limits
#define MIN_EDICTS		256			// johnfitz -- lowest allowed value for max_edicts cvar
#define MAX_EDICTS		32000		// johnfitz -- highest allowed value for max_edicts cvar
									// ents past 8192 can't play sounds in the standard protocol
#define	MAX_LIGHTSTYLES	64
#define	MAX_MODELS		2048		// johnfitz -- was 256
#define	MAX_SOUNDS		2048		// johnfitz -- was 256
#define MAX_PARTICLES	4096		// Default max # of particles at one time
#define MAX_EFFECTS		1024		// Max textures for particles, sprites and flares

#define	SAVEGAME_COMMENT_LENGTH	39

#define	MAX_SCOREBOARD		64

#include "cmdlib.h"
#include "EngineCommon.h"
#include "vid.h"
#include "sys.h"
#include "zone.h"

#include "Xenon.h"
#include "EngineConsoleVariable.h"
#include "video_window.h"
#include "EngineVideoDraw.h"
#include "EngineVideoScreen.h"
#include "EngineNetwork.h"
#include "EngineProtocol.h"
#include "EngineConsoleCommand.h"
#include "sound.h"
#include "audio.h"
#include "render.h"
#include "client.h"
#include "EngineClientTempEntity.h"
#include "progs.h"
#include "server.h"
#include "EngineModel.h"
#include "EngineImage.h" //johnfitz
#include "EngineVideoTextureManager.h"

#include "world.h"
#include "keys.h"
#include "EngineConsole.h"
#include "EngineClientView.h"
#include "menu.h"
#include "crc.h"
#include "glquake.h"

PL_EXTERN_C

// System
double System_DoubleTime(void);

PL_EXTERN_C_END

#include "material.h"

//=============================================================================

// the host system specifies the base of the directory tree, the
// command line parms passed to the program, and the amount of memory
// available for the program to use

typedef struct
{
	char	*basedir,
			*cachedir;		// For development over ISDN lines.
	int		argc;
	char	**argv;
	void	*membase;
	int		memsize;

	char	basepath[MAX_QPATH];	// Base directory for game assets.
} XParameters;


//=============================================================================

PL_EXTERN_C

extern bool noclip_anglehack;

// host
extern XParameters host_parms;

extern ConsoleVariable_t sys_ticrate;
extern ConsoleVariable_t sys_nostdout;
extern ConsoleVariable_t developer;
extern ConsoleVariable_t max_edicts; //johnfitz
extern ConsoleVariable_t cv_max_dlights;

extern	bool		g_hostinitialized;	// True if into command execution
extern	double		host_frametime;
extern	int			host_framecount;	// incremented every frame, never reset
extern	double		realtime;			// not bounded in any way, changed at
										// start of every frame, never rese

void Host_ClearMemory(void);
void Host_ServerFrame(void);
void Host_InitCommands(void);
void Host_Initialize(XParameters *parms);
void Host_Shutdown(void);
void Host_Error(const char *error, ...);
void Host_EndGame(char *message, ...);
void Host_Frame(double time);
void Host_Quit_f(void);
void Host_ClientCommands(char *fmt, ...);
void Host_ShutdownServer(bool crash);

extern int	current_skill;		// skill level for currently loaded level (in case
								//  the user changes the cvar while the level is
								//  running, this reflects the level actually in use)

extern bool	bIsDedicated;

// TODO: Move these somewhere more appropriate

// chase
extern ConsoleVariable_t chase_active;

void TraceLine(PLVector3D start, PLVector3D end, PLVector3D impact);

PL_EXTERN_C_END