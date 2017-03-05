/*
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

typedef struct Material Material;
typedef struct model_s model_t;
typedef struct DynamicLight_s DynamicLight_t;
typedef struct ClientEntity_s ClientEntity_t;
typedef struct ConsoleVariable_s ConsoleVariable_t;

/*	Functions exported from the engine.
*/
typedef struct EngineExport_e
{
	char*(*GetBasePath)();		// Returns the currently active game path.
	char*(*GetMaterialPath)();	// Returns the set material path.
	//char*(*GetModelPath)();	// Returns the set model path.

	void(*SetViewportSize)(unsigned int width, unsigned int height);

	Material*(*LoadMaterial)(const char *cPath);

	void(*UnloadMaterial)(Material *material, bool force);

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
	void(*Print)(const char *msg, ...);														// Prints a message to the console.
	void(*PrintDev)(const char *msg, ...);													// Prints a developer message to the console.

	float(*GetConsoleVariableValue)(const char *var_name);
	bool(*GetConsoleVariableBoolValue)(const char *var_name);

	// Video...

	//void(*DrawPreFrame)(void);
	//void(*DrawPostFrame)(void);
	//void(*DrawGradientBackground)(void);
	//void(*DrawGrid)(float x, float y, float z, int grid_size);
	//void(*DrawString)(int x, int y, const char *cMsg);
	//void(*DrawLine)(MathVector3f_t mvStart, MathVector3f_t mvEnd);
	//void(*DrawMaterialSurface)(Material *mMaterial, int iSkin, int x, int y, int w, int h, float fAlpha);
	//void(*DrawEntity)(ClientEntity_t *Entity);
	//void(*DrawSetCanvas)(VideoCanvasType_t Canvas);
	//void(*DrawResetCanvas)(void);
} XEngineExport;

/*	Functions imported by the engine.
*/
typedef struct
{
	int	iVersion;

	void(*PrintMessage)(char *text);
	void(*PrintWarning)(char *text);
	void(*PrintError)(char *text);
} XEngineImport;

/////////////////////////////////////////////////////////////////////////////////

#define ENGINE_MODULE				"engine"
#define ENGINE_VERSION_INTERFACE	(sizeof(XEngineImport)+sizeof(XEngineExport))

#if defined(KATANA)
#	define ENGINE_FUNCTION PL_EXPORT
#else
#	define ENGINE_FUNCTION
#endif

typedef enum
{
	XPATH_BASE,

	XPATH_FONTS,
	XPATH_LEVELS,
	XPATH_MATERIALS,
	XPATH_SCREENSHOTS,
	XPATH_MODULES,
	XPATH_SHADERS,
	XPATH_SOUNDS,

	XPATH_END
} XPath;

#ifdef __cplusplus

namespace xenon
{
	ENGINE_FUNCTION PLresult Initialize(int argc, char *argv[]);
	ENGINE_FUNCTION void Shutdown();
	ENGINE_FUNCTION void Loop();

	ENGINE_FUNCTION PLbool IsRunning();

	ENGINE_FUNCTION const PLchar *GetPath(XPath path);

	ENGINE_FUNCTION PLuint32 GetVersion();
	ENGINE_FUNCTION PLchar *GetVersionString();
	ENGINE_FUNCTION PLint32 GetInterfaceVersion();
}

#endif
