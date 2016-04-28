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
	class IShader
	{
	public:
		virtual bool Load(const char *path) = 0;
		virtual bool CheckCompileStatus() = 0;

		virtual unsigned int GetInstance() = 0;

		virtual vlShaderType_t GetType() = 0;
	};

	class IShaderProgram
	{
	public:
		virtual void Initialize() = 0;

		virtual void RegisterShader(std::string, vlShaderType_t) = 0;
		virtual void RegisterAttributes() = 0;
		virtual void Link() = 0;

		virtual void Enable() = 0;
		virtual void Disable() = 0;

		virtual void Shutdown() = 0;

		virtual bool IsActive() = 0;

		virtual unsigned int GetInstance() = 0;
	};
}

typedef Core::IShaderProgram CoreShaderProgram;

#else

typedef struct CoreShaderProgram CoreShaderProgram;

#endif