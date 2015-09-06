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

typedef enum
{
	VIDEO_SHADER_FRAGMENT,
	VIDEO_SHADER_VERTEX
} VideoShaderType_t;

typedef unsigned int VideoShader;
typedef unsigned int VideoShaderProgram;

#ifdef __cplusplus
class CVideoShader
{
public:
	CVideoShader(const char *ccPath, VideoShaderType_t vsType);
	~CVideoShader();

	void Enable();
	void Disable();

	VideoShader GetInstance();
	VideoShaderType_t GetType();

private:
	VideoShader	vsShader;	//sVertex, sFragment;
	VideoShaderType_t vsType;

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
	void SetVariable(int iUniformLocation, float x, float y, float z);
	void SetVariable(int iUniformLocation, MathVector3f_t mvVector);
	void SetVariable(int iUniformLocation, float x, float y, float z, float a);
	void SetVariable(int iUniformLocation, int i);
	void SetVariable(int iUniformLocation, float f);

	int GetUniformLocation(const char *ccUniformName);

	VideoShaderProgram GetInstance();
protected:
private:
	//CVideoShader **vsShader;

	VideoShaderProgram vsProgram;
};

extern "C" {
#endif

	void VideoShader_Initialize(void);
	void VideoShader_Enable(void);
	void VideoShader_Disable(void);
	void VideoShader_SetVariablei(int iUniformLocation, int i);
	void VideoShader_SetVariablef(int iUniformLocation, float f);
	void VideoShader_SetVariable3f(int iUniformLocation, float x, float y, float z);

	extern int
		iDiffuseUniform, iDetailUniform, iFullbrightUniform, iSphereUniform,
		iScaleUniform,
		iLightPositionUniform, iLightColourUniform;

#ifdef __cplusplus
}
#endif

#endif