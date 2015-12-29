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

#ifndef VIDEO_SHADER_H
#define VIDEO_SHADER_H

typedef enum
{
	VIDEO_SHADER_INVALID,

	VIDEO_SHADER_FRAGMENT,
	VIDEO_SHADER_VERTEX
} VideoShaderType_t;

typedef int uniform;

#ifdef __cplusplus
class VideoShader
{
public:
	VideoShader(VideoShaderType_t type);
	~VideoShader();

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

#define	PROGRAM_REGISTER_SHADER_START()		\
	program = new VideoShaderProgram();		\
	program->Initialize();
#define	PROGRAM_REGISTER_SHADER(name, type)						\
	{															\
	VideoShader *shader_ = new VideoShader(type);				\
	if(!shader_->Load(#name))									\
		Sys_Error("Failed to load "#name" vertex shader!\n");	\
	program->Attach(shader_);									\
	}
#define	PROGRAM_REGISTER_SHADER_END()	\
	program->Link()

#define	PROGRAM_REGISTER_UNIFORM(name) name = program->GetUniformLocation(#name)

class VideoShaderProgram
{
public:
	VideoShaderProgram();
	~VideoShaderProgram();

	void Initialize();
	void Attach(VideoShader *shader);
	void Enable();
	void Disable();
	void Link();
	void Shutdown();

	bool IsActive() 
	{ 
		return (Video.current_program == instance); 
	}

	void SetVariable(int location, float x, float y, float z);
	void SetVariable(int location, MathVector3f_t vector);
	void SetVariable(int location, float x, float y, float z, float a);
	void SetVariable(int location, int i);
	void SetVariable(int location, float f);

	int GetUniformLocation(const char *name);

	unsigned int GetInstance();
protected:
private:
	bool isenabled;

	unsigned int instance;
};

class VideoShaderManager
{
public:
	VideoShaderManager() 
	{
		program = NULL;
	}

	~VideoShaderManager() 
	{
		delete program;
	}

	virtual void Initialize() = 0;
	virtual void Draw() = 0;
	virtual void Shutdown() = 0;

	virtual void Enable()
	{
		if (program)
			program->Enable();
	}

	virtual void Disable()
	{
		if (program)
			program->Disable();
	}

	VideoShaderProgram *program;

	std::vector<VideoShader*> shaders;
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