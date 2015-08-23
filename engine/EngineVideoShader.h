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

#ifndef __ENGINEVIDEOSHADER__
#define __ENGINEVIDEOSHADER__

typedef enum VideoShaderType
{
	VIDEO_SHADER_FRAGMENT,
	VIDEO_SHADER_VERTEX
} VideoShaderType;

typedef unsigned int VideoShader;
typedef unsigned int VideoShaderProgram;

#ifdef __cplusplus
class CVideoShader
{
public:
	CVideoShader(const char *ccPath, VideoShaderType vsType);
	~CVideoShader();

	void Enable();
	void Disable();

	VideoShader GetInstance();
	VideoShaderType GetType();

private:
	VideoShader	vsShader;	//sVertex, sFragment;
	VideoShaderType vsType;

	const char *ccShaderSource;

	char cShaderPath[PLATFORM_MAX_PATH];

	int iShaderLength;

	bool CheckCompileStatus();
};

class CVideoShaderProgram
{
public:
	CVideoShaderProgram();
	~CVideoShaderProgram();

	void Attach(CVideoShader *Shader);
	void Enable();
	void Disable();
	void Link();
	void SetVariable(const char *ccName, float x, float y, float z);
	void SetVariable(const char *ccName, MathVector3f_t mvVector);
	void SetVariable(const char *ccName, float x, float y, float z, float a);
	void SetVariable(const char *ccName, int i);
	void SetVariable(const char *ccName, float f);

	VideoShaderProgram GetInstance();
protected:
private:
	//VideoShader *vsShaders;

	VideoShaderProgram vsProgram;
};

extern "C" {
#endif

	void VideoShader_Initialize(void);
	void VideoShader_Enable(void);
	void VideoShader_Disable(void);
	void VideoShader_SetVariablei(const char *name, int i);
	void VideoShader_SetVariablef(const char *name, float f);
	void VideoShader_SetVariable3f(const char *name, float x, float y, float z);
	void VideoShader_SetVariable4f(const char *name, float x, float y, float z, float a);

#ifdef __cplusplus
}
#endif

#endif