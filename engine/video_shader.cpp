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

#include "engine_base.h"

#include "video.h"
#include "video_shader.h"

#include "client/shader_base.h"
#include "client/shader_water.h"

using namespace xenon::graphics;

namespace xenon {
	namespace graphics {
		ShaderManager *shader_manager = nullptr;
	}
}

ShaderManager::ShaderManager() {
	Con_Printf("Initializing Shader Manager...\n");
	
	programs.reserve(16);

	Add(new BaseShader(), "base");
	//Add(new WaterShader(), "water");
}

ShaderManager::~ShaderManager()
{
	Clear();
}

void ShaderManager::Add(pl::graphics::ShaderProgram *program, std::string name)
{
	Con_Printf("Adding new shader: %s\n", name.c_str());

	// todo: do we really want to do this here!?
	program->Enable();
	program->Initialize();
	program->Disable();

	programs.emplace(name, program);
}

void ShaderManager::Delete(pl::graphics::ShaderProgram *program) {
	for (auto cprogram = programs.begin(); cprogram != programs.end(); ++cprogram) {
		if (cprogram->second == program) {
			delete cprogram->second;
			programs.erase(cprogram);
			break;
		}
	}
}

void ShaderManager::Delete(std::string name)
{
	auto program = programs.find(name);
	if (program != programs.end())
	{
		delete program->second;
		programs.erase(program);
	}
}

void ShaderManager::Clear()
{
	for (auto program = programs.begin(); program != programs.end(); ++program)
		delete program->second;

	programs.clear();
}

pl::graphics::ShaderProgram * ShaderManager::GetProgram(std::string name)
{
	auto program = programs.find(name);
	if (program != programs.end()) {
        return program->second;
    }

	return nullptr;
}

// Shader

Shader::Shader(pl::graphics::ShaderType type) :
	instance(0), 
	source_length(0), 
	type(type)
{
}

bool Shader::Load(const char *path)
{
#ifdef VL_MODE_OPENGL
	// Check that the path is valid.
	if (path[0] == ' ')
	{
		Con_Warning("Invalid shader path! (%s)\n", path);
		return false;
	}

	// Ensure we use the correct path and shader.
	unsigned int stype;
	switch (type)
	{
	case PL_SHADER_FRAGMENT:
		sprintf(source_path, "%s%s_fragment.shader", g_state.path_shaders, path);
		break;
	case PL_SHADER_VERTEX:
		sprintf(source_path, "%s%s_vertex.shader", g_state.path_shaders, path);
		break;
	default:throw XException("Unknown shader type! (%i) (%s)\n", type, path);
	}

	// Attempt to load it.
	source = (char*)COM_LoadHeapFile(source_path);
	if (!source || source[0] == ' ')
	{
		Con_Warning("Failed to load shader! (%s)\n", source_path);
		return false;
	}

	// Ensure it's a valid length.
#ifdef _MSC_VER
#	pragma warning(suppress: 6387)
#endif
	source_length = (PLuint)strlen(source);
	if (source_length <= 1)
	{
		Con_Warning("Invalid shader! (%i) (%s)\n", source_length, source_path);
		return false;
	}

	plCreateShader(&instance, type);
	const char *full_source[] = {
#if defined (VL_MODE_OPENGL)
		//"#version 110\n",	// OpenGL 2.0
		"#version 120\n",	// OpenGL 2.1
		//"#version 130\n",	// OpenGL 3.0
		//"#version 140\n",	// OpenGL 3.1
		//"#version 150\n",	// OpenGL 3.2
		//"#version 450\n",	// OpenGL 4.5
#elif defined (VL_MODE_OPENGL_ES)
		"#version 100\n",	// OpenGL ES 2.0
#endif
		source
	};
	// todo, introduce pre-processor to catch any custom ones (like include).
	glShaderSource(instance, 2, full_source, NULL);
	glCompileShader(instance);

	if (!CheckCompileStatus())
	{
		Con_Warning("Shader compilation failed! (%s)\n", source_path);
		return false;
	}

	// Everything worked out okay!
	return true;
#else
	return false;
#endif
}

// Compilation

