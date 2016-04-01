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
#include "video_shader.h"

#include "client/shader_base.h"
#include "client/shader_water.h"

/*
	TODO:
		Move all GL functionality over into VideoLayer.
		Better tracking and error checking?
		Evaluate whether the hardware supports shaders or not.
*/

using namespace Core;

ShaderManager *g_shadermanager = nullptr;

ShaderManager::ShaderManager()
{
	Con_Printf("Initializing Shader Manager...\n");

	programs.reserve(16);

	Add(new BaseShader(), "base");
#if 0
	Add(new WaterShader(), "water");
#endif

	// Base starts off enabled, sloppy, I know but this will likely
	// change.
	ShaderProgram *program = Find("base");
	if (program)
		program->Enable();
}

ShaderManager::~ShaderManager()
{
	Clear();
}

void ShaderManager::Add(ShaderProgram *program, std::string name)
{
	Con_Printf("Adding new shader: %s\n", name.c_str());

	// todo: do we really want to do this here!?
	program->Initialize();

	programs.emplace(name, program);
}

void ShaderManager::Delete(ShaderProgram *_program)
{
	for (auto program = programs.begin(); program != programs.end(); ++program)
		if (program->second == _program)
		{
			delete program->second;
			programs.erase(program);
			break;
		}
}

void ShaderManager::Delete(std::string name)
{
	auto program = programs.find(name);
	if (program != programs.end())
	{
		delete program->second;
		programs.erase(program);
	}
}

void ShaderManager::Clear()
{
	for (auto program = programs.begin(); program != programs.end(); ++program)
		delete program->second;

	programs.clear();
}

ShaderProgram *ShaderManager::Find(std::string name)
{
	auto program = programs.find(name);
	if (program != programs.end())
		return program->second;

	return nullptr;
}

// Shader

Shader::Shader(vlShaderType_t type)
{
	instance = 0;

	this->type = type;

	source_length = 0;
}

bool Shader::Load(const char *path)
{
#ifdef VL_MODE_OPENGL
	VIDEO_FUNCTION_START
	// Check that the path is valid.
	if (path[0] == ' ')
	{
		Con_Warning("Invalid shader path! (%s)\n", path);
		return false;
	}

	// Ensure the type is valid.
	if ((type != VL_SHADER_FRAGMENT) && (type != VL_SHADER_VERTEX))
	{
		Con_Warning("Invalid shader type! (%i) (%s)\n", path, type);
		return false;
	}

	// Ensure we use the correct path and shader.
	unsigned int stype;
	if (type == VL_SHADER_FRAGMENT)
	{
		sprintf(source_path, "%s%s_fragment.shader", g_state.path_shaders, path);
		stype = GL_FRAGMENT_SHADER;
	}
	else
	{
		sprintf(source_path, "%s%s_vertex.shader", g_state.path_shaders, path);
		stype = GL_VERTEX_SHADER;
	}

	// Attempt to load it.
	source = (char*)COM_LoadHeapFile(source_path);
	if (!source || source[0] == ' ')
	{
		Con_Warning("Failed to load shader! (%s)\n", source_path);
		return false;
	}

	// Ensure it's a valid length.
#ifdef _MSC_VER
#	pragma warning(suppress: 6387)
#endif
	source_length = strlen(source);
	if (source_length <= 1)
	{
		Con_Warning("Invalid shader! (%i) (%s)\n", source_length, source_path);
		return false;
	}

	instance = glCreateShader(stype);
	const char *full_source[] = {
#if defined (VL_MODE_OPENGL)
		"#version 120\n",	// OpenGL 2.1
#elif defined (VL_MODE_OPENGL_ES)
		"#version 100\n",	// OpenGL ES 2.0
#endif
		source
	};
	glShaderSource(instance, 2, full_source, NULL);
	glCompileShader(instance);

	if (!CheckCompileStatus())
	{
		Con_Warning("Shader compilation failed! (%s)\n", source_path);
		return false;
	}

	// Everything worked out okay!
	return true;
	VIDEO_FUNCTION_END
#else
	return false;
#endif
}

Shader::~Shader()
{
#ifdef VL_MODE_OPENGL
	VIDEO_FUNCTION_START
	glDeleteShader(instance);
	VIDEO_FUNCTION_END
#endif
}

// Compilation

