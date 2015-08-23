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

#include "EngineBase.h"

#include "EngineVideo.h"
#include "EngineVideoShader.h"

/*
	TODO:
		Move all GL functionality over into VideoLayer.
		Better tracking and error checking?
*/

using namespace std;

CVideoShader::CVideoShader(const char *ccPath, VideoShaderType vsType)
{
	VIDEO_FUNCTION_START
	// Check that the path is valid.
	if (ccPath[0] == ' ')
		Sys_Error("Invalid shader path! (%s)\n", ccPath);

	// Ensure the type is valid.
	if((vsType < VIDEO_SHADER_FRAGMENT) || (vsType > VIDEO_SHADER_VERTEX))
		Sys_Error("Invalid shader type! (%i) (%s)\n", ccPath, vsType);

	// Set the shader type.
	this->vsType = vsType;

	// Ensure we use the correct path and shader.
	unsigned int uiShaderType;
	if (vsType == VIDEO_SHADER_FRAGMENT)
	{
		sprintf(cShaderPath, "%s%s_fragment.shader", Global.cShaderPath, ccPath);

		uiShaderType = GL_FRAGMENT_SHADER;
	}
	else
	{
		sprintf(cShaderPath, "%s%s_vertex.shader", Global.cShaderPath, ccPath);

		uiShaderType = GL_VERTEX_SHADER;
	}

	// Attempt to load it.
	ccShaderSource = (char*)COM_LoadFile(cShaderPath, 0);
	if(!ccShaderSource)
		Sys_Error("Failed to load shader! (%s)\n", cShaderPath);

	// Ensure it's a valid length.
	iShaderLength = strlen(ccShaderSource);
	if(iShaderLength <= 1)
		Sys_Error("Invalid shader! (%i) (%s)\n", iShaderLength, cShaderPath);

	vsShader = glCreateShader(uiShaderType);
	glShaderSource(vsShader, 1, &ccShaderSource, &iShaderLength);
	glCompileShader(vsShader);

	if(!CheckCompileStatus())
		Sys_Error("Shader compilation failed! (%s)\n", cShaderPath);
	VIDEO_FUNCTION_END
}

CVideoShader::~CVideoShader()
{
	VIDEO_FUNCTION_START
	glDeleteShader(vsShader);
	VIDEO_FUNCTION_END
}

// Compilation

bool CVideoShader::CheckCompileStatus()
{
	VIDEO_FUNCTION_START
	int iCompileStatus;
	glGetObjectParameterivARB(vsShader, GL_COMPILE_STATUS, &iCompileStatus);
	if (!iCompileStatus)
	{
		int iLength = 0, sLength = 0;
		glGetShaderiv(vsShader, GL_INFO_LOG_LENGTH, &iLength);

		if (iLength > 1)
		{
			char *cLog = new char[iLength];
			glGetInfoLogARB(vsShader, iLength, &sLength, cLog);
			Con_Warning("%s\n", cLog);
			delete[] cLog;
		}

		return false;
	}

	return true;
	VIDEO_FUNCTION_END
}

// Information

VideoShader CVideoShader::GetInstance()
{
	return vsShader;
}

VideoShaderType CVideoShader::GetType()
{
	return vsType;
}

/*
	Shader Program
*/

CVideoShaderProgram::CVideoShaderProgram()
{
	vsProgram = glCreateProgram();
	if (!vsProgram)
	{
		// TODO: Error handling!!
	}
}

CVideoShaderProgram::~CVideoShaderProgram()
{
	Disable();

	glDeleteProgram(vsProgram);
}

void CVideoShaderProgram::Attach(CVideoShader *Shader)
{
	if (!Shader)
		Sys_Error("Attempted to attach an invalid shader!\n");

	glAttachShader(vsProgram, Shader->GetInstance());
}

void CVideoShaderProgram::Enable()
{
	glUseProgram(vsProgram);
}

void CVideoShaderProgram::Disable()
{
	VIDEO_FUNCTION_START
	glUseProgram(0);
	VIDEO_FUNCTION_END
}

void CVideoShaderProgram::Link()
{
	glLinkProgram(vsProgram);

	int iLinkStatus;
	glGetProgramiv(vsProgram, GL_LINK_STATUS, &iLinkStatus);
	if (!iLinkStatus)
	{
		int iLength = 0;

		glGetProgramiv(vsProgram, GL_INFO_LOG_LENGTH, &iLength);
		if (iLength > 1)
		{
			int iLoser = 0;

			char *cLog = new char[iLength];
			glGetInfoLogARB(vsProgram, iLength, &iLoser, cLog);
			Con_Warning("%s\n", cLog);
			delete[] cLog;
		}

		Sys_Error("Shader program linking failed!\nCheck log for details.");
	}
}

// Uniform Handling

void CVideoShaderProgram::SetVariable(const char *ccName, float x, float y, float z)
{
	// TODO: Error checking!
	glUniform3f(glGetUniformLocation(vsProgram, ccName), x, y, z);
}

void CVideoShaderProgram::SetVariable(const char *ccName, MathVector3f_t mvVector)
{
	// TODO: Error checking!
	glUniform3fv(glGetUniformLocation(vsProgram, ccName), 3, mvVector);
}

void CVideoShaderProgram::SetVariable(const char *ccName, float x, float y, float z, float a)
{
	// TODO: Error checking!
	glUniform4f(glGetUniformLocation(vsProgram, ccName), x, y, z, a);
}

void CVideoShaderProgram::SetVariable(const char *ccName, int i)
{
	// TODO: Error checking!
	glUniform1i(glGetUniformLocation(vsProgram, ccName), i);
}

void CVideoShaderProgram::SetVariable(const char *ccName, float f)
{
	// TODO: Error checking!
	glUniform1f(glGetUniformLocation(vsProgram, ccName), f);
}

// Information

VideoShaderProgram CVideoShaderProgram::GetInstance()
{
	return vsProgram;
}

/*
	C Wrapper
*/

CVideoShaderProgram *BaseProgram;
CVideoShader *BaseFragmentShader, *BaseVertexShader;

void VideoShader_Initialize(void)
{
	// Program needs to be created first.
	BaseProgram = new CVideoShaderProgram();

	// Followed by the shaders.
	BaseVertexShader = new CVideoShader("base", VIDEO_SHADER_VERTEX);
	BaseFragmentShader = new CVideoShader("base", VIDEO_SHADER_FRAGMENT);

	// Attach and link it, if this fails then it fails.
	BaseProgram->Attach(BaseVertexShader);
	BaseProgram->Attach(BaseFragmentShader);
	BaseProgram->Link();
}

void VideoShader_Enable(void)
{
	BaseProgram->Enable();
}

void VideoShader_Disable(void)
{
	BaseProgram->Disable();
}

void VideoShader_SetVariablei(const char *name, int i)
{
	BaseProgram->SetVariable(name, i);
}

void VideoShader_SetVariablef(const char *name, float f)
{
	BaseProgram->SetVariable(name, f);
}

void VideoShader_SetVariable3f(const char *name, float x, float y, float z)
{
	BaseProgram->SetVariable(name, x, y, z);
}

void VideoShader_SetVariable4f(const char *name, float x, float y, float z, float a)
{
	BaseProgram->SetVariable(name, x, y, z, a);
}

void VideoShader_Shutdown()
{
	// Check if it's initialized first, just in-case.
	if (BaseProgram)
		delete BaseProgram;
}