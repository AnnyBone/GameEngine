/*	Copyright (C) 1996-2001 Id Software, Inc.
	Copyright (C) 2002-2009 John Fitzgibbons and others
	Copyright (C) 2011-2016 OldTimes Software

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
#include <deque>
#include <list>
#include <time.h>

namespace Core
{
	class Console
	{
		public:
			Console();

			void Clear();
			void ClearNotify();

			void Print(const char *text);

			void ScrollUp();
			void ScrollDown();
			void ScrollHome();
			void ScrollEnd();

			void InputKey(int key, bool ctrl, bool shift);
			void InputClear();

			void Draw(bool draw_input);
			void DrawNotify();

		private:
			struct Line
			{
				std::string text;
				time_t time;
				
				Line(const std::string &t):
					text(t), time(0) {}
			};

			size_t cursor_x, cursor_y;
			size_t backscroll;

			std::deque<Line> lines;

			std::list<std::string> input_history;
			std::list<std::string>::iterator input_hiter;

			std::string input_line;
			size_t input_cursor_pos;
			size_t input_draw_off;

			void linefeed();

			std::list<std::string> prepare_text(unsigned int cols, unsigned int rows);
			static std::list<std::string> wrap_line(std::string line, unsigned int cols);

			/* Tab completion state machine
			 *
			 * Initial state: tab_suggestions is empty, signifying
			 * there is no tab completion active. The other tab_
			 * values are undefined in this case.
			 *
			 * Active state: tab_suggestions is not empty, tab_siter
			 * points to the element in tab_suggestions which has
			 * been inserted into input_line, tab_insert_pos is the
			 * offset into input_line the cursor was at when we
			 * went into active state and where suggestions should
			 * be inserted at.
			 *
			 * NOTE: tab_suggestions should be cleared when normal
			 * editing is resumed.
			*/

			std::list<std::string> tab_suggestions;
			std::list<std::string>::iterator tab_siter;

			void TabComplete(bool reverse);
	};
}

extern Core::Console *con_instance;
#endif

plEXTERN_C_START

extern	bool	con_forcedup;	// because no entities to refresh
extern	bool	g_consoleinitialized;

extern uint8_t *con_chars;

extern char con_lastcenterstring[]; //johnfitz

void Console_Initialize(void);
void Con_DrawConsole(bool draw_input);
void Con_Print(char *txt);
void Con_Printf(const char *fmt, ...);
void Con_Warning(const char *fmt, ...); //johnfitz
void Con_Error(char *fmt, ...);
void Con_DPrintf(const char *fmt, ...);
void Con_SafePrintf(const char *fmt, ...);
void Con_Clear_f(void);
void Con_DrawNotify(void);
void Con_ClearNotify(void);
void Con_ToggleConsole_f(void);
void Console_ErrorMessage(bool bCrash, const char *ccFile, const char *reason);

void Con_InputKey(int key, bool ctrl, bool shift);

// Other crap...
char *Con_Quakebar(unsigned int len);
void Con_LogCenterPrint(char *str);

plEXTERN_C_END
