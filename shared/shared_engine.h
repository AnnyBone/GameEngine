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

#ifndef SHARED_ENGINE_H
#define SHARED_ENGINE_H

#include "platform.h"

#include "shared_flags.h"
#include "shared_material.h"
#include "shared_client.h"

/*	Functions exported from the engine.
*/
typedef struct
{
	int	iVersion;

	bool(*Initialize)(int argc, char *argv[], bool bEmbedded);	// Initializes the engine.
	bool(*IsRunning)();

	char*(*GetBasePath)();		// Returns the currently active game path.
	char*(*GetMaterialPath)();	// Returns the set material path.
	//char*(*GetModelPath)();	// Returns the set model path.
	char*(*GetVersion)();		// Returns the current engine version (as a string).

	void(*Loop)();										// Main loop.
	void(*SetViewportSize)(int iWidth, int iHeight);
	void(*Shutdown)();									// Shutdown.

	Material_t*(*LoadMaterial)(const char *cPath);

	void(*UnloadMaterial)(Material_t *mMaterial);

	model_t*(*LoadModel)(const char *path);

	// Client...

	double(*GetClientTime)();

	ClientEntity_t*(*CreateClientEntity)();	// Creates a "temp" client entity.

	DynamicLight_t*(*CreateDynamicLight)(int Key);

	void(*ClientDisconnect)();

	// Server...

	void(*ServerShutdown)(bool bCrash);

	// Console...

	void(*InsertConsoleCommand)(const char *cCommand);										// Sends the given command to the console.
	void(*RegisterConsoleVariable)(ConsoleVariable_t *cvVariable, void(*Function)(void));	// Register a new console variable.
	void(*SetConsoleVariable)(const char *cVariableName, char *cValue);						// Set the value of an existing console variable.
	void(*ResetConsoleVariable)(const char *cVariableName);									// Resets the value of a console variable.
	void(*Print)(const char *cMessage, ...);												// Prints a message to the console.
	void(*PrintDev)(const char *cMessage, ...);												// Prints a developer message to the console.

	float(*GetConsoleVariableValue)(const char *var_name);
	bool(*GetConsoleVariableBoolValue)(const char *var_name);

	// Video...

	void(*DrawPreFrame)();
	void(*DrawPostFrame)();
	void(*DrawConsole)();
	void(*DrawGradientBackground)();
	void(*DrawFPS)();
	void(*DrawGrid)(float x, float y, float z, int grid_size);
	void(*DrawString)(int x, int y, const char *cMsg);
	void(*DrawLine)(MathVector3f_t mvStart, MathVector3f_t mvEnd);
	void(*DrawMaterialSurface)(Material_t *mMaterial, int iSkin, int x, int y, int w, int h, float fAlpha);
	void(*DrawEntity)(ClientEntity_t *Entity);
	void(*DrawSetCanvas)(VideoCanvasType_t Canvas);
	void(*DrawResetCanvas)();

	// Material Editor
	void(*MaterialEditorInitialize)(void);					// Initializes the material editor.
} EngineExport_t;

/*	Functions imported by the engine.
*/
typedef struct
{
	int	iVersion;

	void(*PrintMessage)(char *text);
	void(*PrintWarning)(char *text);
	void(*PrintError)(char *text);
} EngineImport_t;

#define ENGINE_VERSION	(sizeof(EngineImport_t)+sizeof(EngineExport_t))

#endif // !SHARED_ENGINE_H
