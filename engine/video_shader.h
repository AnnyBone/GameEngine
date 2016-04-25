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
		ShaderProgram(std::string name);
		~ShaderProgram();

		virtual void Initialize() = 0;

		void RegisterShader(std::string path, vlShaderType_t type);
		void RegisterUniform(std::string name, vlUniform_t location);
		void RegisterAttribute(std::string name, vlUniform_t location);
		virtual void RegisterAttributes();

		void Attach(Core::Shader *shader);
		void Enable();
		void Disable();
		void Draw(vlDraw_t *object);
		void Link();
		void Shutdown();

		bool IsActive()	{ return (Video.current_program == instance); }

		void SetUniformVariable(vlUniform_t location, float x, float y, float z);
		void SetUniformVariable(vlUniform_t location, plVector3f_t vector);
		void SetUniformVariable(vlUniform_t location, float x, float y, float z, float a);
		void SetUniformVariable(vlUniform_t location, int i);
		void SetUniformVariable(vlUniform_t location, float f);

		void SetAttributeVariable(int location, plVector3f_t vector);

		vlUniform_t GetUniformLocation(std::string name);
		vlUniform_t GetAttributeLocation(std::string name);

		unsigned int GetInstance() { return instance; };

	private:
		bool isenabled;

		std::vector<Core::Shader*>						shaders;
		std::unordered_map<std::string, vlAttribute_t>	attributes;
		std::unordered_map<std::string, vlUniform_t>	uniforms;

		std::string name;

		vlShaderProgram_t instance;
	};

	class ShaderManager : public CoreManager
	{
	public:
		ShaderManager();
		~ShaderManager();

		void Add(ShaderProgram *program, std::string name);
		void Delete(ShaderProgram *_program);
		void Delete(std::string name);
		void Clear();

		ShaderProgram *GetProgram(std::string name);

	private:
		std::unordered_map<std::string, ShaderProgram*> programs;
	};
}

extern Core::ShaderManager *g_shadermanager;

#define	SHADER_REGISTER_UNIFORM(name, def)		\
	{											\
		auto var = GetUniformLocation(#name);	\
		RegisterUniform(#name, var);			\
		SetUniformVariable(var, def);			\
	}

#define	SHADER_REGISTER_ATTRIBUTE(name, def)		\
	{												\
		auto var = GetAttributeLocation(#name);		\
		RegisterAttribute(#name, var);				\
	}

// TODO: dumb hack because of C++ / C mixing!
typedef Core::ShaderProgram CoreShaderProgram;

#endif