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

#include "shared_game.h"
#include "shared_server.h"
#include "shared_client.h"
#include "shared_formats.h"
#include "shared_engine.h"

#ifdef __cplusplus
#	include "base_class.h"
#	include "exception.h"

#	include "texture.h"
#endif

#define	ENGINE_LOG	"engine"

#ifdef _DEBUG
#	define ENGINE_FUNCTION_START	plWriteLog(ENGINE_LOG, "Function start (%s)\n", PL_FUNCTION);
#	define ENGINE_FUNCTION_END		plWriteLog(ENGINE_LOG, "Function end (%s)\n", PL_FUNCTION);
#else
#	define ENGINE_FUNCTION_START
#	define ENGINE_FUNCTION_END
#endif

typedef struct
{
	// Host Information
	char host_username[PL_SYSTEM_MAX_USERNAME];	// Current system username.

	char	
		path_materials[PL_SYSTEM_MAX_PATH],
		path_textures[PL_SYSTEM_MAX_PATH],
		path_levels[PL_SYSTEM_MAX_PATH],
		path_screenshots[PL_SYSTEM_MAX_PATH],
		path_modules[PL_SYSTEM_MAX_PATH],
		path_fonts[PL_SYSTEM_MAX_PATH],
		path_shaders[PL_SYSTEM_MAX_PATH],
		path_sounds[PL_SYSTEM_MAX_PATH];

	bool embedded;

	double current_time;
} XGlobal;

PL_EXTERN_C

extern XGlobal			g_state;
extern XEngineImport	g_launcher;

PL_EXTERN_C_END