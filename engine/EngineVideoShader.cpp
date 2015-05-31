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

	void PrintLog();
	void Enable();
	void Disable();
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

void CVideoShader::Enable()
{
	glUseProgram(program);
}

void CVideoShader::Disable()
{
	glUseProgram(0);
}

CVideoShader *shaderBase;

void VideoShader_Initialize(void)
{
	shaderBase = new CVideoShader("base");
}

void VideoShader_Enable(void)
{
	shaderBase->Enable();
}

void VideoShader_Disable(void)
{
	shaderBase->Disable();
}