bool Shader::CheckCompileStatus()
{
#ifdef VL_MODE_OPENGL
	VIDEO_FUNCTION_START
	int compile;
	glGetObjectParameterivARB(instance, GL_COMPILE_STATUS, &compile);
	if (!compile)
	{
		int length = 0;
		glGetShaderiv(instance, GL_INFO_LOG_LENGTH, &length);
		if (length > 1)
		{
			char *cLog = new char[length];
			glGetShaderInfoLog(instance, length, NULL, cLog);
			Con_Warning("%s\n", cLog);
			delete[] cLog;
		}

		return false;
	}

	return true;
	VIDEO_FUNCTION_END
#else
	return false;
#endif
}

// Information

unsigned int Shader::GetInstance()
{
	return instance;
}

vlShaderType_t Shader::GetType()
{
	return type;
}

/*
	Shader Program
*/

ShaderProgram::ShaderProgram(std::string _name) : 
	name(_name), 
	instance(0),
	isenabled(false)
{
	instance = vlCreateShaderProgram();
	if (!instance)
		throw EngineException("Failed to create shader program!\n");
}

ShaderProgram::~ShaderProgram()
{
	vlDeleteShaderProgram(&instance);
}

void ShaderProgram::RegisterShader(std::string path, vlShaderType_t type)
{
	Shader *shader_ = new Shader(type);
	if (!shader_->Load(path.c_str()))
		throw EngineException("Failed to load shader! (%s)\n", path.c_str());

	Attach(shader_);
}

void ShaderProgram::Attach(Shader *shader)
{
	VIDEO_FUNCTION_START
	if (!shader)
		throw EngineException("Attempted to attach an invalid shader!\n");

#ifdef VL_MODE_OPENGL
	glAttachShader(instance, shader->GetInstance());
#endif
	shaders.push_back(shader);
	VIDEO_FUNCTION_END
}

void ShaderProgram::Enable()
{
	VIDEO_FUNCTION_START
	vlUseProgram(instance);
	VIDEO_FUNCTION_END
}

void ShaderProgram::Disable()
{
	VIDEO_FUNCTION_START
	vlUseProgram(0);
	VIDEO_FUNCTION_END
}

void ShaderProgram::Link()
{
#ifdef VL_MODE_OPENGL
	VIDEO_FUNCTION_START
	glLinkProgram(instance);

	int iLinkStatus;
	glGetProgramiv(instance, GL_LINK_STATUS, &iLinkStatus);
	if (!iLinkStatus)
	{
		int iLength = 0;

		glGetProgramiv(instance, GL_INFO_LOG_LENGTH, &iLength);
		if (iLength > 1)
		{
			char *cLog = new char[iLength];
			glGetProgramInfoLog(instance, iLength, NULL, cLog);
			Con_Warning("%s\n", cLog);
			delete[] cLog;
		}

		Sys_Error("Shader program linking failed!\nCheck log for details.\n");
	}
	VIDEO_FUNCTION_END
#endif
}

void ShaderProgram::Shutdown()
{
	Disable();
}

// Uniform Handling

int ShaderProgram::GetUniformLocation(const char *name)
{
#ifdef VL_MODE_OPENGL
	return glGetUniformLocation(instance, name);
#else
	return 0;
#endif
}

void ShaderProgram::SetVariable(int location, float x, float y, float z)
{
#ifdef VL_MODE_OPENGL
	if (!IsActive())
		Sys_Error("Ensure shader program is enabled before applying variables! (%i) (%i %i %i)\n",
			location, (int)x, (int)y, (int)z);
	
	glUniform3f(location, x, y, z);
#endif
}

void ShaderProgram::SetVariable(int location, MathVector3f_t vector)
{
#ifdef VL_MODE_OPENGL
	if (!IsActive())
		Sys_Error("Ensure shader program is enabled before applying variables! (%i) (%i %i %i)\n",
			location, (int)vector[0], (int)vector[1], (int)vector[2]);

	glUniform3fv(location, 3, vector);
#endif
}

void ShaderProgram::SetVariable(int location, float x, float y, float z, float a)
{
#ifdef VL_MODE_OPENGL
	if (!IsActive())
		Sys_Error("Ensure shader program is enabled before applying variables! (%i) (%i %i %i %i)\n",
			location, (int)x, (int)y, (int)z, (int)a);
	glUniform4f(location, x, y, z, a);
#endif
}

void ShaderProgram::SetVariable(int location, int i)
{
#ifdef VL_MODE_OPENGL
	if (!IsActive())
		Sys_Error("Ensure shader program is enabled before applying variables! (%i) (%i)\n",
			location, i);
	glUniform1i(location, i);
#endif
}

void ShaderProgram::SetVariable(int location, float f)
{
#ifdef VL_MODE_OPENGL
	if (!IsActive())
		Sys_Error("Ensure shader program is enabled before applying variables! (%i) (%i)\n",
			location, (int)f);
	glUniform1f(location, f);
#endif
}