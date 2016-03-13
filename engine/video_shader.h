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

#pragma once

typedef int uniform;

#ifdef __cplusplus
namespace Core
{
	class Shader
	{
	public:
		Shader(vlShaderType_t type);
		~Shader();

		bool Load(const char *path);
		bool CheckCompileStatus();

		void Enable();
		void Disable();

		unsigned int GetInstance();
		vlShaderType_t GetType();

	private:
		unsigned int instance;

		vlShaderType_t type;

		const char	*source;
		char		source_path[PLATFORM_MAX_PATH];
		int			source_length;
	};

	class ShaderProgram
	{
	public:
		ShaderProgram();
		~ShaderProgram();

		void Initialize();
		void Attach(Core::Shader *shader);
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
}

#define SHADER_IMPLEMENT(name)		\
	public:							\
	name();							\
	virtual void RegisterShaders();
#define	SHADER_REGISTER_START(name)	\
	void name::RegisterShaders() {
#define SHADER_REGISTER_END()		\
	}
#define	SHADER_REGISTER_SCRIPT(name, type)							\
	{																\
	Core::Shader *shader_ = new Core::Shader(type);					\
	if(!shader_->Load(#name))										\
		Sys_Error("Failed to load "#name" shader!\n");				\
	program->Attach(shader_);										\
	shaders.push_back(shader_);										\
	}
#define	SHADER_REGISTER_LINK()	\
	program->Link();
#define	SHADER_REGISTER_UNIFORM(name, def)		\
	name = program->GetUniformLocation(#name);	\
	program->SetVariable(name, def);

class VideoShaderManager
{
public:
	VideoShaderManager() 
	{
		program = NULL;

		shaders.reserve(2);
	}

	~VideoShaderManager() 
	{
		delete program;
	}

	virtual void Initialize();
	virtual void Shutdown();

	virtual void RegisterShaders() = 0;

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

	Core::ShaderProgram *program;

	std::vector<Core::Shader*> shaders;
};

namespace Core
{
	class ShaderManager : public CoreManager
	{
	public:
		ShaderManager();
		~ShaderManager();

		void Initialize();
		void Shutdown();

	protected:
	private:
		std::vector<ShaderProgram*> programs;
	};
}
#endif

plEXTERN_C_START

void Shader_Initialize(void);

extern int iDiffuseUniform;

plEXTERN_C_END