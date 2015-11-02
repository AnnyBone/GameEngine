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

#ifndef VIDEO_SHADER_H
#define VIDEO_SHADER_H

typedef enum
{
	VIDEO_SHADER_INVALID,

	VIDEO_SHADER_FRAGMENT,
	VIDEO_SHADER_VERTEX
} VideoShaderType_t;

#ifdef __cplusplus
class CVideoShader
{
public:
	CVideoShader(VideoShaderType_t type);
	~CVideoShader();

	bool Load(const char *path);
	bool CheckCompileStatus();

	void Enable();
	void Disable();

	unsigned int GetInstance();
	VideoShaderType_t GetType();

private:
	unsigned int instance;

	VideoShaderType_t type;

	const char	*source;
	char		source_path[PLATFORM_MAX_PATH];
	int			source_length;
};

#define	SHADER_REGISTER(name)

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

	unsigned int GetInstance();
protected:
private:
	//CVideoShader **vsShader;

	unsigned int vsProgram;
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

#endif	// VIDEO_SHADER_H