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

/*
	TODO:
		Move all GL functionality over into VideoLayer.
		Better tracking and error checking?
*/

// Manager

void VideoShaderManager::Initialize()
{
	program = new VideoShaderProgram();
	program->Initialize();

	RegisterShaders();
}

void VideoShaderManager::Shutdown()
{
	program->Shutdown();

	shaders.clear();
	shaders.shrink_to_fit();
}

// Shader

VideoShader::VideoShader(VideoShaderType_t type)
{
	instance = 0;

	this->type = type;

	source_length = 0;
}

bool VideoShader::Load(const char *path)
{
	VIDEO_FUNCTION_START
	// Check that the path is valid.
	if (path[0] == ' ')
	{
		Con_Warning("Invalid shader path! (%s)\n", path);
		return false;
	}

	// Ensure the type is valid.
	if ((type < VIDEO_SHADER_FRAGMENT) || (type > VIDEO_SHADER_VERTEX))
	{
		Con_Warning("Invalid shader type! (%i) (%s)\n", path, type);
		return false;
	}

	// Ensure we use the correct path and shader.
	unsigned int uiShaderType;
	if (type == VIDEO_SHADER_FRAGMENT)
	{
		sprintf(source_path, "%s%s_fragment.shader", Global.cShaderPath, path);
		uiShaderType = GL_FRAGMENT_SHADER;
	}
	else
	{
		sprintf(source_path, "%s%s_vertex.shader", Global.cShaderPath, path);
		uiShaderType = GL_VERTEX_SHADER;
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
#pragma warning(suppress: 6387)
#endif
	source_length = strlen(source);
	if (source_length <= 1)
	{
		Con_Warning("Invalid shader! (%i) (%s)\n", source_length, source_path);
		return false;
	}

	instance = glCreateShader(uiShaderType);
	glShaderSource(instance, 1, &source, &source_length);
	glCompileShader(instance);

	if (!CheckCompileStatus())
	{
		Con_Warning("Shader compilation failed! (%s)\n", source_path);
		return false;
	}

	// Everything worked out okay!
	return true;
	VIDEO_FUNCTION_END
}

VideoShader::~VideoShader()
{
	VIDEO_FUNCTION_START
	glDeleteShader(instance);
	VIDEO_FUNCTION_END
}

// Compilation

bool VideoShader::CheckCompileStatus()
{
	VIDEO_FUNCTION_START
	int iCompileStatus;
	glGetObjectParameterivARB(instance, GL_COMPILE_STATUS, &iCompileStatus);
	if (!iCompileStatus)
	{
		int iLength = 0, sLength = 0;
		glGetShaderiv(instance, GL_INFO_LOG_LENGTH, &iLength);

		if (iLength > 1)
		{
			char *cLog = new char[iLength];
			glGetInfoLogARB(instance, iLength, &sLength, cLog);
			Con_Warning("%s\n", cLog);
			delete[] cLog;
		}

		return false;
	}

	return true;
	VIDEO_FUNCTION_END
}

// Information

unsigned int VideoShader::GetInstance()
{
	return instance;
}

VideoShaderType_t VideoShader::GetType()
{
	return type;
}

/*
	Shader Program
*/

VideoShaderProgram::VideoShaderProgram()
{
	instance	= 0;
	isenabled	= false;
}

VideoShaderProgram::~VideoShaderProgram()
{
	glDeleteProgram(instance);
}

void VideoShaderProgram::Initialize()
{
	instance = glCreateProgram();
	if (!instance)
		Sys_Error("Failed to create shader program!\n");
}

void VideoShaderProgram::Attach(VideoShader *shader)
{
	VIDEO_FUNCTION_START
	if (!shader)
		Sys_Error("Attempted to attach an invalid shader!\n");

	glAttachShader(instance, shader->GetInstance());
	VIDEO_FUNCTION_END
}

void VideoShaderProgram::Enable()
{
	VIDEO_FUNCTION_START
	VideoLayer_UseProgram(instance);
	VIDEO_FUNCTION_END
}

void VideoShaderProgram::Disable()
{
	VIDEO_FUNCTION_START
	VideoLayer_UseProgram(0);
	VIDEO_FUNCTION_END
}

void VideoShaderProgram::Link()
{
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
			int iLoser = 0;

			char *cLog = new char[iLength];
			glGetInfoLogARB(instance, iLength, &iLoser, cLog);
			Con_Warning("%s\n", cLog);
			delete[] cLog;
		}

		Sys_Error("Shader program linking failed!\nCheck log for details.\n");
	}
	VIDEO_FUNCTION_END
}

void VideoShaderProgram::Shutdown()
{
	Disable();
}

// Uniform Handling

int VideoShaderProgram::GetUniformLocation(const char *name)
{
	return glGetUniformLocation(instance, name);
}

void VideoShaderProgram::SetVariable(int location, float x, float y, float z)
{
	if (!IsActive())
		Sys_Error("Ensure shader program is enabled before applying variables! (%i) (%i %i %i)\n",
			location, (int)x, (int)y, (int)z);
	
	glUniform3f(location, x, y, z);
}

void VideoShaderProgram::SetVariable(int location, MathVector3f_t vector)
{
	if (!IsActive())
		Sys_Error("Ensure shader program is enabled before applying variables! (%i) (%i %i %i)\n",
			location, (int)vector[0], (int)vector[1], (int)vector[2]);

	glUniform3fv(location, 3, vector);
}

void VideoShaderProgram::SetVariable(int location, float x, float y, float z, float a)
{
	if (!IsActive())
		Sys_Error("Ensure shader program is enabled before applying variables! (%i) (%i %i %i %i)\n",
			location, (int)x, (int)y, (int)z, (int)a);
	glUniform4f(location, x, y, z, a);
}

void VideoShaderProgram::SetVariable(int location, int i)
{
	if (!IsActive())
		Sys_Error("Ensure shader program is enabled before applying variables! (%i) (%i)\n",
			location, i);
	glUniform1i(location, i);
}

void VideoShaderProgram::SetVariable(int location, float f)
{
	if (!IsActive())
		Sys_Error("Ensure shader program is enabled before applying variables! (%i) (%i)\n",
			location, (int)f);
	glUniform1f(location, f);
}

unsigned int VideoShaderProgram::GetInstance()
{
	return instance;
}

/*
	C Wrapper
*/

VideoShaderProgram *base_program;
VideoShader *base_fragment, *base_vertex;

// Uniforms
int	iDiffuseUniform;

void VideoShader_Initialize(void)
{
	// Program needs to be created first.
	base_program = new VideoShaderProgram();
	base_program->Initialize();

	// Followed by the shaders.
	base_vertex = new VideoShader(VIDEO_SHADER_VERTEX);
	if (!base_vertex->Load("base"))
		Sys_Error("Failed to load base vertex shader!\n");

	base_fragment = new VideoShader(VIDEO_SHADER_FRAGMENT);
	if (!base_fragment->Load("base"))
		Sys_Error("Failed to load base fragment shader!\n");

	// Attach and link it, if this fails then it fails.
	base_program->Attach(base_vertex);
	base_program->Attach(base_fragment);
	base_program->Link();

	iDiffuseUniform = base_program->GetUniformLocation("diffuseTexture");
}

void VideoShader_Shutdown()
{
	// Check if it's initialized first, just in-case.
	if (base_program)
	{
		base_program->Shutdown();
		delete base_program;
	}
}