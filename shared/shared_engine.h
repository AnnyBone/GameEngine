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

// Functions exported from the engine.
typedef struct EngineExport {
    char *(*GetBasePath)();        // Returns the currently active game path.
    char *(*GetMaterialPath)();    // Returns the set material path.
    //char*(*GetModelPath)();	// Returns the set model path.

    void (*SetViewportSize)(unsigned int width, unsigned int height);

    Material *(*LoadMaterial)(const char *cPath);

    void (*UnloadMaterial)(Material *material, bool force);

    model_t *(*LoadModel)(const char *path);

    // Client...

    double (*GetClientTime)();

    ClientEntity_t *(*CreateClientEntity)();    // Creates a "temp" client entity.

    DynamicLight_t *(*CreateDynamicLight)(int Key);

    void (*ClientDisconnect)();

    // Server...

    void (*ServerShutdown)(bool bCrash);

    // Console...

    void (*InsertConsoleCommand)(
            const char *cCommand);                                        // Sends the given command to the console.
    void (*RegisterConsoleVariable)(ConsoleVariable_t *cvVariable,
                                    void(*Function)(void));    // Register a new console variable.
    void (*SetConsoleVariable)(const char *cVariableName,
                               char *cValue);                        // Set the value of an existing console variable.
    void (*ResetConsoleVariable)(
            const char *cVariableName);                                    // Resets the value of a console variable.
    void (*Print)(const char *msg,
                  ...);                                                        // Prints a message to the console.
    void (*PrintDev)(const char *msg,
                     ...);                                                    // Prints a developer message to the console.

    float (*GetConsoleVariableValue)(const char *var_name);

    bool (*GetConsoleVariableBoolValue)(const char *var_name);
} EngineExport;

/*	Functions imported by the engine.
*/
typedef struct EngineImport {
    int version;

    void (*PrintMessage)(char *text);

    void (*PrintWarning)(char *text);

    void (*PrintError)(char *text);
} EngineImport;

/////////////////////////////////////////////////////////////////////////////////

#define ENGINE_MODULE                "engine"
#define ENGINE_VERSION_INTERFACE    (sizeof(EngineImport) + sizeof(EngineExport))

#if defined(ENGINE_INTERNAL)
#	define ENGINE_FUNCTION PL_EXPORT
#else
#	define ENGINE_FUNCTION
#endif

#ifdef __cplusplus

namespace xenon {
    namespace engine {
        ENGINE_FUNCTION bool Initialize(int argc, char **argv);

        ENGINE_FUNCTION void Shutdown();

        ENGINE_FUNCTION void Loop();

        ENGINE_FUNCTION bool IsRunning();

        ENGINE_FUNCTION uint32_t GetVersion();

        ENGINE_FUNCTION const char *GetVersionString();

        ENGINE_FUNCTION int32_t GetInterfaceVersion();
    }
}

#endif
