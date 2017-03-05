/*
DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
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

namespace xenon {
	namespace graphics {
#if 0
        class Shader : public IShader {
        public:
            Shader(pl::graphics::ShaderType type);

            ~Shader();

            bool Load(const char *path);

            bool CheckCompileStatus();

            unsigned int GetInstance() { return instance; }

            PLShaderType GetType() { return type; }

        private:
            unsigned int instance;

            PLShaderType type;

            const char *source;
            char source_path[PLATFORM_MAX_PATH];
            PLuint source_length;
        };

        class ShaderProgram : public IShaderProgram {
        public:
            ShaderProgram(std::string name);

            ~ShaderProgram();

            virtual void Initialize() = 0;

            void RegisterShader(std::string path, PLShaderType type);

            virtual void RegisterAttributes();

            void Attach(Shader *shader);

            void Enable();

            void Disable();

            void Draw(PLMesh *object);

            void Link();

            void Shutdown();

            bool IsActive() { return (plGetCurrentShaderProgram() == instance); }

            PLUniform *RegisterUniform(std::string name, PLUniformType type);

            int GetUniformLocation(std::string name);

            PLUniform *GetUniform(std::string name);

            void RegisterAttribute(std::string name, int location);

            void SetAttributeVariable(int location, plVector3f_t vector);

            int GetAttributeLocation(std::string name);

        private:
            std::vector<Shader *> shaders;
            std::unordered_map<std::string, PLAttribute> attributes;
            std::unordered_map<std::string, PLUniform *> uniforms;

            std::string _name;

            PLShaderProgram instance;
        };
#endif

        class ShaderManager : public XManager {
        public:
            ShaderManager();

            ~ShaderManager();

            void Add(pl::graphics::ShaderProgram *program, std::string name);

            void Delete(pl::graphics::ShaderProgram *program);
            void Delete(std::string name);

            void Clear();

            pl::graphics::ShaderProgram * GetProgram(std::string name);

        private:
            std::unordered_map<std::string, pl::graphics::ShaderProgram *> programs;
        };

        extern ShaderManager *shader_manager;
    }
}

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