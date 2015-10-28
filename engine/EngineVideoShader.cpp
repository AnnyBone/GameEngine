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

CVideoShader::CVideoShader(VideoShaderType_t type)
{
	instance = 0;

	this->type = type;

	source_length = 0;
}

bool CVideoShader::Load(const char *path)
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
	source = (char*)COM_LoadFile(source_path, 0);
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

CVideoShader::~CVideoShader()
{
	VIDEO_FUNCTION_START
	glDeleteShader(instance);
	VIDEO_FUNCTION_END
}

// Compilation

bool CVideoShader::CheckCompileStatus()
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

unsigned int CVideoShader::GetInstance()
{
	return instance;
}

VideoShaderType_t CVideoShader::GetType()
{
	return type;
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
	VIDEO_FUNCTION_START
	if (!Shader)
		Sys_Error("Attempted to attach an invalid shader!\n");

	glAttachShader(vsProgram, Shader->GetInstance());
	VIDEO_FUNCTION_END
}

void CVideoShaderProgram::Enable()
{
	VIDEO_FUNCTION_START
	glUseProgram(vsProgram);
	VIDEO_FUNCTION_END
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

int CVideoShaderProgram::GetUniformLocation(const char *ccUniformName)
{
	return glGetUniformLocation(vsProgram, ccUniformName);
}

void CVideoShaderProgram::SetVariable(int iUniformLocation, float x, float y, float z)
{
	// TODO: Error checking!
	glUniform3f(iUniformLocation, x, y, z);
}

void CVideoShaderProgram::SetVariable(int iUniformLocation, MathVector3f_t mvVector)
{
	// TODO: Error checking!
	glUniform3fv(iUniformLocation, 3, mvVector);
}

void CVideoShaderProgram::SetVariable(int iUniformLocation, float x, float y, float z, float a)
{
	// TODO: Error checking!
	glUniform4f(iUniformLocation, x, y, z, a);
}

void CVideoShaderProgram::SetVariable(int iUniformLocation, int i)
{
	// TODO: Error checking!
	glUniform1i(iUniformLocation, i);
}

void CVideoShaderProgram::SetVariable(int iUniformLocation, float f)
{
	// TODO: Error checking!
	glUniform1f(iUniformLocation, f);
}

//

// Information

unsigned int CVideoShaderProgram::GetInstance()
{
	return vsProgram;
}

/*
	C Wrapper
*/

CVideoShaderProgram *BaseProgram;
CVideoShader *BaseFragmentShader, *BaseVertexShader;

// Uniforms
int
	iDiffuseUniform,
	iDetailUniform,
	iFullbrightUniform,
	iSphereUniform,

	iScaleUniform,

	iLightPositionUniform,
	iLightColourUniform;

void VideoShader_Initialize(void)
{
	// Program needs to be created first.
	BaseProgram = new CVideoShaderProgram();

	// Followed by the shaders.
	BaseVertexShader = new CVideoShader(VIDEO_SHADER_VERTEX);
	if (!BaseVertexShader->Load("base"))
		Sys_Error("Failed to load base vertex shader!\n");

	BaseFragmentShader = new CVideoShader(VIDEO_SHADER_FRAGMENT);
	if (!BaseFragmentShader->Load("base"))
		Sys_Error("Failed to load base fragment shader!\n");

	// Attach and link it, if this fails then it fails.
	BaseProgram->Attach(BaseVertexShader);
	BaseProgram->Attach(BaseFragmentShader);
	BaseProgram->Link();

	iDiffuseUniform = BaseProgram->GetUniformLocation("diffuseTexture");
	iDetailUniform = BaseProgram->GetUniformLocation("detailTexture");
	iFullbrightUniform = BaseProgram->GetUniformLocation("fullbrightTexture");
	iSphereUniform = BaseProgram->GetUniformLocation("sphereTexture");

	iScaleUniform = BaseProgram->GetUniformLocation("vertexScale");

	iLightPositionUniform = BaseProgram->GetUniformLocation("lightPosition");
	iLightColourUniform = BaseProgram->GetUniformLocation("lightColour");
}

void VideoShader_Enable(void)
{
#ifdef VIDEO_SUPPORT_SHADERS
	BaseProgram->Enable();
#endif
}

void VideoShader_Disable(void)
{
#ifdef VIDEO_SUPPORT_SHADERS
	BaseProgram->Disable();
#endif
}

void VideoShader_SetVariablei(int iUniformLocation, int i)
{
#ifdef VIDEO_SUPPORT_SHADERS
	BaseProgram->SetVariable(iUniformLocation, i);
#endif
}

void VideoShader_SetVariablef(int iUniformLocation, float f)
{
	BaseProgram->SetVariable(iUniformLocation, f);
}

void VideoShader_SetVariable3f(int iUniformLocation, float x, float y, float z)
{
	BaseProgram->SetVariable(iUniformLocation, x, y, z);
}

void VideoShader_Shutdown()
{
	// Check if it's initialized first, just in-case.
	if (BaseProgram)
		delete BaseProgram;
}