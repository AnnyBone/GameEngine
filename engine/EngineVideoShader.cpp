/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
	Copyright (C) 2011-2015 OldTimes Software

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

extern "C" {
#include "EngineBase.h"

#include "EngineVideo.h"
#include "EngineVideoShader.h"
}

class CVideoShader
{
public:
	CVideoShader(const char *path);
	~CVideoShader();

	void Enable();
	void Disable();

	void SetVariable(const char *name, float x, float y, float z);
	void SetVariable(const char *name, MathVector3f_t vector);
	void SetVariable(const char *name, float x, float y, float z, float a);
	void SetVariable(const char *name, int i);
	void SetVariable(const char *name, float f);
protected:
private:
	const char *vertexSource, *fragmentSource;

	char vertexPath[PLATFORM_MAX_PATH], fragmentPath[PLATFORM_MAX_PATH];

	int vertexSourceLength, fragmentSourceLength;

	unsigned int vertexShader, fragmentShader;
	unsigned int program;
};

CVideoShader::CVideoShader(const char *path)
{
	if (path[0] == ' ')
		Sys_Error("Invalid shader path! (%s)\n", path);

	// Load the vertex shader.

	sprintf(vertexPath, "%s%s_vertex.shader", Global.cShaderPath, path);
	vertexSource = (char*)COM_LoadFile(vertexPath, 0);
	if (!vertexSource)
		Sys_Error("Failed to load shader! (%s)", vertexPath);
	vertexSourceLength = strlen(vertexSource);

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, &vertexSourceLength);
	glCompileShader(vertexShader);

	int compiledStatus;
	glGetObjectParameterivARB(vertexShader, GL_COMPILE_STATUS, &compiledStatus);
	if (!compiledStatus)
	{
		int logLength = 0;
		int sLength = 0;

		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength > 1)
		{
			char *log = (char*)malloc(logLength);
			glGetInfoLogARB(vertexShader, logLength, &sLength, log);
			Con_Warning("%s\n", log);
			free(log);
		}

		Sys_Error("Shader compilation failed! (%s)\n", vertexPath);
	}

	// Load the fragment shader.

	sprintf(fragmentPath, "%s%s_fragment.shader", Global.cShaderPath, path);
	fragmentSource = (char*)COM_LoadFile(fragmentPath, 0);
	if (!fragmentSource)
		Sys_Error("Failed to load shader! (%s)", fragmentPath);
	fragmentSourceLength = strlen(fragmentSource);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, &fragmentSourceLength);
	glCompileShader(fragmentShader);

	glGetObjectParameterivARB(fragmentShader, GL_COMPILE_STATUS, &compiledStatus);
	if (!compiledStatus)
	{
		int logLength = 0;
		int sLength = 0;

		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength > 1)
		{
			char *log = (char*)malloc(logLength);
			glGetInfoLogARB(vertexShader, logLength, &sLength, log);
			Con_Warning("%s\n", log);
			free(log);
		}

		Sys_Error("Shader compilation failed! (%s)\n", fragmentPath);
	}

	// Now link them into a program object.

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &compiledStatus);
	if (!compiledStatus)
	{
		int logLength = 0;
		int sLength = 0;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength > 1)
		{
			char *log = (char*)malloc(logLength);
			glGetInfoLogARB(program, logLength, &sLength, log);
			Con_Warning("%s\n", log);
			free(log);
		}

		Sys_Error("Program linking failed! (%s)\n");
	}
}

CVideoShader::~CVideoShader()
{}

/*
	State Handling
*/

void CVideoShader::Enable()
{
	glUseProgram(program);
}

void CVideoShader::Disable()
{
	glUseProgram(0);
}

/*
	Uniform Handling
*/

void CVideoShader::SetVariable(const char *name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(program, name), x, y, z);
}

void CVideoShader::SetVariable(const char *name, MathVector3f_t vector)
{
	glUniform3fv(glGetUniformLocation(program, name), 3, vector);
}

void CVideoShader::SetVariable(const char *name, float x, float y, float z, float a)
{
	glUniform4f(glGetUniformLocation(program, name), x, y, z, a);
}

void CVideoShader::SetVariable(const char *name, int i)
{
	glUniform1i(glGetUniformLocation(program, name), i);
}

void CVideoShader::SetVariable(const char *name, float f)
{
	glUniform1f(glGetUniformLocation(program, name), f);
}

/**/

CVideoShader *modelShader = NULL;

CVideoShader *currentShader = NULL;

void VideoShader_Initialize(void)
{
	modelShader = new CVideoShader("model");

	currentShader = modelShader;
}

void VideoShader_Enable(void)
{
	currentShader->Enable();
}

void VideoShader_Disable(void)
{
	currentShader->Disable();
}

void VideoShader_SetVariablei(const char *name, int i)
{
	currentShader->SetVariable(name, i);
}

void VideoShader_SetVariablef(const char *name, float f)
{
	currentShader->SetVariable(name, f);
}

void VideoShader_SetVariable3f(const char *name, float x, float y, float z)
{
	currentShader->SetVariable(name, x, y, z);
}

void VideoShader_SetVariable4f(const char *name, float x, float y, float z, float a)
{
	currentShader->SetVariable(name, x, y, z, a);
}