bool Shader::CheckCompileStatus()
{
#ifdef VL_MODE_OPENGL
	int compile;
	glGetObjectParameterivARB(instance, GL_COMPILE_STATUS, &compile);
	if (!compile)
	{
		int length = 0;
		glGetShaderiv(instance, GL_INFO_LOG_LENGTH, &length);
		if (length > 1)
		{
			char *cLog = new char[length];
			glGetShaderInfoLog(instance, length, NULL, cLog);
			Con_Warning("%s\n", cLog);
			delete[] cLog;
		}

		return false;
	}

	return true;
#else
	return false;
#endif
}

/*	Shader Program	*/

ShaderProgram::ShaderProgram(std::string name) : 
	_name(name), 
	instance(0)
{
	plCreateShaderProgram(&instance);
	if (!instance)
		throw XException("Failed to create shader program!\n");
}

ShaderProgram::~ShaderProgram()
{
	for (auto uniform = uniforms.begin(); uniform != uniforms.end(); ++uniform)
		delete uniform->second;
	uniforms.clear();

#if 0
	for (auto attribute = attributes.begin(); attribute != attributes.end(); ++attribute)
		delete attribute->second;
#endif
	attributes.clear();

	plDeleteShaderProgram(&instance);
}

void ShaderProgram::RegisterShader(std::string path, PLShaderType type)
{
	Shader *shader_ = new Shader(type);
	if (!shader_->Load(path.c_str()))
		throw XException("Failed to load shader! (%s)\n", path.c_str());

	Attach(shader_);
}

void ShaderProgram::RegisterAttributes()
{
	// Register all the base attributes.
	RegisterAttribute("a_vertcies", GetAttributeLocation("a_vertices"));
}

void ShaderProgram::RegisterAttribute(std::string name, int location)
{
	// todo
}

void ShaderProgram::Attach(Shader *shader)
{
	if (!shader)
		throw XException("Attempted to attach an invalid shader!\n");

	vlAttachShader(instance, shader->GetInstance());
	shaders.push_back(shader);
}

void ShaderProgram::Enable()
{
	if (IsActive())
		return;

	plSetShaderProgram(instance);
}

void ShaderProgram::Disable()
{
	if (!IsActive())
		return;

	plSetShaderProgram(0);
}

void ShaderProgram::Draw(PLDraw *object)
{
#if defined (VL_MODE_OPENGL)
#if 0
	glEnableVertexAttribArray(a_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, object->_gl_vbo[0]);
	glVertexAttribPointer(
		a_vertices,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,0);
#endif

	plDraw(object);

#if 0
	glDisableVertexAttribArray(a_vertices);
#endif
#endif
}

void ShaderProgram::Link()
{
	vlLinkShaderProgram(&instance);
}

void ShaderProgram::Shutdown()
{
	Disable();
}

// Attribute Handling

PLAttribute ShaderProgram::GetAttributeLocation(std::string name)
{
	return vlGetAttributeLocation(&instance, name.c_str());
}

void ShaderProgram::SetAttributeVariable(int location, plVector3f_t vector)
{

}

// Uniform Handling

PLUniform *ShaderProgram::RegisterUniform(std::string name, PLUniformType type)
{
	if (type >= VL_UNIFORM_END)
		throw XException("Invalid unform type! (%s) (%i)\n", name.c_str(), type);

	// Ensure we don't have it registered already.
	auto uniform = uniforms.find(name);
	if (uniform != uniforms.end())
		return uniform->second;

	// Allocate a new uniform pointer.
	PLUniform *uni = new PLUniform;
	memset(uni, 0, sizeof(PLUniform));
	uni->location	= GetUniformLocation(name);
	uni->type		= type;
	uniforms.emplace(name, uni);

	return uni;
}

// Retrieves the uniform location from the shader.
int ShaderProgram::GetUniformLocation(std::string name)
{
#ifdef VL_MODE_OPENGL
	int loc = glGetUniformLocation(instance, name.c_str());
	if (loc == -1)
		Con_Warning("Failed to get uniform location! (%s)\n", name.c_str());
	return loc;
#else
	return 0;
#endif
}

PLUniform *ShaderProgram::GetUniform(std::string name)
{
	// See if we have it registered already.
	auto uniform = uniforms.find(name);
	if (uniform != uniforms.end())
		return uniform->second;

	return nullptr;
}