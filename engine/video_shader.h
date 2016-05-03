/*	DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
	Version 2, December 2004

	Copyright (C) 2011-2016 Mark E Sowden <markelswo@gmail.com>

	Everyone is permitted to copy and distribute verbatim or modified
	copies of this license document, and changing it is allowed as long
	as the name is changed.

	DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
	TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

	0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#pragma once

#ifdef __cplusplus

namespace Core
{
	class Shader : public IShader
	{
	public:
		Shader(vlShaderType_t type);
		~Shader();

		bool Load(const char *path);
		bool CheckCompileStatus();

		unsigned int GetInstance() { return instance; }
		vlShaderType_t GetType() { return type; }

	private:
		unsigned int instance;

		vlShaderType_t type;

		const char	*source;
		char		source_path[PLATFORM_MAX_PATH];
		int			source_length;
	};

	class ShaderProgram : public IShaderProgram
	{
	public:
		ShaderProgram(std::string name);
		~ShaderProgram();

		virtual void Initialize() = 0;

		void RegisterShader(std::string path, vlShaderType_t type);
		
		virtual void RegisterAttributes();

		void Attach(Core::Shader *shader);
		void Enable();
		void Disable();
		void Draw(vlDraw_t *object);
		void Link();
		void Shutdown();

		bool IsActive()	{ return (Video.current_program == instance); }

		vlUniform_t *RegisterUniform(std::string name, vlUniformType_t type);

		void SetUniformVariable(vlUniform_t *uniform, float x, float y, float z);
		void SetUniformVariable(vlUniform_t *uniform, plVector3f_t vector);
		void SetUniformVariable(vlUniform_t *uniform, float x, float y, float z, float a);
		void SetUniformVariable(vlUniform_t *uniform, int i);
		void SetUniformVariable(vlUniform_t *uniform, unsigned int i);
		void SetUniformVariable(vlUniform_t *uniform, float f);
		void SetUniformVariable(vlUniform_t *uniform, double d);

		int GetUniformLocation(std::string name);
		vlUniform_t *GetUniform(std::string name);

		void RegisterAttribute(std::string name, int location);
		void SetAttributeVariable(int location, plVector3f_t vector);
		int GetAttributeLocation(std::string name);

		unsigned int GetInstance() { return instance; }

	private:
		std::vector<Core::Shader*>						shaders;
		std::unordered_map<std::string, vlAttribute_t>	attributes;
		std::unordered_map<std::string, vlUniform_t*>	uniforms;

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

#define	SHADER_REGISTER_UNIFORM(name, type, def)		\
	{													\
		name = RegisterUniform(#name, type);			\
		SetUniformVariable(name, def);					\
	}

#define	SHADER_REGISTER_ATTRIBUTE(name, def)		\
	{												\
		auto var = GetAttributeLocation(#name);		\
		RegisterAttribute(#name, var);				\
	}

#endif